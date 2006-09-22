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
<inherit class = "icl_list_item">
    <option name = "count" value = "1" />
</inherit>
<inherit class = "icl_tracker" />
<!-- Workaround to force links -->
<option name = "links" value = "1" />
<option name = "rwlock" value = "0" />

<import class = "amq_server_classes" />

<context>
    amq_exchange_t
        *exchange;                      //  Parent exchange
    amq_queue_list_t
        *queue_list;                    //  List of queues for binding
    ipr_looseref_list_t
        *index_list;                    //  List of indices for binding
    icl_shortstr_t
        routing_key;                    //  Binding routing key
    icl_longstr_t
        *arguments;                     //  Additional binding arguments
    Bool
        is_wildcard;                    //  Matches multiple routing keys?
    int
        index;                          //  Index in exchange->binding_index

    //  Only used for topic exchange, might be moved elsewhere
    icl_shortstr_t
        regexp;                         //  Binding routing key pattern

    //  Only used for header exchange, might be moved elsewhere
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
    self->index_list = ipr_looseref_list_new ();
    icl_shortstr_cpy (self->routing_key, routing_key);

    //  Store empty arguments as null to simplify comparisons
    if (arguments && arguments->cur_size)
        self->arguments = icl_longstr_dup (arguments);

    //  Get an index for the binding and complain if the index is full
    self->index = ipr_index_insert (self->exchange->binding_index, self);
    if (!self->index) {
        smt_log_print (amq_broker->alert_log,
            "E: too many bindings in %s exchange", exchange->name);
        self_destroy (&self);
    }
</method>

<method name = "destroy">
    ipr_index_delete (self->exchange->binding_index, self->index);
    amq_queue_list_destroy    (&self->queue_list);
    ipr_looseref_list_destroy (&self->index_list);
    icl_longstr_destroy       (&self->arguments);
</method>

<method name = "bind queue" template = "function">
    <doc>
    Attach queue to current binding if it is not already there.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to bind</argument>
    //
    if (!amq_queue_list_find (
        amq_queue_list_begin (self->queue_list), NULL, queue))
        amq_queue_list_push_back (self->queue_list, queue);
</method>

<method name = "unbind queue" template = "function">
    <doc>
    Remove queue from current binding it is there.  Returns -1 if the
    binding is empty after this operation.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to unbind</argument>
    <local>
    amq_queue_list_iterator_t
        iterator;
    </local>
    //
    iterator = amq_queue_list_find (
        amq_queue_list_begin (self->queue_list), NULL, queue);
    if (iterator)
        amq_queue_list_erase (self->queue_list, iterator);

    //  Signal to caller if binding is now empty
    if (amq_queue_list_size (self->queue_list) == 0)
        rc = -1;
</method>

<method name = "publish" template = "function">
    <doc>
    Publish message to all queues and peers defined for the binding.
    Returns number of times the message was published (fanout), which
    is 1 or greater if there were recipients, zero if not.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "method"  type = "amq_server_method_t *">Publish method</argument>
    <local>
    amq_queue_list_iterator_t
        iterator;
    </local>
    //
    //  Publish to all queues, sending method to async queue class
    iterator = amq_queue_list_begin (self->queue_list);
    while (iterator) {
        if (amq_server_config_debug_route (amq_server_config))
            smt_log_print (amq_broker->debug_log, "X: deliver  queue=%s", (*iterator)->key);
        amq_queue_publish (*iterator, channel, method);
        iterator = amq_queue_list_next (iterator);
        rc++;                           //  Count recepients
    }
</method>

<method name = "selftest" />

</class>

