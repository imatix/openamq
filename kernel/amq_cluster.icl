<?xml?>
<class
    name      = "amq_cluster"
    comment   = "AMQ cluster controller class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
The algorithm is:

1. Failover from primary to backup is automatic, recovery to primary happens
   manually, by stopping the backup server after the primary server has been
   restarted.

2. At startup the primary server becomes active, the backup does not.

3. If the backup server sees the primary server going away and if it has at
   least one connected client, it becomes active and remains active until
   it is stopped. If the primary server goes away, and the backup server
   has no clients, it cedes its position as active.

4. If the primary server sees the backup server going away, and it is not
   active, it becomes active. If the primary server sees the backup coming
   back, as active, the primary stops being active.

When a client application connects to the passive server, and the other server
has correctly identified itself as active, the passive server redirects the
client application to the active server.


Cluster configuration:
    - shared config files between both servers
        - primary server config
            - cluster ip address
            - application ip address
        - backup server config
            - cluster ip address
            - application ip address
    - command-line
        - select primary/backup/alone
        - specify broker name (also non-cluster)

amq_cluster implementation:
    new
        - create amq_cluster exchange
            - hard-coded to always send message to amq_cluster object
        - parse config, get primary & backup details
        - if standalone, forget it
        - create peering to other server
        - send hello message to peering
            - function to tunnel cluster message
            - wrap as basic content
            - publish to amq.cluster on peering
    monitor
        - set-up 1-second async monitor cycle
        - send status message to peering
    destroy
        - unlink amq_cluster exchange
        - stop / unlink peering

amq_cluster_exchange implementation:
    - get basic message, unpack it
    - if hello, invoke amq_cluster_hello method
        - tells cluster that peer is online
    - if status, invoke amq_cluster_status method
        - tells cluster about peer status

Broker status:
    - active/passive
        - invoke method on broker to change state
        - display message to console, show on console
    - if standalone, cluster sets broker->active
    - when incoming connection, if !broker->active
        - then forward to other broker
        -> who holds/calculates this address?




        peering = amq_peering_new (peer server, vhost-key, tracelevel);
        amq_peering_set_login (peering, "peering");

        //  Register handlers, with no object reference, for this test
        amq_peering_set_status_handler  (peering, s_test_status_handler, NULL);
        amq_peering_set_content_handler (peering, s_test_content_handler, NULL);

        //  Start the peering - connects to the AMQP server
        amq_peering_start (peering);

        content = amq_content_basic_new ();
        amq_peering_forward (peering, "amq.cluster", NULL, content);
        amq_content_basic_unlink (&content);

        amq_peering_unlink (&peering);


    - create amq.cluster exchange
        - destroy at shutdown


</doc>

<inherit class = "smt_object" />
<inherit class = "amq_console_object" />

<data name = "cml">
    <class name = "cluster" parent = "broker" label = "Cluster Controller" >
        <field name = "name" label = "Cluster name">
          <get>icl_shortstr_cpy (field_value, amq_broker->name);</get>
        </field>
        <field name = "hosts" label = "Known hosts">
          <get>icl_shortstr_cpy (field_value, self->known_hosts);</get>
        </field>
        <field name = "enabled" type = "bool" label = "Cluster is enabled">
          <get>icl_shortstr_fmt (field_value, "%d", self->enabled);</get>
        </field>
        <field name = "ready" type = "bool" label = "Cluster is ready">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->ready);</get>
        </field>
        <field name = "active" type = "bool" label = "Active broker?">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", self->active);</get>
        </field>
        <field name = "type" label = "Broker type">
          <get>icl_shortstr_cpy (field_value, self->primary? "primary": "backup");</get>
        </field>
    </class>
</data>

<import class = "amq_server_classes" />

<public name = "header">
#define AMQ_CLUSTER_VERSION     0x200   //  2.0a
#define AMQ_CLUSTER_ACTIVE      (void *) 1
#define AMQ_CLUSTER_ALL         0

extern amq_cluster_t
    *amq_cluster;                       //  Single broker, self
</public>

<private>
amq_cluster_t
    *amq_cluster = NULL;
</private>

<context>
    amq_broker_t
        *broker;                        //  Hold onto patent broker
    amq_peering_t
        *peering;                       //  We talk to 0 or 1 other server

    //tbcu
    icl_shortstr_t
        known_hosts;                    //  List of known hosts
    Bool
        failed,                         //  Cluster startup failed
        enabled,                        //  Cluster is enabled
        ready,                          //  Cluster is ready for use
        primary,                        //  We're the primary node
        active;                         //  We're active
    amq_peer_t
        *primary_peer,                  //  Primary server, if not us
        *backup_peer,                   //  Backup server, if not us
        *active_peer;                   //  active server, if not us
</context>

<method name = "new">
    <argument name = "broker" type = "amq_broker_t *">Parent broker</argument>
    <argument name = "cluster name" type = "char *">Cluster name</argument>
    //
    self->broker = amq_broker_link (broker);
    if (cluster_name) {
        icl_shortstr_cpy (amq_broker->name, cluster_name);
        amq_cluster_start (self);
    }
    else
        icl_shortstr_cpy (amq_broker->name, "Standalone");
</method>

<method name = "destroy">
    <action>
    //
    s_stop_cluster (self);
    amq_broker_unlink (&self->broker);
    </action>
</method>

<method name = "start" template = "async function" async = "1">
    <doc>
    Start the cluster.  This can be done at any time, if the
    cluster is not already started.
    </doc>
    //
    <action>
    ipr_config_t
        *config;                        //  Server config tree
    char
        *name = NULL,                   //  Peer server name
        *host = NULL,                   //  Peer external name
        *internal = NULL;               //  Peer internal name
    amq_peer_t
        *peer;                          //  Cluster peer
    Bool
        is_primary,                     //  Peer is primary server
        name_valid;                     //  Own broker name is valid
    int
        primaries = 0,                  //  Number of primary servers
        backups = 0;                    //  Number of backup server

    s_stop_cluster (self);
    self->enabled    = TRUE;

    //  Load and check cluster configuration
    name_valid = FALSE;
    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/cluster", NULL);
    if (config->located) {
        ipr_config_locate (config, "server", NULL);
        while (config->located) {
            name = ipr_config_get (config, "name", "");
            host = ipr_config_get (config, "host", "");
            internal = ipr_config_get (config, "internal", host);

            is_primary = atoi (ipr_config_get (config, "primary", "0")) > 0;
            if (is_primary)
                primaries++;
            else
                backups++;

            if (strnull (name) || strnull (host))
                asl_log_print (amq_broker->alert_log,
                    "E: cluster - server needs 'name' and valid 'host', skipped");
            else
            if (!ipr_net_validate_addr (host))
                asl_log_print (amq_broker->alert_log,
                    "E: cluster - please use a valid 'host' address, '%s' was skipped", host);
            else
            if (!ipr_net_validate_addr (internal))
                asl_log_print (amq_broker->alert_log,
                    "E: cluster - please use a valid 'internal' address, '%s' was skipped", internal);
            else {

                //  We do not create a peer for ourselves
                if (streq (name, amq_broker->name)) {
                    self->primary = is_primary;
                    name_valid = TRUE;
                }
                else {
                    if (amq_server_config_debug_cluster (amq_server_config))
                        asl_log_print (amq_broker->debug_log,
                            "C: server name=%s host=%s internal=%s", name, host, internal);

                    if (*self->known_hosts)
                        icl_shortstr_cat (self->known_hosts, " ");
                    icl_shortstr_cat (self->known_hosts, host);

                    peer = amq_peer_new (self, name, internal, is_primary);
                    if (is_primary)
                        self->primary_peer = peer;
                    else
                        self->backup_peer = peer;

                    amq_peer_unlink (&peer);
                }
            }
            ipr_config_next (config);
        }
    }
    ipr_config_destroy (&config);

    //  Set initial active to primary node
    self->active = FALSE;
    self->active_peer = self->primary_peer;

    //  We go through an asynchronous method so that it can send timer
    //  events to the cluster agent. A cluster of one is allowed.

    if (primaries != 1)
        icl_console_print ("E: cluster - exactly one primary must be defined (have %d)", primaries);
    else
    if (backups != 1)
        icl_console_print ("E: cluster - multiple backup servers not allowed (have %d)", backups);
    else
    if (name_valid) {
        amq_proxy_agent_init ();
        asl_log_print (amq_broker->alert_log,
            "I: cluster - own server name is '%s'", amq_broker->name);
        smt_timer_request_delay (self->thread, 100 * 1000, monitor_event);
    }
    else
        icl_console_print ("E: cluster - '%s' not configured server name", amq_broker->name);
    </action>
</method>

<!--
    This is the cluster monitor, which is invoked once per second to
    update the cluster status.  It rechecks each peer and counts the
    votes to decide whether this broker should become active or not.
 -->

<event name = "monitor">
    <action>
    amq_peer_t
        *peer;                          //  Cluster peer
    int
        current_actives = 0;            //  How many actives
    Bool
        cluster_alive = TRUE;           //  Is cluster alive?

    //  Update all peers' status
    while (peer) {
        amq_peer_monitor (peer);
        if (peer->active)
            current_actives++;

        if (peer->joined) {
            if (self->primary) {
                //  If we're primary, and backup is active, we stop being active
                if (peer->backup && peer->active) {
                    self->active = FALSE;
                    self->active_peer = peer;
                }
            }
            else
            if (self->backup) {
                //  If we're backup, and primary is active, leave things alone
                if (peer->primary && peer->active)
                    ;   //  Primary peer should itself stop being active
            }
            else {
                //  If we're support, complain if we detect multiple actives
                if (current_actives > 1) {
                    asl_log_print (amq_broker->alert_log,
                        "E: cluster - multiple actives detected");
                    self->active_peer = NULL;
                }
            }
        }
        else
        if (peer->offlined) {
            if (self->primary) {
                //  If we're primary and backup goes offline, we become active
                if (peer->backup && peer->active) {
                    self->active = TRUE;
                    peer->active = FALSE;
                }
            }
            else
            if (self->backup) {
                //  If we're backup and primary goes offline, we become
                //  active if/when we have at least one client connection.
                if (peer->primary && peer->active) {
                    if (amq_server_connection_count ()) {
                        self->active = TRUE;
                        peer->active = FALSE;
                    }
                    else {
                        //  Only log this message the first time
                        if (self->active_peer)
                            asl_log_print (amq_broker->alert_log,
                                "I: cluster - can't become active without connected clients");
                        self->active_peer = NULL;
                    }
                }
            }
            else {
                //  If we're support, and active goes offline, we suspend cluster
                if (peer->active) {
                    asl_log_print (amq_broker->alert_log,
                        "I: cluster - no active server present");
                    self->active_peer = NULL;
                    peer->active = FALSE;
                }
            }
        }
        else
            cluster_alive = FALSE;      //  Waiting for cluster to start fully
    }
    //  If cluster started and there are no actives, primary becomes active
    if (cluster_alive && current_actives == 0 && self->primary)
        self->active = TRUE;

    //  Set new calculated active state
    if (self->active) {
        if (!amq_broker->active)
            asl_log_print (amq_broker->alert_log,
                "I: ********************  active ON  *********************");
        amq_broker->active = TRUE;
        self->active_peer = NULL;
    }
    else {
        if (amq_broker->active)
            asl_log_print (amq_broker->alert_log,
                "I: ********************  active OFF  ********************");
        amq_broker->active = FALSE;
        if (!self->active_peer)
            cluster_alive = FALSE;      //  No active means cluster is inoperable
    }
    if (cluster_alive && !self->ready) {
        self->ready = TRUE;         //  Cluster is now ready for use
        asl_log_print (amq_broker->alert_log,
            "I: *****  CLUSTER VHOST '%s' READY FOR CONNECTIONS  *****",
            amq_server_config_cluster_vhost (amq_server_config));
    }
    else
    if (!cluster_alive && self->ready) {
        self->ready = FALSE;
        asl_log_print (amq_broker->alert_log,
            "I: *****  CLUSTER VHOST '%s' STOPPING NEW CONNECTIONS  *****",
            amq_server_config_cluster_vhost (amq_server_config));
    }
    //  Cluster monitor runs once per second
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<method name = "balance client" template = "async function" async = "1">
    <doc>
    Redirect client application to one of the cluster peers. Selects
    the cluster peer with the fewest clients.
    </doc>
    <argument name = "connection" type = "amq_server_connection_t *">client connection</argument>
    //
    <possess>
    connection = amq_server_connection_link (connection);
    </possess>
    <release>
    amq_server_connection_unlink (&connection);
    </release>
    //
    <action>
    amq_peer_t
        *peer,
        *best_peer = NULL;
    int
        lowest_load = amq_server_connection_count ();

    while (peer) {
        //  Weight our peer to avoid needless redirections
        if (peer->load + amq_server_config_cluster_rebalance (amq_server_config) < lowest_load) {
            best_peer = amq_peer_link (peer);
            lowest_load = peer->load;
        }
        peer = amq_peer_list_next (&peer);
    }
    if (best_peer) {
        if (amq_server_config_debug_cluster (amq_server_config))
            asl_log_print (amq_broker->debug_log,
                "C: redirect client=%s tohost=%s",
                connection->client_address, best_peer->host);

        best_peer->load++;              //  At least until we get new figures
        amq_server_agent_connection_redirect (
            connection->thread, best_peer->host, self->known_hosts);
        amq_peer_unlink (&best_peer);
    }
    else
        amq_server_agent_connection_open_ok (connection->thread, self->known_hosts);
    </action>
</method>


<method name = "tunnel out" template = "async function" async = "1">
    <doc>
    Forward method to cluster. The mechanism works by wrapping the
    method as a content and then sending that via a Cluster.Proxy
    method to each target peer.  Every method is stamped with the
    originating channel so that replies can be redirected to that
    channel if needed.
    </doc>
    <argument name = "peer"    type = "amq_peer_t *">to peer</argument>
    <argument name = "method"  type = "amq_server_method_t *">method to send</argument>
    <argument name = "channel" type = "amq_server_channel_t *">channel for reply</argument>
    //
    <possess>
    peer = amq_peer_link (peer);
    method = amq_server_method_link (method);
    channel = amq_server_channel_link (channel);
    </possess>
    <release>
    amq_peer_unlink (&peer);
    amq_server_method_unlink (&method);
    amq_server_channel_unlink (&channel);
    </release>
    //
    <action>
    amq_content_tunnel_t
        *content;                       //  Message content
    ipr_bucket_t
        *bucket;
    amq_server_connection_t
        *connection;

    //  Wrap outgoing method as a content that we can tunnel to the
    //  other peer or peers
    content = amq_content_tunnel_new  ();
    bucket = amq_server_method_encode (method);
    amq_content_tunnel_record_body    (content, bucket);
    amq_content_tunnel_set_proxy_name (content, amq_broker->name);
    amq_content_tunnel_set_data_name  (content, method->name);
    amq_content_tunnel_set_durable    (content, durable);

    connection = channel?
        amq_server_connection_link (channel->connection): NULL;
    if (connection) {
        amq_content_tunnel_set_headers_field (content,
            "connection", connection->id);
        amq_content_tunnel_set_headers_field (content,
            "channel", "%d", channel->number);
        amq_server_connection_unlink (&connection);
    }
    ipr_bucket_unlink (&bucket);

    //  The peer argument can be a constant, or a real peer reference
    if (peer == AMQ_CLUSTER_ACTIVE) {
        if (self->active_peer)
            amq_peer_tunnel (self->active_peer, content);
        else
            asl_log_print (amq_broker->alert_log, "E: cluster - no tunnel to active");
    }
    else
    if (peer == AMQ_CLUSTER_ALL) {
        //  Send to all peers, and self if requested
        peer = amq_peer_list_first (self->peer_list);
        while (peer) {
            amq_peer_tunnel (peer, content);
            peer = amq_peer_list_next (&peer);
        }
    }
    else
        amq_peer_tunnel (peer, content);

    amq_content_tunnel_unlink (&content);
    </action>
</method>

<method name = "tunnel in" template = "async function" async = "1">
    <doc>
    Accepts tunneled method from cluster and process it accordingly.
    - execute the method
    - record the method if it's durable
    </doc>
    <argument name = "content" type = "amq_content_tunnel_t *">the message content</argument>
    <argument name = "channel" type = "amq_server_channel_t *">channel for reply</argument>
    //
    <possess>
    content = amq_content_tunnel_link (content);
    channel = amq_server_channel_link (channel);
    </possess>
    <release>
    amq_content_tunnel_unlink (&content);
    amq_server_channel_unlink (&channel);
    </release>
    //
    <action>
    ipr_bucket_t
        *bucket;                        //  Content body
    amq_server_method_t
        *method;                        //  Decoded method
    amq_peer_t
        *peer;                          //  Cluster peer for sender
    icl_shortstr_t
        strerror;                       //  Text for method errors
    amq_server_connection_t
        *connection;
    Bool
        delivered_to_peer;              //  Message for peer was delivered

    //  Decode proxied method from content body
    bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
    bucket->cur_size = amq_content_tunnel_get_body (
        content, bucket->data, bucket->max_size);
    method = amq_server_method_decode (bucket, strerror);
    if (!method)
        asl_log_print (amq_broker->alert_log, "E: %s", strerror);
    assert (method);

    connection = channel?
        amq_server_connection_link (channel->connection): NULL;

    if (connection == NULL)
        asl_log_print (amq_broker->alert_log,
            "W: cluster method %s from disconnecting peer %s, ignored",
            content->data_name, content->proxy_name);
    else
    if (amq_server_config_debug_cluster (amq_server_config)) {
        icl_console_print ("C: accept   method=%s from=%s",
            content->data_name, content->proxy_name);
        amq_server_method_dump (method, amq_broker->debug_log, "C: ");
    }
    //  Find out what peer sent this method to us
    peer = connection? amq_peer_list_first (self->peer_list): NULL;
    delivered_to_peer = FALSE;
    while (peer) {
        if (streq (peer->name, content->proxy_name)) {
            if (method->class_id == AMQ_SERVER_CLUSTER)
                s_execute_method (self, method, peer);
            else
            if (method->class_id  == AMQ_SERVER_BASIC
            &&  method->method_id == AMQ_SERVER_BASIC_GET)
                s_execute_basic_get (self, method, peer, channel, content);
            else
                amq_server_method_execute (method, connection, channel);

            delivered_to_peer = TRUE;
        }
        peer = amq_peer_list_next (&peer);
    }
    if (!delivered_to_peer)
        asl_log_print (amq_broker->alert_log,
            "E: check cluster config - is '%s' used for wrong peer?", content->proxy_name);

    amq_server_connection_unlink (&connection);
    amq_server_method_unlink (&method);
    ipr_bucket_unlink (&bucket);
    amq_peer_unlink (&peer);
    </action>
</method>

<method name = "peer push" template = "async function" async = "1">
    <doc>
    Publishes a message out the specified peer. Pushed messages are not
    recorded in the cluster state.
    </doc>
    <argument name = "peer" type = "amq_peer_t *">Proxy session</argument>
    <argument name = "method" type = "amq_server_method_t *">Publish method</argument>
    //
    <possess>
    peer = amq_peer_link (peer);
    method = amq_server_method_link (method);
    </possess>
    <release>
    amq_peer_unlink (&peer);
    amq_server_method_unlink (&method);
    </release>
    //
    <action>
    if (self->ready && peer)
        amq_peer_push (peer, method);
    </action>
</method>

<private name = "async header">
static void
    s_stop_cluster (amq_cluster_t *self);
static void
    s_append_to_state (amq_cluster_t *self, amq_content_tunnel_t *content);
static void
    s_execute_basic_get (amq_cluster_t *self, amq_server_method_t *method, amq_peer_t *peer,
        amq_server_channel_t *channel, amq_content_tunnel_t *content);
static void
    s_execute_method (amq_cluster_t *self, amq_server_method_t *method, amq_peer_t *peer);
</private>

<private name = "async footer">
static void
s_stop_cluster (amq_cluster_t *self)
{
    amq_peer_list_destroy (&self->peer_list);
    self->ready = FALSE;
}


//  We handle the queue GET method here since we need to return it
//  using the channel/connection information from the tunneled content

static void
s_execute_basic_get (
    amq_cluster_t        *self,
    amq_server_method_t  *method,
    amq_peer_t           *peer,
    amq_server_channel_t *channel,
    amq_content_tunnel_t *content)
{
    asl_field_list_t
        *headers;
    char
        *connection_id,
        *channel_nbr;
    amq_queue_t
        *queue;
    amq_server_connection_t
        *connection;
    //  We format a cluster-id that is stamped into the content before
    //  it is sent back to the requesting node, so that this node can
    //  route the content out to the correct client application.
    icl_shortstr_t
        cluster_id;         
        
    headers = asl_field_list_new (content->headers);
    connection_id = asl_field_list_string (headers, "connection");
    channel_nbr   = asl_field_list_string (headers, "channel");
    icl_shortstr_fmt (cluster_id, "%s/%s/%s", peer->name, connection_id, channel_nbr);
    asl_field_list_destroy (&headers);

    queue = amq_queue_table_search (self->vhost->queue_table, method->payload.basic_get.queue);
    if (queue) {
        amq_queue_get (queue, channel, method->class_id, cluster_id);
        amq_queue_unlink (&queue);
    }
    else {
        connection = channel?
            amq_server_connection_link (channel->connection): NULL;
        if (connection) {
            asl_log_print (amq_broker->alert_log,
                "E: cluster queue '%s' not defined", method->payload.basic_get.queue);
            amq_server_agent_basic_get_empty (
                connection->thread, channel->number, cluster_id);
            amq_server_connection_unlink (&connection);
        }
    }
}

//  We handle the cluster class here, since we already have the sending
//  peer and it's easier than going via the method execute code.
//
static void
s_execute_method (
    amq_cluster_t       *self,
    amq_server_method_t *method,
    amq_peer_t          *peer)
{
    if (method->method_id == AMQ_SERVER_CLUSTER_HELLO)
        amq_peer_recv_hello (peer, &method->payload.cluster_hello);
    else
    if (method->method_id == AMQ_SERVER_CLUSTER_STATUS)
        amq_peer_recv_status (peer, &method->payload.cluster_status);
}
</private>

<method name = "selftest" />

</class>
