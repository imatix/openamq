<?xml version="1.0"?>
<!--
    Copyright (c) 1996-2007 iMatix Corporation

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
    comment = "OpenAMQ server protocol methods"
    script  = "asl_gen"
    chassis = "server"
    >
<inherit name = "asl_server" />
<inherit name = "amq" />

<option name = "product_name" value = "OpenAMQ Server" />

<!-- CHANNEL -->

<class name = "channel">
  <action name = "flow">
    amq_server_channel_flow (channel, method->active);
  </action>
</class>

<!-- EXCHANGE -->

<!-- TODO this entire file needs to be redesigned
    1. move the method bodies into the respective objects
    2. allow this layer to be interfaced with other protocol agents
       such as HTTP
    -->
<class name = "exchange">
  <action name = "declare">
    <local>
    amq_exchange_t
        *exchange;
    int
        exchange_type = 0;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    exchange_type = amq_exchange_type_lookup (method->type);
    if (exchange_type) {
        //  Find exchange and create if necessary
        exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
        if (!exchange) {
            if (method->passive)
                amq_server_channel_error (channel, ASL_NOT_FOUND, "No such exchange defined");
            else {
                if (ipr_str_prefixed (method->exchange, "amq."))
                    amq_server_channel_error (channel,
                        ASL_ACCESS_REFUSED, "Exchange name not allowed");
                else {
                    exchange = amq_exchange_new (
                        vhost->exchange_table,
                        vhost,
                        exchange_type,
                        method->exchange,
                        method->durable,
                        method->auto_delete,
                        method->internal);

                    //  This can fail if two threads create the same exchange at the
                    //  same time... so let's go find the actual exchange object
                    if (!exchange)
                        exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
                }
            }
        }
        if (exchange) {
            if (exchange->type == exchange_type) {
                icl_shortstr_cpy (channel->current_exchange, exchange->name);
                if (!method->nowait)
                    amq_server_agent_exchange_declare_ok (connection->thread, channel->number);
            }
            else
                amq_server_connection_error (connection,
                    ASL_NOT_ALLOWED, "Exchange exists with different type");

            amq_exchange_unlink (&exchange);
        }
    }
    else
        amq_server_connection_error (connection, ASL_COMMAND_INVALID, "Unknown exchange type");
  </action>

  <action name = "delete">
    <local>
    amq_exchange_t
        *exchange;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Use current channel exchange if method has a blank name
    if (strnull (method->exchange))
        icl_shortstr_cpy (method->exchange, channel->current_exchange);

    exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
    if (exchange) {
        //  Tell client delete was successful
        if (!method->nowait)
            amq_server_agent_exchange_delete_ok (connection->thread, channel->number);

        //  Destroy the exchange on this peer
        amq_exchange_destroy (&exchange);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>
</class>

<!-- QUEUE -->

<class name = "queue">
  <action name = "declare">
    <local>
    amq_queue_t
        *queue;
    static qbyte
        queue_index = 0;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Find queue and create if necessary
    if (strnull (method->queue))
        icl_shortstr_fmt (method->queue, "#%d", icl_atomic_inc32 (&queue_index));
    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (!queue) {
        if (method->passive)
            amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
        else {
            //  The queue->connection specifies owner connection, for exclusive queues
            queue = amq_queue_new (
                vhost,
                method->exclusive? connection: NULL,
                method->queue,
                method->durable,
                method->exclusive,
                //  Setting the 'exclusive' flag always implies 'auto-delete'
                method->exclusive? TRUE: method->auto_delete,
                method->arguments);

            //  This can fail if two threads create the same queue at the
            //  same time... so let's go find the actual queue object
            if (!queue)
                queue = amq_queue_table_search (vhost->queue_table, method->queue);

            if (queue) {
                //  Make default binding, if wanted
                if (vhost->default_exchange)
                    amq_exchange_bind_queue (
                        vhost->default_exchange, NULL, queue, queue->name, NULL);

                //  Add to connection's exclusive queue list
                if (method->exclusive)
                    amq_server_connection_own_queue (connection, queue);
            }
            else
                amq_server_connection_error (connection,
                    ASL_RESOURCE_ERROR, "Unable to declare queue");
        }
    }
    if (queue) {
        if (method->exclusive && queue->connection == NULL)
            icl_console_print ("W: queue.declare aborted by connection close");
        else
        if (method->exclusive && queue->connection != connection)
            amq_server_channel_error (
                channel,
                ASL_ACCESS_REFUSED,
                "Queue cannot be made exclusive to this connection");
        else {
            //  AMQP requires us to hold a current queue per channel
            icl_shortstr_cpy (channel->current_queue, queue->name);
            if (!method->nowait)
                amq_server_agent_queue_declare_ok (
                    connection->thread,
                    channel->number,
                    queue->name,
                    amq_queue_message_count (queue),
                    amq_queue_consumer_count (queue));
        }
        amq_queue_unlink (&queue);
    }
  </action>

  <action name = "bind">
    <local>
    amq_exchange_t
        *exchange;                      //  Exchange to bind to
    amq_queue_t
        *queue;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Use current channel queue if method uses a blank queue
    if (strnull (method->queue)) {
        icl_shortstr_cpy (method->queue, channel->current_queue);
        if (strnull (method->routing_key))
            icl_shortstr_cpy (method->routing_key, channel->current_queue);
    }
    exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
    if (exchange) {
        queue = amq_queue_table_search (vhost->queue_table, method->queue);
        if (queue) {
            amq_exchange_bind_queue (
                exchange, channel, queue, method->routing_key, method->arguments);
            if (!method->nowait)
                amq_server_agent_queue_bind_ok (connection->thread, channel->number);
            amq_queue_unlink (&queue);
        }
        else
            amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
        amq_exchange_unlink (&exchange);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "unbind">
    <local>
    amq_exchange_t
        *exchange;                      //  Exchange to unbind from 
    amq_queue_t
        *queue;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Use current channel queue if method uses a blank queue
    if (strnull (method->queue)) {
        icl_shortstr_cpy (method->queue, channel->current_queue);
        if (strnull (method->routing_key))
            icl_shortstr_cpy (method->routing_key, channel->current_queue);
    }
    exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
    if (exchange) {
        queue = amq_queue_table_search (vhost->queue_table, method->queue);
        if (queue) {
            amq_exchange_protocol_unbind_queue (
                exchange, channel, queue, method->routing_key, method->arguments);
            if (!method->nowait)
                amq_server_agent_queue_unbind_ok (connection->thread, channel->number);

            /*  TODO: Clustering code missing  */
            amq_queue_unlink (&queue);
        }
        else
            amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
        amq_exchange_unlink (&exchange);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "delete">
    <local>
    amq_queue_t
        *queue;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Use current channel queue if method uses a blank queue
    if (strnull (method->queue))
        icl_shortstr_cpy (method->queue, channel->current_queue);

    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (queue) {
        //  Tell client we deleted the queue ok
        if (!method->nowait)
            amq_server_agent_queue_delete_ok (
                connection->thread, channel->number, amq_queue_message_count (queue));

        //  Destroy the queue on this peer
        amq_vhost_unbind_queue (vhost, queue);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "purge">
    <local>
    amq_queue_t
        *queue;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Use current channel queue if method uses a blank queue
    if (strnull (method->queue))
        icl_shortstr_cpy (method->queue, channel->current_queue);

    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (queue) {
        amq_queue_purge (queue, channel, method->nowait);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>
</class>

<!-- Basic -->

<class name = "basic">
  <action name = "consume">
    <local>
    amq_queue_t
        *queue;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Use current channel queue if method uses a blank queue
    if (strnull (method->queue))
        icl_shortstr_cpy (method->queue, channel->current_queue);

    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (queue) {
        //  The channel is responsible for creating/cancelling consumers
        amq_server_channel_consume (channel, queue, self, method->nowait);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "publish">
    <local>
    amq_exchange_t
        *exchange;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    if (*method->exchange)
        //  Lookup exchange specified in method
        exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
    else
        //  Get default exchange for virtual host
        exchange = amq_exchange_link (vhost->default_exchange);

    if (exchange) {
        if (!exchange->internal || strnull (method->exchange)) {
            //  This method may only come from an external application
            assert (connection);
            amq_content_$(class.name)_set_routing_key (
                content,
                method->exchange,
                method->routing_key,
                connection->id);

            amq_exchange_publish (exchange, channel, self);
        }
        else
            amq_server_channel_error (channel,
                ASL_ACCESS_REFUSED, "Exchange is for internal use only");

        amq_exchange_unlink (&exchange);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "get">
    <local>
    amq_queue_t
        *queue;
    </local>
    <local>
    amq_vhost_t
        *vhost;
    </local>
    <header>
    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
    </header>
    <footer>
        amq_vhost_unlink (&vhost);
    }
    else
        amq_server_connection_error (connection, ASL_CONNECTION_FORCED, "Server not ready");
    </footer>
    //
    //  Use current channel queue if method uses a blank queue
    if (strnull (method->queue))
        icl_shortstr_cpy (method->queue, channel->current_queue);

    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (queue) {
        amq_queue_get (queue, channel, self->class_id);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "cancel">
    amq_server_channel_cancel (channel, method->consumer_tag, TRUE, method->nowait);
  </action>
</class>

<!-- File -->
<class name = "file">
  <action name = "consume" sameas = "basic" />
  <action name = "cancel"  sameas = "basic" />
</class>

</protocol>
