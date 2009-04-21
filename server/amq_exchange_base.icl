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
    <argument name = "content" type = "amq_content_basic_t *">Content to publish</argument>
    <argument name = "group" type = "int">User group, from connection</argument>
    <argument name = "immediate" type = "Bool">Send immediately or return?</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    char
        *routing_key,
        *message_id,
        *sender_id;
    int
        delivered = 0;                  //  Number of message deliveries
    amq_server_connection_t
        *connection;
    size_t
        set_size,                       //  Size of queue publish set
        set_index;                      //  Index into queue publish set
    amq_queue_t
        *last_queue = NULL;             //  Last queue we published to
    amq_lease_t
        *lease;                         //  Queue lease, if any
    </local>
    //
    <header>
    $(selfname:upper)_ASSERT_SANE (self);

    routing_key = content->routing_key;
    message_id  = content->message_id;
    sender_id   = content->sender_id;
    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: publish  %s: routing_key=%s", self->exchange->name, routing_key);

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
            lease = amq_lease_link (last_queue->lease);
            if (lease && lease->thread && last_queue->feed_on) {
                if (last_queue->feed_no_local
                && streq (content->producer_id, lease->connection_id)) {
                    if (amq_server_config_debug_route (amq_server_config))
                        smt_log_print (amq_broker->debug_log, "X: discard  queue=%s (direct, no-local, undeliverable)",
                            last_queue->key);
                }
                else
                    s_direct_deliver (last_queue, content, lease);
            }
            else {
                amq_queue_publish (last_queue, channel, content, immediate);
                if (amq_server_config_debug_route (amq_server_config))
                    smt_log_print (amq_broker->debug_log, "X: deliver  queue=%s",
                        last_queue->key);
            }
            amq_lease_unlink (&lease);
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
static void
    s_direct_deliver (amq_queue_t *queue, amq_content_basic_t *content, amq_lease_t *lease);
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

void
s_direct_deliver (amq_queue_t *queue, amq_content_basic_t *content, amq_lease_t *lease)
{
    qbyte
        queue_size = 0;

    queue->contents_in++;
    queue->traffic_in += content->body_size;
    queue_size = icl_atomic_inc32 ((volatile qbyte *) &(lease->pending));
    //  Track highest known direct queue size for monitoring
    if (amq_broker->direct_high < queue_size)
        amq_broker->direct_high = queue_size;

    //  This code mirrors code in amq_queue_basic.icl
    //  Check warning limit
    if (queue->warn_limit && queue_size >= queue->warn_limit && !queue->warned) {
        smt_log_print (amq_broker->alert_log,
            "W: queue=%s hit %d messages: no action (%s, %s, %s %s)",
            queue->name, queue_size,
            queue->connection->client_address,
            queue->connection->client_product,
            queue->connection->client_version,
            queue->connection->client_instance);
        queue->warned = TRUE;
    }
    //  Check just one of drop/trim (trim processed like drop, kill ignored)
    if ((queue->drop_limit && queue_size >= queue->drop_limit)
    ||  (queue->trim_limit && queue_size >= queue->trim_limit)) {
        if (!queue->dropped) {
            smt_log_print (amq_broker->alert_log,
                "W: queue=%s hit %d messages: dropping messages (%s, %s, %s, %s)",
                queue->name, queue_size,
                queue->connection->client_address,
                queue->connection->client_product,
                queue->connection->client_version,
                queue->connection->client_instance);
            queue->dropped = TRUE;
        }
        icl_atomic_dec32 ((volatile qbyte *) &(lease->pending));
        queue->drop_count++;
        content = NULL;
    }
    if (content) {
        //  Update queue statistics
        queue->contents_out++;
        queue->traffic_out += content->body_size;

        amq_server_agent_direct_out (lease->thread, content, 0);
        icl_atomic_inc32 ((volatile qbyte *) &(amq_broker->direct_fed));
        if (amq_server_config_debug_route (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "X: deliver  queue=%s (direct)",  queue->key);
    }
}
</private>

<method name = "selftest" />

</class>
