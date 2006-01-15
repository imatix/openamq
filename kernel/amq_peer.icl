<?xml?>
<class
    name      = "amq_peer"
    comment   = "Cluster peer class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements the cluster peer, which handles the dialogue
between the current server and the cluster. The peer sends requests
to the peer agent and replies are handled by callbacks back via the
cluster class.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "list" />
    <option name = "rwlock" value = "0" />
</inherit>

<import class = "amq_server_classes" />

<public name = "header">
#define AMQ_PEER_PENDING    0           //  Cluster node never connected
#define AMQ_PEER_PRIMARY    1           //  Primary node is alive and online
#define AMQ_PEER_OFFLINE    2           //  Cluster node has gone offline
#define AMQ_PEER_SECONDARY  3           //  Secondary node, can't vote
</public>

<context>
    amq_cluster_t
        *cluster;                       //  Parent cluster controller
    smt_thread_t
        *thread;                        //  Active agent thread if any
    dbyte
        channel_nbr;                    //  Active channel number
    icl_shortstr_t
        hostname;                       //  Hostname to connect to
    Bool
        connected;                      //  Connected to cluster node
    icl_shortstr_t
        spid;                           //  Remote node's server process id
    Bool
        primary,                        //  Are we in the primary partition?
        loopback,                       //  Are we looping back to ourselves?
        offlined;                       //  Has node gone offline?
    Bool
        duplicate;                      //  Mark peer for destruction
    qbyte
        last_sequence;                  //  Last publish method seen
</context>              

<method name = "new">
    <argument name = "cluster"  type = "amq_cluster_t *">Parent cluster controller</argument>
    <argument name = "hostname" type = "char *">Server to connect to</argument>
    <argument name = "primary"  type = "Bool">Primary node?</argument>
    self->cluster = cluster;
    self->primary = primary;
    
    self->channel_nbr = 1;              //  Single channel per connection

    icl_shortstr_cpy (self->hostname, hostname);
</method>

<method name = "destroy">
    self_disconnect (self);
</method>

<method name = "status" template = "function">
    <doc>
    Reports the node status, automatically connecting or disconnecting as
    nodes come and go from the cluster.
    </doc>
    //
    if (self->thread) {
        if (self->thread->zombie)
            self_disconnect (self);
    }
    else
    if (ipr_net_ping (self->hostname))
        self_connect (self);

    if (!self->primary)
        rc = AMQ_PEER_SECONDARY;
    else
    if (self->connected)
        rc = AMQ_PEER_PRIMARY;
    else
    if (self->offlined)
        rc = AMQ_PEER_OFFLINE;
    else
        rc = AMQ_PEER_PENDING;
</method>

<method name = "connect" template = "function" private = "1">
    <doc>
    Starts a peer agent thread to connect to the cluster node.
    </doc>
    <local>
    icl_longstr_t
        *auth_data;                     //  Authorisation data
    </local>
    //
    assert (!self->thread);
    auth_data = self_auth_plain ("cluster", "cluster");
    self->thread = amq_proxy_agent_connection_thread_new (
        self,                           //  Callback for incoming methods
        self->hostname,
        amq_server_config_cluster_vhost (amq_server_config),
        auth_data,
        amq_server_config_trace (amq_server_config));

    amq_proxy_agent_channel_open (self->thread, self->channel_nbr);
    icl_longstr_destroy (&auth_data);
</method>

<method name = "disconnect" template = "function" private = "1">
    <doc>
    Disconnects from the cluster node.
    </doc>
    //
    if (self->thread) {
        if (!self->zombie)
            amq_proxy_agent_connection_close (self->thread);
        smt_thread_unlink (&self->thread);
        if (self->connected) {
            self->connected = FALSE;
            self->offlined = TRUE;
        }
    }
</method>

<method name = "vote" template = "function">
    <doc>
    Returns the vote from the current node.  A node will vote for us if
    we are the senior node (lower spid).  We don't need to ask the node
    anything since it told us its spid at connection time.  We always
    vote for ourselves.
    </doc>
    //
    assert (self->primary);
    if (self->connected && strcmp (self->spid, amq_broker->spid) >= 0)
        rc = 1;                         //  We are more senior than the node
    else
        rc = 0;                         //  Nack or unknown means vote 'no'
</method>

<method name = "auth plain" return = "auth data" private = "1">
    <doc>
    Formats a new authentication data block for a plain login.  The protocol
    uses a SASL-style binary data block for authentication.  This method is
    a simple way of turning a name and password into such a block.  Note
    that plain authentication data is not encrypted and does not provide
    any degree of confidentiality.
    </doc>
    <argument name = "login"    type = "char *">User login name</argument>
    <argument name = "password" type = "char *">User password</argument>
    <declare name = "auth data" type = "icl_longstr_t *">Authentication data</declare>
    <local>
    asl_field_list_t
        *field_list;
    </local>
    field_list = asl_field_list_new (NULL);
    asl_field_new_string (field_list, "login",    login);
    asl_field_new_string (field_list, "password", password);
    auth_data = asl_field_list_flatten (field_list);
    asl_field_list_destroy (&field_list);
</method>

<method name = "joined" template = "function">
    <doc>
    Registers the peer as connected.  This method comes from the proxy
    agent via the cluster object, asynchronously.
    </doc>
    <argument name = "spid" type = "char *">Node's spid</argument>
    <local>
    ipr_looseref_t
        *looseref;                      //  State object
    amq_content_cluster_t
        *content;                       //  Server state message
    </local>
    //
    self->connected = TRUE;
    self->offlined = FALSE;
    icl_shortstr_cpy (self->spid, spid);
    if (streq (self->spid, amq_broker->spid))
        self->loopback = TRUE;

    icl_console_print ("I: cluster - now talking to %s%s",
        self->hostname, self->loopback? " (loopback)": "");

    if (self->cluster->root) {
        //  Now synchronise new peer with our state
        looseref = ipr_looseref_list_first (self->cluster->state_list);
        while (looseref) {
            content = (amq_content_cluster_t *) (looseref->object);
            if (amq_server_config_trace_cluster (amq_server_config))
                icl_console_print ("C: replay   method=%s", content->method_name);
            amq_peer_proxy (self, content, NULL, 0);
            looseref = ipr_looseref_list_next (&looseref);
        }
    }
</method>

<method name = "proxy" template = "function">
    <doc>
    Sends a message to the cluster service of the specified peer.
    </doc>
    <argument name = "content" type = "amq_content_cluster_t *">Method to send</argument>
    <argument name = "client connection" type = "char *" >Client connection</argument>
    <argument name = "client channel" type = "int" >Client channel</argument>
    //
    if (self->connected) {
        if (amq_server_config_trace_cluster (amq_server_config))
            icl_console_print ("C: proxy    method=%s peer=%s", content->method_name, self->spid);

        amq_proxy_agent_cluster_proxy (
            self->thread,
            self->channel_nbr,
            content,
            client_connection,
            client_channel);
    }
</method>

<method name = "push" template = "function">
    <doc>
    Pass already-formatted method to the specified peer.  Detects and
    eliminates duplicates of same message sent to same peer.
    </doc>
    <argument name = "method" type = "amq_server_method_t *">Publish method</argument>
    //
    if (self->connected) {
        if (self->last_sequence != method->sequence) {
            if (amq_server_config_trace_cluster (amq_server_config))
                icl_console_print ("C: passthru method=%s peer=%s",
                    method->name, self->spid);
            amq_proxy_agent_push (
                self->thread,
                self->channel_nbr,
                (amq_proxy_method_t *) method);
        }
        else
            icl_console_print ("I: drop method=%s peer=%s seq=%ld",
                method->name, self->spid, method->sequence);

        self->last_sequence = method->sequence;
    }
    else
        icl_console_print ("W: drop unconnected method=%s peer=%s seq=%ld",
            method->name, self->spid, method->sequence);
</method>

<method name = "selftest" />

</class>
