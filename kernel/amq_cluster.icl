<?xml?>
<class
    name      = "amq_cluster"
    comment   = "AMQ cluster controller class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This implements the cluster controller, which is responsible for all
cluster transport and management issues. The cluster controller is an
asynchronous object. It manages a list of cluster peers.  

The cluster consists of one primary server, zero or one backup server,
and zero or more support servers. At any moment either the primary or
the backup server is the "head server".  The head server holds all
shared queues.

Failover from primary to backup is automatic, recovery to primary 
happens manually, through stopping the backup server after the primary
server has been restarted.

At startup the primary server becomes master, the backup does not.

If the backup server sees the primary server going away and if it has at
least one connected client, it becomes master and remains master until
it is stopped. If the primary server goes away, and the backup server
has no clients, it cedes its position as master.

If the primary server sees the backup server going away, and it is not
master, it becomes master. If the primary server sees the backup coming
back, as master, the primary stops being head.

If we're neither primary nor backup, we work with primary or backup as
they specify.  If both servers claim to be master at the same time, we
warn the primary server to cede its role as master.

//TODO: integrate with amq console
</doc>

<inherit class = "smt_object" />
<import class = "amq_server_classes" />

<public name = "header">
#define AMQ_CLUSTER_PRIMARY     (void *) 1
#define AMQ_CLUSTER_ALL         0
#define AMQ_CLUSTER_TRANSIENT   0       //  Method durability
#define AMQ_CLUSTER_DURABLE     1

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
    amq_vhost_t
        *vhost;                         //  Hold onto parent vhost
    amq_peer_list_t
        *peer_list;                     //  List of peers in cluster
    icl_shortstr_t
        known_hosts;                    //  List of known hosts
    Bool
        enabled,                        //  Cluster is enabled
        ready,                          //  Cluster is ready for use
        primary,                        //  We're the primary node
        backup,                         //  We're the backup node
        master;                         //  We're master
    amq_peer_t
        *primary_peer,                  //  Primary server, if not us
        *backup_peer,                   //  Backup server, if not us
        *master_peer;                   //  Master server, if not us
    ipr_looseref_list_t
        *state_list;                    //  Cluster state, list of contents
    int64_t
        state_size;                     //  Size of cluster state
    Bool
        state_alert;                    //  State oversize alert shown?
    ipr_crc_t
        *crc;                           //  Check sum of server list
</context>

<method name = "new">
    <argument name = "cluster name" type = "char *">Cluster name</argument>
    <local>
    ipr_config_t
        *config;                        //  Server config tree
    char
        *name = NULL,                   //  Peer server name
        *host = NULL;                   //  Peer server host
    amq_peer_t
        *peer;                          //  Cluster peer
    Bool
        is_primary,                     //  Peer is primary server
        is_backup,                      //  Peer is backup server
        name_valid;                     //  Own broker name is valid
    int
        primaries = 0,                  //  Number of primary servers
        backups = 0;                    //  Number of backup server
    </local>
    //
    if (cluster_name) {
        icl_shortstr_cpy (amq_broker->name, cluster_name);
        self->enabled = TRUE;
    }
    //  We prepare everything so that the cluster could be enabled
    //  later, via the console
    self->broker     = amq_broker_link (amq_broker);
    self->vhost      = amq_vhost_link  (amq_vhost);
    self->peer_list  = amq_peer_list_new ();
    self->state_list = ipr_looseref_list_new ();
    self->crc        = ipr_crc_new ();

    name_valid = FALSE;
    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/cluster", NULL);
    if (config->located) {
	    ipr_config_locate (config, "server", NULL);
	    while (config->located) {
            name = ipr_config_get (config, "name", "");
            host = ipr_config_get (config, "host", "");
            
            is_primary = atoi (ipr_config_get (config, "primary", "0")) > 0;
            if (is_primary)
                primaries++;
            is_backup = atoi (ipr_config_get (config, "backup", "0"))  > 0;
            if (is_backup)
                backups++;
            
            if (is_primary && is_backup)
                icl_console_print ("E: server cannot be both primary and backup");
            else
            if (*name & *host) {
                //  Keep running checksum of server list
                ipr_crc_calc_str (self->crc, name);
                ipr_crc_calc_str (self->crc, host);

                //  We do not create a peer for ourselves
                if (streq (name, amq_broker->name)) {
                    self->primary = is_primary;
                    self->backup  = is_backup;
                    name_valid = TRUE;
                }
                else {
                    icl_console_print ("I: cluster - server name=%s host=%s", name, host);
                    if (*self->known_hosts)
                        icl_shortstr_cat (self->known_hosts, " ");
                    icl_shortstr_cat (self->known_hosts, host);
                    
                    peer = amq_peer_new (self, name, host, is_primary, is_backup);
                    amq_peer_list_queue (self->peer_list, peer);
                    if (is_primary)
                        self->primary_peer = peer;
                    else
                    if (is_backup)
                        self->backup_peer = peer;
                    
                    amq_peer_unlink (&peer);
                }
            }
            else
                icl_console_print ("W: server definition needs 'name' and 'host', skipped");
                
            ipr_config_next (config);
        }
    }
    ipr_config_destroy (&config);

    //  Set initial master to primary node
    self->master      = self->primary;
    self->master_peer = self->primary_peer;
        
    //  We go through an asynchronous method so that it can send timer
    //  events to the cluster agent. A cluster of one is allowed.

    if (self->enabled) {
        if (primaries != 1) {
            icl_console_print ("E: cluster must have exactly one primary");
            smt_shut_down ();
        }
        else
        if (backups > 1) {
            icl_console_print ("E: cluster cannot have multiple backups");
            smt_shut_down ();
        }
        else
        if (name_valid)
            self_start (self);
        else {
            icl_console_print ("E: '%s' is not a valid server name - see config", amq_broker->name);
            smt_shut_down ();
        }
    }
</method>

<method name = "destroy">
    <action>
    amq_content_tunnel_t
        *content;                       //  Server state message
    //
    self->ready = FALSE;

    while ((content = (amq_content_tunnel_t *) ipr_looseref_pop (self->state_list)))
        amq_content_tunnel_destroy (&content);

    ipr_crc_destroy           (&self->crc);
    ipr_looseref_list_destroy (&self->state_list);
    amq_peer_list_destroy     (&self->peer_list);
    amq_broker_unlink         (&self->broker);
    amq_vhost_unlink          (&self->vhost);
    </action>
</method>

<method name = "start" template = "async function" async = "1">
    <doc>
    Start the cluster controller.  This is done asynchronously after
    we have configured the cluster.
    </doc>
    //
    <action>
    amq_proxy_agent_init ();
    icl_console_print ("I: cluster - own server name is '%s'", amq_broker->name);
    smt_timer_request_delay (self->thread, 100 * 1000, monitor_event);
    </action>
</method>

<!--
    This is the cluster monitor, which is invoked once per second to
    update the cluster status.  It rechecks each peer and counts the
    votes to decide whether this broker should become master or not.
 -->

<event name = "monitor">
    <action>
    amq_peer_t
        *peer;                          //  Cluster peer
    Bool
        cluster_alive = TRUE;           //  Cluster is working?
    int
        masters = 0;                    //  How many masters

    //  Update all peers' status
    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        amq_peer_monitor (peer);
        if (peer->offline) {
            //  Handle a peer that goes/is offline
            if (self->primary) {
                //  If we're primary and backup goes offline, we become master
                if (peer->backup)
                    self->master = TRUE;
            }
            else
            if (self->backup) {
                //  If we're backup and primary goes offline, we become master
                //      So long as we have at least one connected client
                if (peer->primary)
                    self->master = (amq_server_connection_count () > 0);
            }
            else {
                //  If we're support, and master goes offline, we suspend cluster
                if (peer == self->master_peer)
                    cluster_alive = FALSE;
            }
        }
        else
        if (peer->ready) {
            //  Handle a peer that comes back/is online
            if (self->primary) {
                //  If we're primary, and backup is master, we stop being master
                if (peer->backup && peer->master) {
                    self->master = FALSE;
                    self->master_peer = peer;
                }
            }
            else
            if (self->backup) {
                //  If we're backup, and primary is master, we remain master
                if (peer->primary && peer->master) {
                    ;   //  Do nothing, primary peer should stop being master
                }
            }
            else {
                //  If we're support, complain if we detect multiple masters
                if (peer->master)
                    masters++;
                if (masters > 0) {
                    icl_console_print ("E: cluster - multiple masters detected");
                    cluster_alive = FALSE;
                }
            }            
        }
        else
            cluster_alive = FALSE;      //  Waiting for cluster to start fully
            
        peer = amq_peer_list_next (&peer);
    }
    //  Set new calculated master state
    if (self->master) {
        if (!amq_broker->master)
            icl_console_print ("I: ******************* MASTER ON ********************");
        amq_broker->master = TRUE;
        self->master_peer = NULL;
    }
    else {
        if (amq_broker->master)
            icl_console_print ("I: ******************* MASTER OFF *******************");
        amq_broker->master = FALSE;
    }
    if (cluster_alive && !self->ready) {
        self->ready = TRUE;         //  Cluster is now ready for use
        icl_console_print ("I: **** CLUSTER VHOST '%s' READY FOR CONNECTIONS ****",
            amq_server_config_cluster_vhost (amq_server_config));
    }
    else
    if (!cluster_alive && self->ready) {
        self->ready = FALSE;
        icl_console_print ("I: **** CLUSTER VHOST '%s' STOPPING NEW CONNECTIONS ****",
            amq_server_config_cluster_vhost (amq_server_config));
    }
    //  Cluster monitor runs once per second
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<method name = "peer ready" template = "async function" async = "1">
    <doc>
    Register a successful outgoing connection to a cluster peer.
    </doc>
    <argument name = "peer" type = "amq_peer_t *">Proxy session</argument>
    <argument name = "name" type = "char *">Peer's server name</argument>
    //
    <possess>
    name = icl_mem_strdup (name);
    </possess>
    <release>
    icl_mem_free (name);
    </release>
    <action>
    amq_peer_ready (peer, name);
    </action>
</method>

<method name = "balance client" template = "async function" async = "1">
    <doc>
    Redirect client application to one of the cluster peers. Selects
    the cluster peer with the fewest clients.
    </doc>
    <argument name = "connection" type = "amq_server_connection_t *">client connection</argument>
    //
    <possess>
    amq_server_connection_link (connection);
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

    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        //  Weight our peer to avoid needless redirections
        if (peer->load + 1 < lowest_load)
            best_peer = amq_peer_link (peer);
        peer = amq_peer_list_next (&peer);
    }
    if (best_peer) {
        if (amq_server_config_trace_cluster (amq_server_config))
            icl_console_print ("C: redirect client=%s tohost=%s",
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

<method name = "bind exchange" template = "function">
    <doc>
    Replicate cluster bind method to all peers.  This sets up an
    exchange-to-exchange binding so that messages will get bounced
    down to the peer that holds the local queue. We send this method
    packaged as a forwarded method.
    </doc>
    <argument name = "exchange"    type = "char *">exchange name</argument>
    <argument name = "routing key" type = "char *">message routing key</argument>
    <argument name = "arguments"   type = "icl_longstr_t *">arguments for binding</argument>
    <local>
    amq_server_method_t
        *method;
    </local>
    //
    method = amq_server_method_new_cluster_bind (exchange, routing_key, arguments);
    amq_cluster_tunnel_out (self, AMQ_CLUSTER_ALL, method, AMQ_CLUSTER_DURABLE, NULL);
    amq_server_method_destroy (&method);
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
    <argument name = "durable" type = "Bool">part of cluster state?</argument>
    <argument name = "channel" type = "amq_server_channel_t *">channel for reply</argument>
    //
    <possess>
    amq_server_method_possess (method);
    if (channel)
        amq_server_channel_link (channel);
    </possess>
    <release>
    amq_server_method_destroy (&method);
    if (channel)
        amq_server_channel_unlink (&channel);
    </release>
    //
    <action>
    amq_content_tunnel_t
        *content;                       //  Message content

    content = s_tunnel_content (method, durable, channel);
    if (durable)                        //  Record in state if wanted
        s_append_to_state (self, content);

    //  The peer argument can be a constant, or a real peer reference
    if (peer == AMQ_CLUSTER_PRIMARY) {
        if (self->primary_peer)
            amq_peer_tunnel (self->primary_peer, content);
        else
            icl_console_print ("E: cluster - attempted to tunnel to self");       
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

    amq_content_tunnel_destroy (&content);
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
    amq_content_tunnel_possess (content);
    amq_server_channel_link (channel);
    </possess>
    <release>
    amq_content_tunnel_destroy (&content);
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

    //  Decode proxied method from content body
    bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
    bucket->cur_size = amq_content_tunnel_get_body (
        content, bucket->data, bucket->max_size);
    method = amq_server_method_decode (bucket, strerror);
    assert (method);

    if (amq_server_config_trace_cluster (amq_server_config)) {
        icl_console_print ("C: accept   method=%s from=%s",
            content->data_name, content->proxy_name);
        amq_server_method_dump (method, "C: ");
    }
    //  Find out what peer sent this method to us
    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        if (streq (peer->name, content->proxy_name)) {
            if (method->class_id == AMQ_SERVER_CLUSTER)
                s_execute_method (self, method, peer);
            else
                amq_server_method_execute (method, channel->connection, channel);
        }
        peer = amq_peer_list_next (&peer);
    }
    amq_server_method_destroy (&method);
    ipr_bucket_destroy (&bucket);
    amq_peer_unlink (&peer);

    //  If message is part of the cluster state, record it
    if (content->durable)
        s_append_to_state (self, content);
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
    amq_server_method_possess (method);
    </possess>
    <release>
    amq_server_method_destroy (&method);
    </release>
    //
    <action>
    if (self->ready && peer)
        amq_peer_push (peer, method);
    </action>
</method>

<private name = "async header">
static amq_content_tunnel_t *
    s_tunnel_content (amq_server_method_t *method, Bool durable, amq_server_channel_t *channel);
static void
    s_append_to_state (amq_cluster_t *self, amq_content_tunnel_t *content);
static void
    s_execute_method (amq_cluster_t *self, amq_server_method_t *method, amq_peer_t *peer);
</private>

<private name = "async footer">
static amq_content_tunnel_t *
s_tunnel_content (amq_server_method_t *method, Bool durable, amq_server_channel_t *channel)
{
    ipr_bucket_t
        *bucket;
    amq_content_tunnel_t
        *content;                       //  Message content

    content = amq_content_tunnel_new  ();
    bucket = amq_server_method_encode (method);
    amq_content_tunnel_record_body    (content, bucket);
    amq_content_tunnel_set_proxy_name (content, amq_broker->name);
    amq_content_tunnel_set_data_name  (content, method->name);
    amq_content_tunnel_set_durable    (content, durable);
    if (channel) {
        amq_content_tunnel_set_headers_field (content,
            "connection", channel->connection->id);
        amq_content_tunnel_set_headers_field (content,
            "channel", "%d", channel->number);
    }
    ipr_bucket_destroy (&bucket);
    return (content);
}

static void
s_append_to_state (amq_cluster_t *self, amq_content_tunnel_t *content)
{
    if (amq_server_config_trace_cluster (amq_server_config))
        icl_console_print ("C: record   method=%s", content->data_name);

    amq_content_tunnel_possess (content);
    ipr_looseref_queue (self->state_list, content);
    self->state_size += content->body_size + sizeof (content);

    if (self->state_size
    > amq_server_config_cluster_state_mb (amq_server_config) * 1024 * 1024
    && !self->state_alert) {
        icl_console_print ("E: WARNING: cluster state exceeds %dMb",
            amq_server_config_cluster_state_mb (amq_server_config));
        self->state_alert = TRUE;
    }
}

//  We handle the cluster class here, since we already have the sending
//  peer and it's easier than going via the method execute code.
//
static void
s_execute_method (amq_cluster_t *self, amq_server_method_t *method, amq_peer_t *peer)
{
    //  Cluster.Status method
    if (method->method_id == AMQ_SERVER_CLUSTER_STATUS) {
        amq_peer_recv_status (peer, &method->payload.cluster_status);
    }
    else
    //  Cluster.Bind method
    if (method->method_id == AMQ_SERVER_CLUSTER_BIND) {
        amq_exchange_t
            *exchange;              //  Exchange to bind to

        if (strnull (method->payload.cluster_bind.exchange))
            exchange = amq_exchange_link (amq_vhost->default_exchange);
        else
            exchange = amq_exchange_search (
                amq_vhost->exchange_table, method->payload.cluster_bind.exchange);

        if (exchange) {
            amq_exchange_bind_peer (
                exchange,
                peer,
                method->payload.cluster_bind.routing_key,
                method->payload.cluster_bind.arguments);
            amq_exchange_unlink (&exchange);
        }
        else
            icl_console_print ("E: no such exchange defined: '%s'",
                method->payload.cluster_bind.exchange);
    }
}
</private>

<method name = "selftest" />

</class>
