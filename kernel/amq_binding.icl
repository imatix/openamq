<?xml?>
<class
    name      = "amq_binding"
    comment   = "An exchange binding"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements an exchange binding, which is a collection of
queues and other exchanges that share the same binding arguments.
This class runs lock-free as a child of the asynchronous exchange
class.
</doc>

<inherit class = "icl_object">
    <option name = "links" value = "1" />
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_tracker" />

<import class = "amq_server_classes" />

<context>
    amq_exchange_t
        *exchange;                      //  Parent exchange
    amq_queue_list_t
        *queue_list;                    //  List of queues for binding
    ipr_looseref_list_t
        *peer_list;                     //  List of cluster peers for binding
    ipr_looseref_list_t
        *index_list;                    //  List of indices for binding
    icl_shortstr_t
        routing_key;                    //  Binding routing key
    icl_longstr_t
        *arguments;                     //  Additional binding arguments
    int
        index;                          //  Index in exchange->binding_index

    //  Only used for dest-wild matching, might be moved elsewhere
    icl_shortstr_t
        regexp;                         //  Binding routing key pattern

    //  Only used for property matching, might be moved elsewhere
    int
        field_count;                    //  Number of fields indexed
    Bool
        match_all;                      //  Match all fields?
</context>

<method name = "new">
    <argument name = "exchange"    type = "amq_exchange_t *">Parent exchange</argument>
    <argument name = "routing key" type = "char *">Bind to routing key</argument>
    <argument name = "arguments"   type = "icl_longstr_t *" >Additional arguments</argument>
    self->exchange   = exchange;
    self->queue_list = amq_queue_list_new ();
    self->peer_list  = ipr_looseref_list_new ();
    self->index_list = ipr_looseref_list_new ();
    icl_shortstr_cpy (self->routing_key, routing_key);

    //  Store empty arguments as null to simplify comparisons
    if (arguments && arguments->cur_size)
        self->arguments = icl_longstr_dup (arguments);

    //  Get an index for the binding and complain if the index is full
    self->index = ipr_index_insert (self->exchange->binding_index, self);
    if (!self->index) {
        icl_console_print ("W: too many bindings in %s exchange", exchange->name);
        self_destroy (&self);
    }
</method>

<method name = "destroy">
    <local>
    amq_peer_t
        *peer;                          //  Peer object reference
    </local>
    //
    //  Drop all references to peers for the binding
    while ((peer = (amq_peer_t *) ipr_looseref_pop (self->peer_list)))
        amq_peer_unlink (&peer);

    ipr_index_delete (self->exchange->binding_index, self->index);
    amq_queue_list_destroy (&self->queue_list);
    ipr_looseref_list_destroy (&self->peer_list);
    ipr_looseref_list_destroy (&self->index_list);
    icl_longstr_destroy       (&self->arguments);
</method>

<method name = "bind queue" template = "function">
    <doc>
    Attach queue to current binding if it is not already there.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to bind</argument>

    if (!amq_queue_list_find (amq_queue_list_begin (self->queue_list), NULL,
          queue))
        amq_queue_list_push_back (self->queue_list, queue);
</method>

<method name = "bind peer" template = "function">
    <doc>
    Attach peer to current binding.
    </doc>
    <argument name = "peer" type = "amq_peer_t *">Peer to bind</argument>
    <local>
    ipr_looseref_t
        *looseref;                      //  We check the queues per binding
    </local>
    //
    looseref = ipr_looseref_list_first (self->peer_list);
    while (looseref && looseref->object != peer)
        looseref = ipr_looseref_list_next (&looseref);

    if (looseref)                       //  Ignore duplicates
        ipr_looseref_unlink (&looseref);
    else
        ipr_looseref_queue (self->peer_list, amq_peer_link (peer));
</method>

<method name = "unbind queue" template = "function">
    <doc>
    Remove queue from current binding it is there.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to unbind</argument>
    <local>
    amq_queue_list_iterator_t
        iterator;
    </local>
    //
    iterator = amq_queue_list_find (
        amq_queue_list_begin (self->queue_list), NULL, queue);
    if (!iterator)
        amq_queue_list_erase (self->queue_list, iterator);
</method>

<method name = "unbind peer" template = "function">
    <doc>
    Remove peer from current binding it is there.
    </doc>
    <argument name = "peer" type = "amq_peer_t *">Peer to unbind</argument>
    <local>
    ipr_looseref_t
        *looseref;                      //  We check the peers per binding
    </local>
    //
    looseref = ipr_looseref_list_first (self->peer_list);
    while (looseref) {
        if (looseref->object == peer) {
            peer = (amq_peer_t *) (looseref->object);
            amq_peer_unlink (&peer);
            ipr_looseref_destroy (&looseref);
            break;
        }
        looseref = ipr_looseref_list_next (&looseref);
    }
</method>

<method name = "publish" template = "function">
    <doc>
    Publish message to all queues and peers defined for the binding.
    Returns number of active consumers found on all queues published to.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "method"  type = "amq_server_method_t *">Publish method</argument>
    <local>
    amq_queue_list_iterator_t
        iterator;
    ipr_looseref_t
        *looseref;                      //  Bound object
    amq_peer_t
        *peer;                          //  Peer we publish to
    </local>
    //
    //  Publish to all queues, sending method to async queue class
    iterator  = amq_queue_list_begin (self->queue_list);
    while (iterator)
    {
        if (amq_server_config_trace_route (amq_server_config))
            icl_console_print ("X: publish  queue=%s", (*iterator)->key);
        amq_queue_publish (*iterator, channel, method);
        rc++;  //  Count recepients
        iterator  = amq_queue_list_next  (iterator);
    }

    //  Publish to peers, sending method to async cluster class
    looseref = ipr_looseref_list_first (self->peer_list);
    while (looseref) {
        peer = (amq_peer_t *) (looseref->object);
        if (amq_cluster->enabled
        &&  channel->connection->type != AMQ_CONNECTION_TYPE_CLUSTER
        &&  strneq (channel->connection->client_proxy_name, peer->name)) {
            if (amq_server_config_trace_route (amq_server_config))
                icl_console_print ("X: publish  peer=%s", peer->name);
            amq_cluster_peer_push (amq_cluster, peer, method);
            rc++;                       //  Count recepients
        }
        looseref = ipr_looseref_list_next (&looseref);
    }
</method>

<method name = "selftest" />

</class>
