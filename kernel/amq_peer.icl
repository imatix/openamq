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

<option name = "links" value = "1"/>

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
        name,                           //  Peer server name
        host;                           //  Peer host name
    Bool
        connected,                      //  Peer is connected?
        joined,                         //  Peer has shown signs of life?
        offlined,                       //  Peer has gone offline
        primary,                        //  Peer is the primary node?
        backup,                         //  Peer is the backup node
        master;                         //  Peer is master node
    int
        heartbeat,                      //  Heartbeat in seconds
        heartbeat_timer,                //  Count-down timer for heartbeat
        heartbeat_ttl,                  //  TTL in heartbeats, zero = dead
        load;                           //  Peer load, connections
    qbyte
        last_sequence;                  //  Last publish method seen
</context>

<method name = "new">
    <argument name = "cluster" type = "amq_cluster_t *">Parent cluster controller</argument>
    <argument name = "name" type = "char *">Peer server name</argument>
    <argument name = "host" type = "char *">Peer host name</argument>
    <argument name = "primary" type = "Bool">Peer is primary?</argument>
    <argument name = "backup" type = "Bool">Peer is backup?</argument>
    //
    self->channel_nbr = 1;              //  Single channel per connection
    self->cluster = cluster;
    self->primary = primary;
    self->backup  = backup;
    icl_shortstr_cpy (self->name, name);
    icl_shortstr_cpy (self->host, host);
</method>

<method name = "destroy">
    self_disconnect (self);
</method>

<method name = "monitor" template = "function">
    <doc>
    Monitor the peer node status. If the node was offline and has come
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
    smt_thread_t
        *thread;
    </local>
    //
    thread = smt_thread_link (self->thread);
    if (thread) {
        if (thread->zombie)
            self_disconnect (self);
        else
        if (self->connected) {
            self->heartbeat_timer--;
            if (self->heartbeat_timer == 0) {
                self_send_status (self);

                //  If no reply received within X heartbeats, disconnect
                self->heartbeat_timer = self->heartbeat;
                self->heartbeat_ttl--;
                if (self->heartbeat_ttl == 0) {
                    smt_log_print (amq_broker->alert_log,
                        "E: cluster - peer '%s' not responding, assuming failed",
                        self->name);
                    self_disconnect (self);
                }
            }
        }
        smt_thread_unlink (&thread);
    }
    else
        self_connect (self);
</method>

<method name = "connect" template = "function" private = "1">
    <doc>
    Starts a peer agent thread to connect to the cluster node.
    </doc>
    <local>
    icl_longstr_t
        *auth_data;                     //  Authorisation data
    ipr_config_t
        *config;                        //  Current server config file
    </local>
    //
    assert (!self->thread);

    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/security", "plain");
    if (config->located)
        ipr_config_locate (config, "user", "cluster");

    if (config->located) {
        auth_data = amq_client_connection_auth_plain ("cluster", ipr_config_get (config, "password", ""));
        self->thread = amq_proxy_agent_connection_thread_new (
            self,                       //  Callback for incoming methods
            self->host,
            amq_server_config_cluster_vhost (amq_server_config),
            auth_data,
            "Cluster connection",       //  Instance name
            amq_server_config_trace (amq_server_config));

        amq_proxy_agent_channel_open (self->thread, self->channel_nbr);
        icl_longstr_destroy (&auth_data);
    }
    else
        icl_console_print ("W: no cluster login defined, cannot join cluster");

    ipr_config_destroy (&config);
</method>

<method name = "disconnect" template = "function" private = "1">
    <doc>
    Disconnects from the cluster node.
    </doc>
    <local>
    amq_exchange_t
        *exchange;
    amq_vhost_t
        *vhost;
    </local>
    //
    if (self->thread) {
        if (!self->zombie)
            amq_proxy_agent_connection_close (self->thread);
        smt_thread_unlink (&self->thread);
    }
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost && self->connected) {
        //  Go through all exchanges & bindings, remove link to peer
        exchange = amq_exchange_by_vhost_first (vhost->exchange_list);
        while (exchange) {
            amq_exchange_unbind_peer (exchange, self);
            exchange = amq_exchange_by_vhost_next (&exchange);
        }
        self->connected = FALSE;
        self->joined    = FALSE;
        self->offlined  = TRUE;
        icl_console_print ("I: cluster - disconnected from %s", self->host);
    }
    amq_vhost_unlink (&vhost);
</method>

<method name = "ready" template = "function">
    <doc>
    Registers the peer as connected. This method comes from the proxy
    agent via the cluster object, asynchronously.
    </doc>
    <argument name = "name" type = "char *">Node's server name</argument>
    <local>
    ipr_looseref_t
        *looseref;                      //  State object
    amq_content_tunnel_t
        *content;                       //  Server state message
    </local>
    //
    self->connected = TRUE;
    self->offlined = FALSE;
    self->master = FALSE;               //  Until we get a new status
    smt_log_print (amq_broker->alert_log,
        "I: cluster - connected to %s", self->host);

    self->heartbeat = amq_server_config_cluster_heartbeat (amq_server_config);
    self->heartbeat_timer = self->heartbeat;

    //  Send a status message immediately to check our configuration
    self_send_hello (self);

    //  Synchronise our state to the new peer
    if (amq_server_config_debug_cluster (amq_server_config))
        smt_log_print (amq_broker->debug_log, "C: cluster synchronising with new peer");
    looseref = ipr_looseref_list_first (self->cluster->state_list);
    while (looseref) {
        content = (amq_content_tunnel_t *) (looseref->object);
        if (amq_server_config_debug_cluster (amq_server_config))
            smt_log_print (amq_broker->debug_log, "C: replay   method=%s", content->data_name);
        icl_shortstr_cpy (content->proxy_name, self->cluster->broker->name);
        amq_peer_tunnel (self, content);
        looseref = ipr_looseref_list_next (&looseref);
    }
</method>

<method name = "send hello" template = "function">
    <doc>
    Send a Cluster.Hello method to the peer.
    </doc>
    <local>
    amq_server_method_t
        *method;
    </local>
    //
    method = amq_server_method_new_cluster_hello (
        AMQ_CLUSTER_VERSION,
        amq_broker->name,
        amq_server_config_cluster_vhost (amq_server_config),
        self->cluster->crc->value);
    amq_cluster_tunnel_out (self->cluster, self, method, FALSE, NULL);
    amq_server_method_unlink (&method);
</method>

<method name = "recv hello" template = "function">
    <doc>
    We have received a Cluster.Hello method from a specific peer.
    </doc>
    <argument name = "method" type = "amq_server_cluster_hello_t *">Method</argument>
    //
    //  Double-check that we agree with peer on cluster settings
    if (method->version != AMQ_CLUSTER_VERSION) {
        smt_log_print (amq_broker->alert_log,
            "E: cluster - peer has wrong version %04x (expect %04x)",
            method->version, AMQ_CLUSTER_VERSION);
        self_disconnect (self);
    }
    else
    if (strneq (method->name, self->name)) {
        smt_log_print (amq_broker->alert_log,
            "E: cluster - peer using wrong name %s (expect %s)", method->name, self->name);
        self_disconnect (self);
    }
    else
    if (method->signature != self->cluster->crc->value) {
        smt_log_print (amq_broker->alert_log,
            "E: cluster - peer has wrong signature %04x (expect %04x)",
            method->signature, self->cluster->crc->value);
        self_disconnect (self);
    }
    else
    if (strneq (method->vhost, amq_server_config_cluster_vhost (amq_server_config))) {
        smt_log_print (amq_broker->alert_log,
            "E: cluster - peer using wrong vhost %s (expect %s)",
            method->vhost, amq_server_config_cluster_vhost (amq_server_config));
        self_disconnect (self);
    }
</method>

<method name = "send status" template = "function">
    <doc>
    Send a Cluster.Status method to the peer.
    </doc>
    <local>
    amq_server_method_t
        *method;
    </local>
    //
    method = amq_server_method_new_cluster_status (
        amq_broker->master,
        amq_server_config_cluster_heartbeat (amq_server_config),
        amq_server_connection_count (),
        amq_content_basic_count (),
        ipr_bucket_used () / (1024 * 1024),
        amq_exchange_count (),
        amq_queue_count (),
        amq_consumer_count (),
        amq_binding_count (),
        amq_peer_count ());

    amq_cluster_tunnel_out (self->cluster, self, method, FALSE, NULL);
    amq_server_method_unlink (&method);
</method>

<method name = "recv status" template = "function">
    <doc>
    We have received a Cluster.Status method from a specific peer.
    </doc>
    <argument name = "method" type = "amq_server_cluster_status_t *">Method</argument>
    //
    //  Normalise heartbeat between peers
    self->joined = TRUE;
    self->heartbeat = (amq_server_config_cluster_heartbeat (amq_server_config)
                    + method->heartbeat) / 2;
    self->heartbeat_ttl = AMQ_HEARTBEAT_TTL;
    self->load   = method->connections;
    self->master = method->master;
</method>

<method name = "tunnel" template = "function">
    <doc>
    Sends a message to the cluster service of the specified peer.
    </doc>
    <argument name = "content" type = "amq_content_tunnel_t *">Data to send</argument>
    //
    if (self->connected) {
        if (amq_server_config_debug_cluster (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "C: proxy    method=%s peer=%s", content->data_name, self->name);
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
            if (amq_server_config_debug_cluster (amq_server_config))
                smt_log_print (amq_broker->debug_log,
                    "C: passthru method=%s peer=%s", method->name, self->name);
            amq_proxy_agent_push (
                self->thread,
                self->channel_nbr,
                (amq_proxy_method_t *) method);
        }
        else
            smt_log_print (amq_broker->alert_log,
                "W: cluster dropped method=%s peer=%s seq=%d reason=duplicate",
                method->name, self->name, method->sequence);

        self->last_sequence = method->sequence;
    }
    else
        smt_log_print (amq_broker->alert_log,
            "W: cluster dropped method=%s peer=%s seq=%d reason=unconnected",
            method->name, self->name, method->sequence);
</method>

<method name = "selftest" />

</class>
