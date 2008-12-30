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
    amq_queue_set_t
        *queue_set;                     //  Set of queues for binding
    amq_binding_mgt_t
        *binding_mgt;                   //  Interface to console
    ipr_looseref_list_t
        *index_list;                    //  List of indices for binding
    icl_shortstr_t
        routing_key;                    //  Binding routing key
    icl_longstr_t
        *arguments;                     //  Additional binding arguments
    icl_shortstr_t
        arguments_str;                  //  Arguments in string form
    Bool
        is_wildcard,                    //  Matches multiple routing keys?
        exclusive;                      //  Has at least one exclusive queue
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
    <argument name = "exclusive"   type = "Bool">Queue is exclusive?</argument>
    <local>
    amq_federation_t
        *federation;
    asl_field_list_t
        *field_list;
    </local>
    //
    self->exchange = exchange;
    self->queue_set = amq_queue_set_new ();
    self->index_list = ipr_looseref_list_new ();
    self->binding_mgt = amq_binding_mgt_new (exchange, self);
    self->exclusive = exclusive;
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
    // Store binding arguments in human readable form
    field_list = asl_field_list_new (self->arguments);
    assert (field_list);
    asl_field_list_dump (field_list, self->arguments_str);
    asl_field_list_destroy (&field_list);
    
    //  Notify federation, if any, about new binding
    federation = amq_federation_link (self->exchange->federation);
    if (federation) {
        amq_federation_binding_created (federation,
            self->routing_key, self->arguments, self->exclusive);
        amq_federation_unlink (&federation);
    }
</method>

<method name = "destroy">
    <local>
    amq_federation_t
        *federation;
    </local>
    //
    //  Notify federation, if any, about binding being destroyed
    federation = amq_federation_link (self->exchange->federation);
    if (federation) {
        amq_federation_binding_destroyed (federation,
            self->routing_key, self->arguments);
        amq_federation_unlink (&federation);
    }
    if (self->exchange->binding_index)
        ipr_index_delete (self->exchange->binding_index, self->index);

    amq_binding_mgt_destroy (&self->binding_mgt);
    amq_queue_set_destroy (&self->queue_set);
    ipr_looseref_list_destroy (&self->index_list);
    icl_longstr_destroy (&self->arguments);
</method>

<method name = "bind queue" template = "function">
    <doc>
    Attach queue to queue_set.  Called by parent exchange during queue bind.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to bind</argument>
    //
    if (!amq_queue_set_find (self->queue_set, queue))
        amq_queue_set_queue (self->queue_set, queue);
</method>

<method name = "unbind queue" template = "function">
    <doc>
    Remove queue from queue_set.  Returns -1 if the binding is empty (has no
    queues) after this operation.  Called by parent exchange during queue
    unbind.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">Queue to unbind</argument>
    <local>
    amq_queue_set_iter_t *
        iterator;
    </local>
    //
    if (!self->zombie) {
        iterator = amq_queue_set_find (self->queue_set, queue);
        if (iterator)
            amq_queue_set_iter_destroy (&iterator);

        //  Signal to caller if binding is now empty
        if (amq_queue_set_count (self->queue_set) == 0)
            rc = -1;
    }
</method>

<method name = "collect" template = "function">
    <doc>
    Collect all queues for the binding into the caller's publish set.
    To avoid the same message being published multiple times to the same
    queue via different bindings, the exchane collects all queues for a
    message, sorts, and eliminates duplicates.  Returns new queue set
    size.  If queue set size reaches 75% of limit, prints warning.
    </doc>
    <argument name = "queue set" type = "amq_queue_t **">Queue set</argument>
    <argument name = "set size"  type = "size_t">Queue set size</argument>
    <local>
    amq_queue_set_iter_t *
        iterator;
    </local>
    //
    iterator = amq_queue_set_first (self->queue_set);
    while (iterator) {
        if (set_size < AMQ_QUEUE_SET_MAX)
            queue_set [set_size++] = (amq_queue_t *) iterator->item;
        iterator = amq_queue_set_next (&iterator);
    }
    rc = set_size;
    if (set_size > AMQ_QUEUE_SET_MAX * 75 / 100)
        smt_log_print (amq_broker->alert_log,
            "W: reaching AMQ_QUEUE_SET_MAX (at %d), please notify openamq-dev@imatix.com",
            AMQ_QUEUE_SET_MAX);
</method>

<method name = "selftest" />

</class>
