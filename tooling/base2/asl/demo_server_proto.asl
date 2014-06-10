<?xml version="1.0"?>
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
<protocol
    comment = "ASL demo server"
    script  = "asl_gen"
    chassis = "server"
    >
<option name = "product_name" value = "ASL Demo Server" />
<inherit name = "demo" />
<inherit name = "asl_server" />

<class name = "channel">
    <action name = "flow">
        channel->active = method->active;
    </action>
</class>

<class name = "exchange">
    <action name = "declare">
        <local>
        demo_exchange_t
            *exchange;
        </local>
        //
        //  Find exchange and create if necessary
        exchange = demo_exchange_search (method->exchange);
        if (!exchange) {
            if (method->passive)
                demo_server_channel_error (
                    channel, ASL_NOT_FOUND, "No such exchange defined",
                    DEMO_SERVER_EXCHANGE, DEMO_SERVER_EXCHANGE_DECLARE);
            else {
                exchange = demo_exchange_new (
                    method->exchange, method->durable, method->auto_delete);
                if (!exchange)
                    demo_server_channel_error (
                        channel, ASL_RESOURCE_ERROR, "Unable to declare exchange",
                        DEMO_SERVER_EXCHANGE, DEMO_SERVER_EXCHANGE_DECLARE);
            }
        }
        if (exchange) {
            demo_server_agent_exchange_declare_ok (
                connection->thread, channel->number);
            demo_exchange_unlink (&exchange);
        }
    </action>

    <action name = "delete">
        <local>
        demo_exchange_t
            *exchange;
        </local>
        //
        exchange = demo_exchange_search (method->exchange);
        if (exchange) {
            demo_server_agent_exchange_delete_ok (
                connection->thread, channel->number);
            demo_exchange_destroy (&exchange);
        }
        else
            demo_server_channel_error (channel, ASL_NOT_FOUND,
                "No such exchange defined",
                DEMO_SERVER_EXCHANGE, DEMO_SERVER_EXCHANGE_DELETE);
    </action>
</class>

<class name = "queue">
    <action name = "declare">
        <local>
        demo_queue_t
            *queue;
        </local>
        //
        //  Find queue and create if necessary
        queue = demo_queue_search (method->queue);
        if (!queue) {
            if (method->passive)
                demo_server_channel_error (
                    channel, ASL_NOT_FOUND, "No such queue defined",
                    DEMO_SERVER_QUEUE, DEMO_SERVER_QUEUE_DECLARE);
            else {
                queue = demo_queue_new (
                    method->queue,
                    method->durable,
                    method->exclusive,
                    method->auto_delete,
                    channel);
                if (!queue)
                    demo_server_channel_error (
                        channel, ASL_RESOURCE_ERROR, "Unable to declare queue",
                        DEMO_SERVER_QUEUE, DEMO_SERVER_QUEUE_DECLARE);
            }
        }
        if (queue) {
            demo_server_agent_queue_declare_ok (
                connection->thread, channel->number, method->queue, NULL);
            demo_queue_unlink (&queue);
        }
    </action>

    <action name = "bind">
        <local>
        demo_exchange_t
            *exchange;
        demo_queue_t
            *queue;
        </local>
        exchange = demo_exchange_search (method->exchange);
        if (exchange) {
            queue = demo_queue_search (method->queue);
            if (queue) {
                demo_exchange_bind (exchange, channel, queue, method->arguments);
                demo_queue_unlink (&queue);
            }
            else
                demo_server_channel_error (channel, ASL_NOT_FOUND,
                    "No such queue defined",
                    DEMO_SERVER_QUEUE, DEMO_SERVER_QUEUE_BIND);
            demo_exchange_unlink (&exchange);
        }
        else
            demo_server_channel_error (channel, ASL_NOT_FOUND,
                "No such exchange defined",
                DEMO_SERVER_QUEUE, DEMO_SERVER_QUEUE_BIND);
    </action>

    <action name = "delete">
        <local>
        demo_queue_t
            *queue;
        </local>
        queue = demo_queue_search (method->queue);
        if (queue) {
            demo_server_agent_queue_delete_ok (
                connection->thread, channel->number, 0);
            demo_queue_destroy (&queue);
        }
        else
            demo_server_channel_error (channel, ASL_NOT_FOUND,
                "No such queue defined",
                DEMO_SERVER_QUEUE, DEMO_SERVER_QUEUE_DELETE);
    </action>
</class>

<class name = "basic">
    <action name = "publish">
        <local>
        demo_exchange_t
            *exchange;
        </local>
        exchange = demo_exchange_search (method->exchange);
        if (exchange) {
            demo_content_$(class.name)_set_routing_key (
                self->content,
                method->exchange,
                method->routing_key,
                connection->id);
            demo_exchange_publish (exchange, channel, self->content);
            demo_exchange_unlink (&exchange);
        }
        else
            demo_server_channel_error (channel, ASL_NOT_FOUND,
                "No such exchange defined",
                DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_PUBLISH);
    </action>
    <action name = "get">
        <local>
        demo_queue_t
            *queue;
        </local>
        queue = demo_queue_search (method->queue);
        if (queue) {
            demo_queue_get (queue, channel);
            demo_queue_unlink (&queue);
        }
        else
            demo_server_channel_error (channel, ASL_NOT_FOUND,
                "No such queue defined",
                DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_GET);
    </action>
    <action name = "consume">
        demo_server_channel_consume (channel, self);
    </action>
    <action name = "cancel">
        demo_server_channel_cancel (channel, self);
    </action>
</class>

<!---
    This is a minimalistic implementation of the direct class
    We check that the sink / feed name is valid and format a
    lease that contains the sink / feed name, for easy decoding.
  -->
<class name = "direct">
    <action name = "put">
        <local>
        demo_lease_t
            *lease;
        </local>
        lease = demo_lease_new (method->sink, DP_SINK, channel);
        if (lease) {
            demo_server_agent_direct_put_ok (
                connection->thread, channel->number, lease->name);
            demo_lease_unlink (&lease);
        }
        else
            demo_server_channel_error (channel, ASL_NOT_FOUND,
                "No such sink",
                DEMO_SERVER_DIRECT, DEMO_SERVER_DIRECT_PUT);
    </action>
    <action name = "get">
        <local>
        demo_lease_t
            *lease;
        </local>
        lease = demo_lease_new (method->feed, DP_FEED, channel);
        if (lease) {
            demo_server_agent_direct_get_ok (
                connection->thread, channel->number, lease->name);
            demo_lease_unlink (&lease);
        }
        else
            demo_server_channel_error (channel, ASL_NOT_FOUND,
                "No such feed",
                DEMO_SERVER_DIRECT, DEMO_SERVER_DIRECT_GET);
    </action>
</class>

</protocol>
