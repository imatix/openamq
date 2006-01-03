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
asynchronous object. It manages a list of peers, which are the primary
cluster servers, plus all secondary servers that have connected to this
server as cluster peers.
//TODO: integrate with amq console
</doc>

<inherit class = "smt_object" />
<import class = "amq_server_classes" />

<public name = "header">
//  Different ways of pushing a message to cluster peers
#define AMQ_CLUSTER_PUSH_NONE       0
#define AMQ_CLUSTER_PUSH_ALL        1
#define AMQ_CLUSTER_PUSH_SECONDARY  2

extern amq_cluster_t
    *amq_cluster;                       //  Single broker, self
</public>

<private>
amq_cluster_t
    *amq_cluster = NULL;
</private>

<context>
    amq_peer_list_t
        *peer_list;                     //  List of peers in cluster
    Bool
        enabled,                        //  Cluster peers are enabled
        ready,                          //  Cluster is ready for use
        primary,                        //  Are we in the primary partition?
        root;                           //  Are we the root server?
    amq_peer_t
        *root_peer;                     //  Root server in cluster
    int
        primary_nodes,                  //  Number of primary nodes
        votes;                          //  Last known votes
    ipr_looseref_list_t
        *state_list;                    //  Cluster state, list of contents
    int64_t
        state_size;                     //  Size of cluster state
    Bool
        state_alert;                    //  State oversize alert shown?
</context>

<method name = "new">
    <local>
    char
        *primary,                       //  List of primary hosts
        *from_ptr;                      //  Scan through primary string
    int
        namesize;
    icl_shortstr_t
        hostname;                       //  Current hostname
    amq_peer_t
        *peer;                          //  Cluster peer 
    </local>
    //
    self->peer_list  = amq_peer_list_new ();
    self->state_list = ipr_looseref_list_new ();
    primary = amq_server_config_cluster_primary (amq_server_config);

    while (*primary) {
        //    
        //TODO: more general way to split string into individual hostnames
        //
        while (*primary == ' ' || *primary == ',')
            *primary++;                 //  Skip spaces and commas
        from_ptr = primary;
        while (*primary && *primary != ' ' && *primary != ',')
            primary++;                  //  Find end of hostname
        namesize = primary - from_ptr;
        if (namesize > ICL_SHORTSTR_MAX)
            namesize = ICL_SHORTSTR_MAX;
        memcpy (hostname, from_ptr, namesize);
        hostname [namesize] = 0;
        from_ptr = primary + 1;
            
        if (*hostname) {
            //  Check hostname is not a duplicate
            peer = amq_peer_list_first (self->peer_list);
            while (peer) {
                if (ipr_str_lexeq (peer->hostname, hostname)) {
                    icl_console_print ("E: cluster - duplicate primary server %s", hostname);
                    break;
                }
                peer = amq_peer_list_next (&peer);
            }
            if (!peer) {
                peer = amq_peer_new (self, hostname, TRUE);
                amq_peer_list_queue (self->peer_list, peer);
                self->primary_nodes++;
            }
            amq_peer_unlink (&peer);
        }
    }
    //  We go through an asynchronous method so that it can send timer
    //  events to the cluster agent
    if (amq_server_config_cluster_enabled (amq_server_config)) {
        self->enabled = TRUE;
        amq_cluster_start (self);
    }
</method>

<method name = "destroy">
    <action>
    amq_content_cluster_t
        *content;                       //  Server state message
    //
    while ((content = (amq_content_cluster_t *) ipr_looseref_pop (self->state_list)))
        amq_content_cluster_destroy (&content);
    ipr_looseref_list_destroy (&self->state_list);
    
    self->enabled = FALSE;
    amq_peer_list_destroy (&self->peer_list);
    </action>
</method>

<method name = "start" template = "async function" async = "1">
    <doc>
    Start the cluster controller.  This is done asynchronously after
    creating the amq_cluster object.
    </doc>
    <action>
    amq_proxy_agent_init ();
    if (amq_peer_list_count (self->peer_list))
        smt_timer_request_delay (self->thread, 100 * 1000, monitor_event);
    else
        icl_console_print ("W: cluster - no primary servers specified");
    </action>
</method>

<!--
    This is the cluster monitor, which is invoked once per second to
    update the cluster status.  It rechecks each peer and counts the
    votes to decide whether this broker should become root or not.
 -->

<event name = "monitor">
    <action>
    amq_peer_t
        *peer;                          //  Cluster peer
    int
        votes = 0,                      //  Number of junior peers
        cur_voters = 0,                 //  Number of voters for this round
        all_voters = 0;                 //  Total number of primary peers
    int
        new_root = -1;                  //  0 = off, 1 = on, -1 = unchanged
    amq_server_method_t
        *method;
    int
        pending = 0,
        primary = 0,
        offline = 0,
        secondary = 0;

    //  Update all peers' status and get voting status
    //  Pending = no, active = maybe, offline = abstain, secondary = can't vote
    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        switch (amq_peer_status (peer)) {
            case AMQ_PEER_PENDING:
            pending++;
                if (amq_server_config_trace_cluster (amq_server_config)
                && !self->ready)
                    icl_console_print ("C: pending  hostname=%s", peer->hostname);
                //  Any pending peers means election is pending
                all_voters++;
                cur_voters++;
                break;
            case AMQ_PEER_PRIMARY:
            primary++;
                all_voters++;
                cur_voters++;
                votes += amq_peer_vote (peer);
                if (peer->loopback && !self->primary) {
                    self->primary = TRUE;
                    icl_console_print ("I: cluster - acting as primary server");
                }
                break;
            case AMQ_PEER_OFFLINE:
            offline++;
                //  Offline peers abstain but must remain a minority
                all_voters++;
                if (peer == self->root_peer)
                    s_cluster_broken (self, "disconnected");
                break;
            case AMQ_PEER_SECONDARY:
            secondary++;
                //  Secondary nodes have nothing to say
                break;  
        }
        peer = amq_peer_list_next (&peer);
    }
    //  We must get the same counts if the cluster is sane
    assert (all_voters == self->primary_nodes);

    //  Report new results if any
    if (votes != self->votes) {
        if (amq_server_config_trace_cluster (amq_server_config))
            icl_console_print ("C: election %d/%d/%d votes", votes, cur_voters, all_voters);
        self->votes = votes;
    }
    if (votes == cur_voters && cur_voters * 2 > all_voters) {
        //  Take over as cluster root if we have a unanimous vote and the 
        //  electorate constitutes more than 50% of the primary servers.
        //  Secondary servers can't vote because in a fragmented network
        //  they could create split-brain scenarios.
        //
        if (!self->root && self->primary) {
            icl_console_print ("I: cluster - start as root (%d/%d/%d votes)",
                votes, cur_voters, all_voters);
            new_root = 1;
        }
    }
    else
    if (votes == cur_voters && cur_voters * 2 < all_voters) {
        //  Resign as cluster root if we have a unanimous vote but the
        //  electorate constitutes less than 50% of the primary servers
        //
        if (self->root) {
            icl_console_print ("I: cluster - stop as root (%d/%d/%d votes)",
                votes, cur_voters, all_voters);
            new_root = 0;
        }
    }
    if (new_root != -1) {
        //  Set new root state and then tell all nodes about it
        self->root = new_root;
        method = amq_server_method_new_cluster_root (self->root);
        amq_cluster_forward (self, amq_vhost, method, TRUE, TRUE);
        amq_server_method_destroy (&method);
    }
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<method name = "joined" template = "async function" async = "1">
    <doc>
    Register a successful outgoing connection to a cluster peer. This
    happens when we connect to a peer that was specified in the original
    primary server list, or to a secondary peer that has connected to us
    at a later stage.
    </doc>
    <argument name = "peer" type = "amq_peer_t *">Proxy session</argument>
    <argument name = "spid" type = "char *">Peer's spid</argument>
    //
    <possess>
    spid = icl_mem_strdup (spid);
    peer = amq_peer_link (peer);
    </possess>
    <release>
    icl_mem_free (spid);
    if (peer->duplicate)
        amq_peer_destroy (&peer);
    else
        amq_peer_unlink (&peer);
    </release>
    //
    <action>
    amq_peer_t
        *duplicate;                     //  Cluster peer duplicate if any

    //  Check for another peer with the same spid - this can happen
    //  at startup, when primary peers connect to us at the same time
    //  as we connect to them...
    duplicate = amq_peer_list_first (self->peer_list);
    while (duplicate) {
        if (streq (duplicate->spid, spid) && peer != duplicate)
            break;
        duplicate = amq_peer_list_next (&duplicate);
    }
    //  If we have a duplicate then destroy the one that's not the
    //  primary node, i.e. the one looking like a secondary node.
    if (duplicate) {
        if (peer->primary)
            amq_peer_destroy (&duplicate);
        else {
            amq_peer_unlink (&duplicate);
            peer->duplicate = TRUE;
        }
    }
    if (!peer->duplicate)
        amq_peer_joined (peer, spid);
    </action>
</method>

<method name = "register" template = "async function" async = "1">
    <doc>
    Register a new server that has decided to talk to us, via a client
    connection.  If the server is unknown, we'll add it to our list of
    peers and then connect back to it.  We may get duplicates, these are
    removed when the peer connects.
    </doc>
    <argument name = "spid"     type = "char *">Peer's spid</argument>
    <argument name = "hostname" type = "char *">Peer's hostname</argument>
    //
    <possess>
    spid = icl_mem_strdup (spid);
    hostname = icl_mem_strdup (hostname);
    </possess>
    <release>
    icl_mem_free (spid);
    icl_mem_free (hostname);
    </release>
    //
    <action>
    amq_peer_t
        *peer;                          //  Cluster peer 

    //  Check that peer is not already known
    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        if (streq (peer->spid, spid))
            break;
        peer = amq_peer_list_next (&peer);
    }
    if (!peer) {
        //  Create new peer as secondary connection
        icl_console_print ("I: cluster - %s is talking to us", hostname);
        peer = amq_peer_new (self, hostname, FALSE);
        amq_peer_list_queue (self->peer_list, peer);
    }
    amq_peer_unlink (&peer);
    </action>
</method>

<method name = "cancel" template = "async function" async = "1">
    <doc>
    Cancel an inbound cluster connection.  If the connection came from
    a secondary server, we remove this from our peer list.
    </doc>
    <argument name = "spid" type = "char *">Peer's spid</argument>
    //  
    <possess>
    spid = icl_mem_strdup (spid);
    </possess>
    <release>
    icl_mem_free (spid);
    </release>
    //
    <action>
    amq_peer_t
        *peer;                          //  Cluster peer 

    //  Check if peer is a known secondary server
    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        if (streq (peer->spid, spid) && !peer->primary)
            break;
        peer = amq_peer_list_next (&peer);
    }
    if (peer) {
        icl_console_print ("I: cluster - %s stopped talking us", peer->hostname);
        amq_peer_destroy (&peer);
    }
    </action>
</method>

<method name = "forward" template = "async function" async = "1">
    <doc>
    Forward method to cluster.  The mechanism works by wrapping the
    method as a content and then sending that via a Cluster.Publish
    method to each target peer.

    - If we're a primary server we send the method to all peers.
    - If we're a secondary server we send the method to all primary
      peers, and we also ask the root server to forward the method
      by setting the 'fanout' option.
    </doc>
    <argument name = "vhost" type = "amq_vhost_t *">virtual host</argument>
    <argument name = "method" type = "amq_server_method_t *">method to send</argument>
    <argument name = "stateful" type = "Bool">part of cluster state?</argument>
    <argument name = "toself" type = "Bool">send to self as well?</argument>
    //
    <possess>
    amq_server_method_possess (method);
    </possess>
    <release>
    amq_server_method_destroy (&method);
    </release>
    //
    <action>
    ipr_bucket_t
        *bucket;
    amq_content_cluster_t
        *content;                       //  Message content

    bucket  = amq_server_method_encode (method);
    content = amq_content_cluster_new ();
    amq_content_cluster_record_body    (content, bucket);
    amq_content_cluster_set_message_id (content, method->name);
    amq_content_cluster_set_stateful   (content, (byte) stateful);
    s_broadcast_message (self, content, vhost->name, toself);

    amq_content_cluster_destroy (&content);
    ipr_bucket_destroy (&bucket);
    </action>
</method>

<method name = "forward bind" template = "function">
    <doc>
    Forward cluster bind method to all peers.  This sets up an
    exchange-to-exchange binding so that messages will get bounced
    down to the peer that holds the local queue. We send this method
    packaged as a forwarded method.
    </doc>
    <argument name = "vhost"       type = "amq_vhost_t *">virtual host</argument>
    <argument name = "exchange"    type = "char *">exchange name</argument>
    <argument name = "routing key" type = "char *">message routing key</argument>
    <argument name = "arguments"   type = "icl_longstr_t *">arguments for binding</argument>
    <local>
    amq_server_method_t
        *method;
    </local>
    //
    method = amq_server_method_new_cluster_bind (
        vhost->name, exchange, routing_key, arguments);
    amq_cluster_forward (self, amq_vhost, method, TRUE, FALSE);
    amq_server_method_destroy (&method);
</method>

<method name = "ident" template = "function">
    <doc>
    Returns the current node ident, which is R, B, or S depending
    on whether we are the root, a backup primary, or a secondary
    server.
    </doc>
    if (self->root)
        rc = 'R';
    else
    if (self->primary)
        rc = 'B';
    else
        rc = 'S';
</method>

<method name = "accept" template = "async function" async = "1">
    <doc>
    Accepts packed method from cluster and process it accordingly.
    - execute the method
    - record the method if we're a primary peer
    - if fanout set, and we're root
        - broadcast the method again
    </doc>
    <argument name = "content" type = "amq_content_cluster_t *">The message content</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    //
    <possess>
    amq_content_cluster_possess (content);
    </possess>
    <release>
    amq_content_cluster_destroy (&content);
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

    //  Get content body into a bucket
    bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
    bucket->cur_size = amq_content_cluster_get_body (
        content, bucket->data, bucket->max_size);

    method = amq_server_method_decode (bucket, strerror);
    assert (method);

    //  Find out what peer sent this method to us
    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        if (streq (peer->spid, content->sender_spid))
            break;
        peer = amq_peer_list_next (&peer);
    }
    assert (peer);

    if (amq_server_config_trace_cluster (amq_server_config)) {
        icl_console_print ("C: accept   message=%s from=%s",
            content->message_id, content->sender_spid);
        amq_server_method_dump (method, "C: ");
    }
    //  We handle the cluster class here, since we already have
    //  the originating peer and it's easier than going via the
    //  method execute code.
    //
    if (method->class_id == AMQ_SERVER_CLUSTER) {
        //
        //  Execute Cluster.Root method
        if (method->method_id == AMQ_SERVER_CLUSTER_ROOT) {
            amq_server_cluster_root_t
                *cluster_root = &method->payload.cluster_root;

            if (cluster_root->active)
                s_cluster_ready (self, peer);
            else
                s_cluster_broken (self, "resigned");
        }
        else
        //
        //  Execute Cluster.Bind method
        if (method->method_id == AMQ_SERVER_CLUSTER_BIND) {
            amq_server_cluster_bind_t
                *cluster_bind = &method->payload.cluster_bind;
            amq_exchange_t
                *exchange;              //  Exchange to bind to

            if (strnull (cluster_bind->exchange))
                exchange = amq_exchange_link (amq_vhost->default_exchange);
            else
                exchange = amq_exchange_search (
                    amq_vhost->exchange_table, cluster_bind->exchange);

            if (exchange) {
                amq_exchange_bind_peer (
                    exchange,
                    peer,
                    cluster_bind->routing_key,
                    cluster_bind->arguments);
                amq_exchange_unlink (&exchange);
            }
            else
                icl_console_print ("E: no such exchange defined: '%s'",
                    cluster_bind->exchange);
        }
    }
    else
        amq_server_method_execute (method, channel->connection, channel);

    //  If message is part of the cluster state, record it
    if (self->primary
    &&  content->stateful
    &&  strneq (content->sender_spid, amq_broker->spid))
        s_append_to_state (self, content);

    //  If required, re-broadcast message
    if (self->root && content->fanout)
        s_broadcast_message (self, content, NULL, FALSE);

    amq_peer_unlink (&peer);
    amq_server_method_destroy (&method);
    ipr_bucket_destroy (&bucket);
    </action>
</method>

<method name = "peer push" template = "async function" async = "1">
    <doc>
    Publishes a message out to on a specific peer, depending on the
    push_out mode.  Pushed messages are not recorded in state.
    </doc>
    <argument name = "peer"   type = "amq_peer_t *">Proxy session</argument>
    <argument name = "vhost"  type = "amq_vhost_t *">virtual host</argument>
    <argument name = "method" type = "amq_server_method_t *">Publish method</argument>
    <argument name = "mode"   type = "int">Push-out mode</argument>
    //
    <possess>
    peer = amq_peer_link (peer);
    amq_server_method_possess (method);
    </possess>
    <release>
    amq_peer_unlink (&peer);
    amq_server_method_destroy (&method);
    </release>
    //
    <action>
    if (mode == AMQ_CLUSTER_PUSH_ALL || !self->primary)
        amq_peer_push (peer, method);
    </action>
</method>

<private name = "async header">
//  Cluster ready or not
static void
    s_cluster_ready (amq_cluster_t *self, amq_peer_t *peer);
static void
    s_cluster_broken (amq_cluster_t *self, char *reason);

//  Sending stuff out to the cluster
static void
    s_broadcast_message (amq_cluster_t *self, amq_content_cluster_t *content,
                         char *vhost_name, Bool toself);
static void
    s_append_to_state (amq_cluster_t *self, amq_content_cluster_t *content);
</private>

<private name = "async footer">
static void
s_cluster_ready (amq_cluster_t *self, amq_peer_t *peer)
{
    if (!self->ready) {
        icl_console_print ("I: **** CLUSTER '%s' READY FOR CONNECTIONS ****",
            amq_server_config_cluster_key (amq_server_config));
        if (peer->loopback)
            self->root = TRUE;      //  We're root!
        else
            self->root = FALSE;
        self->root_peer = peer;
        self->ready = TRUE;         //  Cluster is now ready for use
    }
}

static void
s_cluster_broken (amq_cluster_t *self, char *reason)
{
    if (self->ready && self->root_peer) {
        icl_console_print ("I: cluster - %s %s", self->root_peer->hostname, reason);
        icl_console_print ("I: **** CLUSTER '%s' STOPPING NEW CONNECTIONS ****",
            amq_server_config_cluster_key (amq_server_config));
        self->root_peer = NULL;
        self->ready = FALSE;
    }
}


/*  -------------------------------------------------------------------------
    Broadcast message to cluster:
      - if we're a secondary server, set the fanout option
      - send to all primary peers
      - if we're a primary server & method is stateful:
        - append to state
 */

static void
s_broadcast_message (
    amq_cluster_t *self,
    amq_content_cluster_t *content,
    char *vhost_name,
    Bool toself)
{
    amq_peer_t
        *peer;                          //  Cluster peer
    icl_shortstr_t
        sender_spid;                    //  Original sender, if any

    //  Save original sender in case we're re-broadcasting
    icl_shortstr_cpy (sender_spid, content->sender_spid);

    //  Set sender id, and other fields if desired
    //  When reforwarding a message we leave the vhost as-is
    amq_content_cluster_set_sender_spid (content, amq_broker->spid);
    if (vhost_name)
        amq_content_cluster_set_vhost_name (content, vhost_name);

    if (self->primary) {
        //  If message is part of the cluster state, record it
        //  We do this only if we're a primary server since the
        //  replay only happens from the root to new servers...
        if (content->stateful)
            s_append_to_state (self, content);

        amq_content_cluster_set_fanout (content, FALSE);
    }
    else
        //  Ask root (eventually) to fanout the method
        amq_content_cluster_set_fanout (content, TRUE);

    peer = amq_peer_list_first (self->peer_list);
    while (peer) {
        //  Don't send to original sender, if any
        //  Don't send to self unless explicitly requested
        if (strneq (peer->spid, sender_spid)
        && (toself || peer->loopback == FALSE))
            amq_peer_forward (peer, content);
        peer = amq_peer_list_next (&peer);
    }
}

static void
s_append_to_state (amq_cluster_t *self, amq_content_cluster_t *content)
{
    if (amq_server_config_trace_cluster (amq_server_config))
        icl_console_print ("C: record   message=%s", content->message_id);

    amq_content_cluster_possess (content);
    ipr_looseref_queue (self->state_list, content);
    self->state_size += content->body_size + sizeof (content);

    if (self->state_size
    > amq_server_config_cluster_state_mb (amq_server_config)
    * 1024 * 1024 && !self->state_alert) {
        icl_console_print ("E: WARNING: cluster state exceeds %dMb", 
            amq_server_config_cluster_state_mb (amq_server_config));
        self->state_alert = TRUE;
    }
}
</private>

<method name = "selftest" />

</class>
