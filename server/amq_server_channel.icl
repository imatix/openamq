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
    name      = "amq_server_channel"
    comment   = "AMQ server channel class"
    version   = "1.0"
    script    = "smt_object_gen"
    >
<doc>
This class implements the AMQ server channel class.  The channel holds
a set of consumers managed both as a list and as a lookup table.  The
maximum number of consumers per channel is set at compile time.

Channel flow control: each channel gets a credit window when it gets a
consumer.  This window defines the maximum size of its flow queue, the
number of contents it has received from a queue but not yet handed to
SMT.  When the channel receives a content from a queue, it spends a
credit.  When it passes a content to SMT it earns a credit.  Queues
check the credit to decide whether or not to use the channel's consumers.
</doc>

<inherit class = "asl_server_channel" />
<option name = "basename" value = "amq_server" />

<context>
    amq_connection_t
        *mgt_connection;                //  Connection management object
    amq_consumer_by_channel_t
        *consumer_list;                 //  List of consumers for channel
    icl_shortstr_t
        current_exchange,               //  Last exchange declared on channel
        current_queue;                  //  Last queue declared on channel
</context>

<method name = "new">
    self->consumer_list = amq_consumer_by_channel_new ();
    if (amq_broker)                     //  Null during self-testing
        self->mgt_connection = amq_connection_new (amq_broker, self);
</method>

<method name = "destroy">
    <action>
    {
    amq_consumer_t
        *consumer;                      //  Consumer in index table

    //  We destroy consumers by asking the respective queues
    while ((consumer = amq_consumer_by_channel_pop (self->consumer_list))) {
        if (amq_queue_cancel (consumer->queue, consumer, FALSE, TRUE))
            //  If the async cancel failed, destroy the consumer ourselves
            amq_consumer_destroy (&consumer);
    }
    amq_connection_destroy (&self->mgt_connection);
    }
    </action>
</method>

<method name = "free">
    amq_consumer_by_channel_destroy (&self->consumer_list);
</method>

<method name = "flow" template = "async function">
    <doc>
    Implements the channel.flow method.  When a channel is restarted,
    we dispatch the channel's virtual host.
    </doc>
    <argument name = "active" type = "Bool">Active consumer?</argument>
    //
    <action>
    amq_consumer_t
        *consumer;                      //  Consumer object reference

    self->active = active;
    consumer = amq_consumer_by_channel_first (self->consumer_list);
    while (consumer) {
        consumer->paused = !active;
        consumer->queue->feed_on = active;
        if (active)
            amq_queue_dispatch (consumer->queue);
        consumer = amq_consumer_by_channel_next (&consumer);
    }
    amq_server_agent_channel_flow_ok (self->connection->thread, self->number, self->active);
    </action>
</method>

<method name = "consume" template = "async function">
    <doc>
    Creates a new channel consumer as specified.  Mechanism is as follows:
    server_channel creates consumer, then attaches it to own consumer list
    and sends to queue so that queue can add consumer to its consumer list.
    </doc>
    <argument name = "queue"  type = "amq_queue_t *">Queue to consume from</argument>
    <argument name = "method" type = "amq_server_method_t *">Consume method</argument>
    //
    <possess>
    queue  = amq_queue_link (queue);
    method = amq_server_method_link (method);
    </possess>
    <release>
    amq_queue_unlink (&queue);
    amq_server_method_unlink (&method);
    </release>
    //
    <action>
    amq_consumer_t
        *consumer = NULL;
    Bool
        nowait = method->payload.basic_consume.nowait;

    if (strused (method->payload.basic_consume.consumer_tag))
        consumer = amq_consumer_table_search (
            self->connection->consumer_table, method->payload.basic_consume.consumer_tag);

    //  If consumer tag specified, ignore re-consume of same tag on same queue
    if (consumer) {
        if (consumer->queue == queue) {
            if (!nowait)
                amq_server_agent_basic_consume_ok (
                    self->connection->thread, self->number, consumer->tag);
        }
        else
            amq_server_channel_error (self, ASL_NOT_ALLOWED, "Consumer tag used on other queue",
                AMQ_SERVER_BASIC, AMQ_SERVER_BASIC_CONSUME);
    }
    else {
        consumer = amq_consumer_new (self->connection, self, queue, method);
        if (consumer) {
            amq_consumer_by_channel_queue (self->consumer_list, consumer);
            if (queue->exclusive)
                self->credit = amq_server_config_private_credit (amq_server_config);
            else
                self->credit = amq_server_config_shared_credit (amq_server_config);
            if (self->credit == 0)
                self->credit = 1;
            amq_queue_consume (queue, consumer, self->active, nowait);
        }
        else {
            amq_server_channel_error (self, ASL_RESOURCE_ERROR, "Unable to create consumer",
                AMQ_SERVER_BASIC, AMQ_SERVER_BASIC_CONSUME);
            smt_log_print (amq_broker->alert_log, "W: cannot create consumer - too many consumers?");
        }
    }
    amq_consumer_unlink (&consumer);
    </action>
</method>

<method name = "ack" template = "async function">
    <doc>
    Implements the Basic.Ack method on all consumers in the channel.
    </doc>
    <argument name = "delivery_tag" type = "int64_t">Delivery tag to ack</argument>
    <argument name = "multiple" type = "Bool">Ack multiple messages?</argument>
    //
    <action>
    amq_consumer_t
        *consumer;                      //  Consumer object reference

    consumer = amq_consumer_by_channel_first (self->consumer_list);
    while (consumer) {
        //  Get queue to do work on consumer so it's synchronized
        amq_queue_consumer_ack (consumer->queue, consumer);
        consumer = amq_consumer_by_channel_next (&consumer);
    }
    </action>
</method>

<method name = "earn">
    <local>
    amq_consumer_t
        *consumer;                      //  Consumer object reference
    </local>
    //
    if (icl_atomic_get32 ((volatile qbyte *) &self->credit) == 1) {
        //  Dispatch all queues for the channel
        consumer = amq_consumer_by_channel_first (self->consumer_list);
        while (consumer) {
            amq_queue_dispatch (consumer->queue);
            consumer = amq_consumer_by_channel_next (&consumer);
        }
    }
</method>

<method name = "cancel" template = "async function" on_shutdown = "1">
    <doc>
    Cancels channel consumer specified by tag.  May be called either
    from method handler - sync true - or from queue agent - sync false.
    Mechanism is as follows: removes consumer from index table and from
    own consumer list.  If sync, then sends to queue so that queue can
    remove from own consumer list.  If not sync, destroys consumer.
    </doc>
    <argument name = "tag"  type = "char *">Consumer tag</argument>
    <argument name = "sync" type = "Bool">Are we talking to a client?</argument>
    <argument name = "nowait" type = "Bool">No reply method wanted</argument>
    //
    <possess>
    tag = icl_mem_strdup (tag);
    </possess>
    <release>
    icl_mem_free (tag);
    </release>
    //
    <action>
    amq_consumer_t
        *consumer = NULL;               //  Consumer reference
    amq_queue_t
        *queue;

    consumer = amq_consumer_table_search (self->connection->consumer_table, tag);
    if (consumer) {
        amq_consumer_by_channel_remove (consumer);
        if (sync) {
            //  Pass to queue to do the final honours
            queue = amq_queue_link (consumer->queue);
            if (queue) {
                //  Send request to queue, but if queue is just dying, this
                //  can fail, and leave a dangling consumer.
                if (amq_queue_cancel (queue, consumer, TRUE, nowait))
                    amq_consumer_unlink (&consumer);
                amq_queue_unlink (&queue);
            }
            else
                amq_consumer_unlink (&consumer);
        }
        else {
            //  Consumer must have been removed from its per-queue list
            assert (consumer->by_queue_next == consumer);
            amq_consumer_destroy (&consumer);
        }
    }
    else
    if (sync)
        amq_server_agent_basic_cancel_ok (self->connection->thread, self->number, tag);
    </action>
</method>

<method name = "selftest">
    <local>
    amq_server_channel_table_t
        *table;
    amq_server_channel_t
        *channel;
    int
        count;
    dbyte
        channel_nbr;
    </local>

    smt_initialise ();
    table = amq_server_channel_table_new ();

    //  Let's try some edge cases
    channel = amq_server_channel_new (table, NULL, AMQ_SERVER_CHANNEL_TABLE_INITIAL_SIZE - 1);
    amq_server_channel_destroy (&channel);
    smt_wait (0);

    channel = amq_server_channel_new (table, NULL, AMQ_SERVER_CHANNEL_TABLE_INITIAL_SIZE);
    amq_server_channel_destroy (&channel);
    smt_wait (0);

    channel = amq_server_channel_new (table, NULL, AMQ_SERVER_CHANNEL_TABLE_INITIAL_SIZE + 1);
    amq_server_channel_destroy (&channel);

    //  Now some random table bashing
    for (count = 0; count < 2000; count++) {
        channel_nbr = randomof (0xffff);
        channel = amq_server_channel_new (table, NULL, channel_nbr);
        amq_server_channel_destroy (&channel);
        smt_wait (0);
    }
    amq_server_channel_table_destroy (&table);
</method>

</class>
