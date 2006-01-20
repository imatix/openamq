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
<inherit class = "icl_tracker" />

<import class = "amq_server_classes" />

<public name = "header">
#define AMQ_PEER_PENDING    0           //  Cluster node never connected
#define AMQ_PEER_PRIMARY    1           //  Primary node is alive and online
#define AMQ_PEER_OFFLINE    2           //  Cluster node has gone offline
#define AMQ_PEER_SECONDARY  3           //  Secondary node, can't vote

#define AMQ_HEARTBEAT_TTL   3           //  3 heartbeats lost, we're dead
</public>

<context>
    amq_cluster_t
        *cluster;                       //  Parent cluster controller
    smt_thread_t
        *thread;                        //  Active agent thread if any
    dbyte
        channel_nbr;                    //  Active channel number
    icl_shortstr_t
        host;                           //  Host to connect to
    Bool
        connected;                      //  Connected to cluster node
    int
        heartbeat,                      //  Heartbeat in seconds
        heartbeat_timer,                //  Count-down timer for heartbeat
        heartbeat_ttl;                  //  TTL in heartbeats, zero = dead
    icl_shortstr_t
        spid;                           //  Remote node's server process id
    Bool
        primary,                        //  Are we in the primary partition?
        loopback,                       //  Are we looping back to ourselves?
        offlined;                       //  Has node gone offline?
    int
        load;                           //  Peer load, connections
    Bool
        duplicate;                      //  Mark peer for destruction
    qbyte
        last_sequence;                  //  Last publish method seen
</context>              

<method name = "new">
    <argument name = "cluster"  type = "amq_cluster_t *">Parent cluster controller</argument>
    <argument name = "host"     type = "char *">Server to connect to</argument>
    <argument name = "primary"  type = "Bool">Primary node?</argument>
    self->cluster = cluster;
    self->primary = primary;
    self->channel_nbr = 1;              //  Single channel per connection
    icl_shortstr_cpy (self->host, host);
</method>

<method name = "destroy">
    self_disconnect (self);
</method>

<method name = "update" template = "function">
    <doc>
    Update the peer node status. If the node was offline and has come
    online, connect to it, and if the node appears to be active, send
    it a status message.

    The heartbeating model works thus:
    - the internal hearbeat of the cluster monitor is 1 second.
    - each second, the cluster monitor will check the status of each
      cluster peer (this method).
    - at a calculated interval, the monitor will send a status command
      to each peer.
    - if, after some calculated interval, the monitor has not received
      a status command from a peer, it will disconnect and attempt to
      reconnect to that peer.

    Each peer has a configured 'heartbeat interval', which it broadcasts
    to all other peers except itself. The actual heartbeat used between
    any two peers, in both directions, is the mean of the two peers'
    heartbeat intervals.  This model allows heartbeating to be tuned in
    a cluster that has both LAN and WAN components.
    </doc>
    <local>
    amq_server_method_t
        *method;
    </local>
    //
    if (self->thread) {
        if (self->thread->zombie)
            self_disconnect (self);
        else
        if (!self->loopback) {
            self->heartbeat_timer--;
            if (self->heartbeat_timer == 0) {
                self->heartbeat_timer = self->heartbeat;
                //  Send our status to the peer, so it knows we're alive
                method = amq_server_method_new_cluster_status (
                    amq_server_config_cluster_heartbeat (amq_server_config),
                    amq_server_connection_count (),
                    amq_content_basic_count (),
                    icl_mem_used () / (1024 * 1024),
                    amq_exchange_count (),
                    amq_queue_count (),
                    amq_consumer_count (),
                    amq_binding_count (),
                    amq_peer_count (),
                    self->cluster->primary_nodes,
                    self->primary,
                    self->cluster->root);

                amq_cluster_proxy_peer (amq_cluster, self, method);
                amq_server_method_destroy (&method);

                //  If no reply received within X heartbeats, disconnect
                self->heartbeat_ttl--;
                if (self->heartbeat_ttl == 0)
                    self_disconnect (self);
            }
        }
    }
    else
    if (ipr_net_ping (self->host, NULL))
        self_connect (self);
</method>

<method name = "status" template = "function">
    <doc>
    Reports the last-known node status.
    </doc>
    //
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
        self->host,
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
    amq_content_tunnel_t
        *content;                       //  Server state message
    </local>
    //
    self->connected = TRUE;
    self->offlined  = FALSE;
    self->heartbeat = amq_server_config_cluster_heartbeat (amq_server_config);
    self->heartbeat_timer = self->heartbeat;

    icl_shortstr_cpy (self->spid, spid);
    if (streq (self->spid, amq_broker->spid))
        self->loopback = TRUE;

    icl_console_print ("I: cluster - now talking to %s%s",
        self->host, self->loopback? " (loopback)": "");

    if (self->cluster->root) {
        //  Now synchronise new peer with our state
        looseref = ipr_looseref_list_first (self->cluster->state_list);
        while (looseref) {
            content = (amq_content_tunnel_t *) (looseref->object);
            if (amq_server_config_trace_cluster (amq_server_config))
                icl_console_print ("C: replay   method=%s", content->data_name);
            amq_peer_tunnel (self, content);
            looseref = ipr_looseref_list_next (&looseref);
        }
    }
</method>

<method name = "heartbeat" template = "function">
    <doc>
    We have received a Cluster.Status method from a specific peer.
    </doc>
    <argument name = "method" type = "amq_server_cluster_status_t *">Method</argument>
    //
    //  Heartbeat in both directions will average out to be the same
    self->heartbeat = (amq_server_config_cluster_heartbeat (amq_server_config)
                     + method->heartbeat) / 2;
    self->heartbeat_ttl = AMQ_HEARTBEAT_TTL;
    self->load = method->connections;
</method>

<method name = "tunnel" template = "function">
    <doc>
    Sends a message to the cluster service of the specified peer.
    </doc>
    <argument name = "content" type = "amq_content_tunnel_t *">Data to send</argument>
    //
    if (self->connected) {
        if (amq_server_config_trace_cluster (amq_server_config))
            icl_console_print ("C: proxy    method=%s peer=%s", content->data_name, self->spid);
        amq_proxy_agent_tunnel_request (self->thread, self->channel_nbr, content, NULL);
    }
</method>

<method name = "push" template = "function">
    <doc>
    Pass already-formatted method to the specified peer. Detects and
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
