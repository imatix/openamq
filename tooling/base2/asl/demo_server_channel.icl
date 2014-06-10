<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
    name      = "demo_server_channel"
    comment   = "Demo server channel class"
    version   = "1.0"
    script    = "smt_object_gen"
    >
<doc>
    This class implements the ASL demo server channel class.
</doc>

<inherit class = "asl_server_channel" />
<option name = "basename" value = "demo_server" />

<method name = "consume" template = "async function">
    <doc>
    Creates a new channel consumer to specified queue.  Only has an effect
    in Direct Protocol feed access.  Consumers are implemented as leases on
    queues.
    - find queue for consume
    - check queue is exclusive (required for lease)
    - lease may not yet be activated (happens asynchronously)
    - return consume-tag from queue name
    - set queue->feed_on to true
    </doc>
    <argument name = "method" type = "demo_server_method_t *">Consume method</argument>
    //
    <possess>
    method = demo_server_method_link (method);
    </possess>
    <release>
    demo_server_method_unlink (&method);
    </release>
    //
    <action>
    demo_queue_t
        *queue;

    queue = demo_queue_search (method->payload.basic_consume.queue);
    if (queue) {
        if (queue->exclusive && queue->channel == self) {
            demo_server_agent_basic_consume_ok (
                self->connection->thread, 
                self->number, 
                queue->name);
            queue->feed_on = TRUE;
            demo_queue_unlink (&queue);
        }
        else
            demo_server_channel_error (self, ASL_ACCESS_REFUSED, 
                "Access refused to queue",
                DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CONSUME);
    }
    else
        demo_server_channel_error (self, ASL_NOT_FOUND, 
            "No such queue defined", 
            DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CONSUME);
    </action>
</method>

<method name = "cancel" template = "async function" on_shutdown = "1">
    <doc>
    Cancels a channel consumer on specified queue.  Only has an effect in
    Direct Protocol feed access.  Consumers are implemented as leases on
    queues.
    - find queue for cancel (consumer-tag is queue name)
    - check queue is exclusive (required for lease)
    - set queue->feed_on to true
    </doc>
    <argument name = "method" type = "demo_server_method_t *">Cancel method</argument>
    //
    <possess>
    method = demo_server_method_link (method);
    </possess>
    <release>
    demo_server_method_unlink (&method);
    </release>
    //
    <action>
    demo_queue_t
        *queue;

    queue = demo_queue_search (method->payload.basic_cancel.consumer_tag);
    if (queue) {
        if (queue->exclusive && queue->channel == self) {
            demo_server_agent_basic_cancel_ok (
                self->connection->thread, 
                self->number, 
                queue->name);
            queue->feed_on = FALSE;
            demo_queue_unlink (&queue);
        }
        else
            demo_server_channel_error (self, ASL_ACCESS_REFUSED, 
                "Access refused to consumer",
                DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
    }
    else
        demo_server_channel_error (self, ASL_NOT_FOUND, 
            "Invalid consumer tag", 
            DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
    </action>
</method>

</class>
