<?xml?>
<class
    name      = "amq_binding"
    comment   = "An exchange binding"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements an exchange binding, which is a collection of
queues and other exchanges that share the same binding arguments.
This class runs lock-free as a child of the asynchronous exchange
class.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "list" />
    <option name = "rwlock" value = "0" />
</inherit>

<import class = "amq_server_classes" />

<context>
    amq_exchange_t
        *exchange;                      //  Parent exchange
    ipr_looseref_list_t
        *queue_list;                    //  List of queues for binding
    ipr_looseref_list_t
        *exchange_list;                 //  List of exchanges for binding
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
    self->exchange      = exchange;
    self->queue_list    = ipr_looseref_list_new ();
    self->exchange_list = ipr_looseref_list_new ();
    self->index_list    = ipr_looseref_list_new ();
    self->arguments     = icl_longstr_dup (arguments);
    icl_shortstr_cpy (self->routing_key, routing_key);

    //  Get an index for the binding and complain if the index is full
    self->index = ipr_index_insert (self->exchange->binding_index, self);
    if (!self->index) {
        icl_console_print ("W: too many bindings in %s exchange", exchange->name);
        self_destroy (&self);
    }
</method>

<method name = "destroy">
    <local>
    amq_queue_t
        *queue;                         //  Queue object reference
    amq_exchange_t
        *exchange;                      //  Exchange object reference
    </local>
    //
    //  Drop all references to queues and exchanges for the binding
    while ((queue = (amq_queue_t *) ipr_looseref_pop (self->queue_list)))
        amq_queue_unlink (&queue);
    while ((exchange = (amq_exchange_t *) ipr_looseref_pop (self->exchange_list)))
        amq_exchange_unlink (&exchange);

    ipr_index_delete (self->exchange->binding_index, self->index);
    ipr_looseref_list_destroy (&self->queue_list);
    ipr_looseref_list_destroy (&self->exchange_list);
    ipr_looseref_list_destroy (&self->index_list);
    icl_longstr_destroy       (&self->arguments);
</method>

<method name = "bind queue" template = "function">
    <doc>
    Attach queue to current binding.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to bind</argument>
    //
    amq_queue_link (queue);
    ipr_looseref_queue (self->queue_list, queue);
</method>

<method name = "bind exchange" template = "function">
    <doc>
    Attach exchange to current binding.
    </doc>
    <argument name = "exchange" type = "amq_exchange_t *">Exchange to bind</argument>
    //
    amq_exchange_link (exchange);
    ipr_looseref_queue (self->exchange_list, exchange);
</method>

<method name = "publish" template = "function">
    <doc>
    Publish message to all queues and exchanges defined for the
    binding.  Returns number of active consumers found on all queues
    published to.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "class id"  type = "int">The content class</argument>
    <argument name = "content"   type = "void *">The message content</argument>
    <argument name = "mandatory" type = "Bool">Warn if unroutable</argument>
    <argument name = "immediate" type = "Bool">Warn if no consumers</argument>
    <local>
    ipr_looseref_t
        *looseref,                      //  Bound object
        *oldref;                        //  Backcopy for garbage collection
    amq_queue_t
        *queue;                         //  Queue we publish to
    amq_exchange_t
        *exchange;                      //  Exchange we publish to
    </local>
    //
    //  Publish to all queues, sending method to async queue class
    looseref = ipr_looseref_list_first (self->queue_list);
    while (looseref) {
        queue = (amq_queue_t *) (looseref->object);
        if (!queue->zombie) {
            if (amq_server_config_trace_route (amq_server_config))
                icl_console_print ("X: publish  queue=%s", queue->key);
            amq_queue_publish (queue, channel, class_id, content, immediate);
            looseref = ipr_looseref_list_next (&looseref);
            rc++;
        }
        else {
            //  Garbage-collect zombied queue
            oldref   = ipr_looseref_link      (looseref);
            looseref = ipr_looseref_list_next (&looseref);
            ipr_looseref_destroy (&oldref);
            amq_queue_unlink (&queue);
        }
    }
    //  Publish to all exchanges, sending method to async exchange class
    looseref = ipr_looseref_list_first (self->exchange_list);
    while (looseref) {
        exchange = (amq_exchange_t *) (looseref->object);
        if (!exchange->zombie) {
            if (amq_server_config_trace_route (amq_server_config))
                icl_console_print ("X: publish  exchange=%s", exchange->key);
            amq_exchange_publish (exchange, channel, class_id, content, mandatory, immediate);
            looseref = ipr_looseref_list_next (&looseref);
        }
        else {
            //  Garbage-collect zombied exchange
            oldref   = ipr_looseref_link      (looseref);
            looseref = ipr_looseref_list_next (&looseref);
            ipr_looseref_destroy (&oldref);
            amq_exchange_unlink (&exchange);
        }
    }
</method>

<method name = "selftest" />

</class>
