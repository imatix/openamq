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
    name      = "amq_exchange_base"
    comment   = "Exchange abstract base class"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
This is an abstract base class for all exchange implementations.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_server_classes" />

<context>
    amq_exchange_t
        *exchange;                      //  Parent exchange
</context>

<method name = "new">
    <argument name = "exchange" type = "amq_exchange_t *">Parent exchange</argument>
    //
    self->exchange = exchange;
</method>

<method name = "compile" return = "rc">
    <doc>
    Compiles the binding for the exchange, returns 1 if the binding was
    invalid.  Note that if an exchange uses a routing key at all, it must be
    mandatory in all binds.
    </doc>
    <argument name = "self_v"  type = "void *">The exchange cast as a void *</argument>
    <argument name = "binding" type = "amq_binding_t *">Binding to compile</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for errors</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
</method>

<method name = "publish" return = "rc">
    <doc>
    Routes one message through the exchange.
    </doc>
    <argument name = "self_v" type = "void *">The exchange cast as a void *</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "method" type = "amq_server_method_t *">Publish method</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    amq_server_basic_publish_t
        *basic_method = NULL;
    amq_content_basic_t
        *basic_content = NULL;
    char
        *routing_key = "",
        *message_id = NULL,
        *sender_id = NULL;
    int
        delivered = 0;                  //  Number of message deliveries
    amq_server_connection_t
        *connection;
    size_t
        set_size,                       //  Size of queue publish set
        set_index;                      //  Index into queue publish set
    amq_queue_t
        *last_queue = NULL;             //  Last queue we published to
    </local>
    //
    <header>
    $(selfname:upper)_ASSERT_SANE (self);

    if (method->class_id == AMQ_SERVER_BASIC) {
        basic_method  = &method->payload.basic_publish;
        basic_content = (amq_content_basic_t *) (method->content);
        routing_key   = basic_method->routing_key;
        message_id    = basic_content->message_id;
        sender_id     = basic_content->sender_id;
        if (amq_server_config_debug_route (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "X: publish  %s: routing_key=%s", self->exchange->name, routing_key);
    }
    else
        smt_log_print (amq_broker->alert_log,
            "E: $(selfname) - bad class_id - %d", method->class_id);
   
    //  Grab reference to connection 
    connection = channel?
        amq_server_connection_link (channel->connection): NULL;
        
    //  Collect all queues to publish to into queue_set, initially empty
    set_size = 0;
    </header>
    <footer>
    //  Sort the queue set and then publish to all unique queues
    if (set_size > 1)
        qsort (self->exchange->queue_set, set_size, sizeof (void *), s_compare_queue);
    for (set_index = 0; set_index < set_size; set_index++) {
        if (self->exchange->queue_set [set_index] != last_queue) {
            last_queue = self->exchange->queue_set [set_index];

            if (amq_server_config_debug_route (amq_server_config))
                smt_log_print (amq_broker->debug_log, "X: deliver  queue=%s", 
                    last_queue->key);
            amq_queue_publish (last_queue, channel, method);
            delivered++;
        }
    }
    amq_server_connection_unlink (&connection);
    rc = delivered;                     //  Return number of deliveries
    </footer>
</method>

<private name = "header">
static int
    s_compare_queue (const void *queue1, const void *queue2);
</private>

<private>
static int
s_compare_queue (const void *queue1, const void *queue2)
{
    if (*(void **) queue1 < *(void **) queue2)
        return (-1);
    else
    if (*(void **) queue1 > *(void **) queue2)
        return (1);
    else
        return (0);
}
</private>

<method name = "unbind" return = "rc">
    <doc>
    Unbinds the binding for the exchange.
    </doc>
    <argument name = "self_v"  type = "void *">The exchange cast as a void *</argument>
    <argument name = "binding" type = "amq_binding_t *">Binding to unbind</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
</method>

<method name = "selftest" />

</class>
