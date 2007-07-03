<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
-->

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
    //  Notify MTA about new binding
    if (self->exchange->mta)
        amq_cluster_mta_binding_created (self->exchange->mta, 
            self->routing_key, self->arguments);
</method>

<method name = "destroy">
    //  Notify MTA about binding being destroyed
    if (self->exchange->mta)
        amq_cluster_mta_binding_destroyed (self->exchange->mta,
            self->routing_key, self->arguments);

    if (self->exchange->binding_index)
        ipr_index_delete (self->exchange->binding_index, self->index);

    amq_queue_list_destroy    (&self->queue_list);
    ipr_looseref_list_destroy (&self->index_list);
    icl_longstr_destroy       (&self->arguments);
</method>

<method name = "bind queue" template = "function">
    <doc>
    Attach queue to queue_list.  Called by parent exchange during queue bind.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to bind</argument>
    //
    if (!amq_queue_list_find (self->queue_list, queue))
        amq_queue_list_queue (self->queue_list, queue);
</method>

<method name = "unbind queue" template = "function">
    <doc>
    Remove queue from queue_list.  Returns -1 if the binding is empty (has no
    queues) after this operation.  Called by parent exchange during queue
    unbind.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to unbind</argument>
    <local>
    amq_queue_list_iter_t *
        iterator;
    </local>
    //
    if (!self->zombie) {
        iterator = amq_queue_list_find (self->queue_list, queue);
        if (iterator)
            amq_queue_list_iter_destroy (&iterator);

        //  Signal to caller if binding is now empty
        if (amq_queue_list_count (self->queue_list) == 0)
            rc = -1;
    }
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
    amq_queue_list_iter_t *
        iterator;
    </local>
    //
    //  Publish to all queues, sending method to async queue class
    iterator = amq_queue_list_first (self->queue_list);
    while (iterator) {
        if (amq_server_config_debug_route (amq_server_config))
            smt_log_print (amq_broker->debug_log, "X: deliver  queue=%s", iterator->item->key);
        amq_queue_publish (iterator->item, channel, method);
        iterator = amq_queue_list_next (&iterator);
        rc++;                           //  Count recepients
    }
</method>

<method name = "selftest" />

</class>

