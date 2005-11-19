<?xml version="1.0"?>
<protocol
    comment = "OpenAMQ server protocol methods"
    script  = "asl_gen"
    chassis = "server"
    >
<inherit name = "amq" />
<inherit name = "asl_server" />
<option name = "product_name" value = "OpenAMQ Server" />

<!-- CHANNEL -->

<class name = "channel">
  <action name = "flow">
    amq_server_channel_flow (channel, method->active);
  </action>
</class>

<!-- EXCHANGE -->

<class name = "exchange">
  <action name = "declare">
    <local>
    amq_exchange_t
        *exchange;
    int
        exchange_type = 0;
    </local>
    //
    if (ipr_str_prefixed (method->exchange, "amq.") && !method->passive)
        amq_server_connection_exception (connection, ASL_NOT_ALLOWED,
            "Exchange name not allowed");
    else
        exchange_type = amq_exchange_type_lookup (method->type);
        
    if (exchange_type) {
        //  Find exchange and create if necessary
        exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
        if (!exchange) {
            if (method->passive)
                amq_server_channel_close (
                    channel, ASL_NOT_FOUND, "No such exchange defined");
            else {
                exchange = amq_exchange_new (
                    amq_vhost->exchange_table,
                    amq_vhost,
                    exchange_type,
                    method->exchange,
                    method->durable,
                    method->auto_delete,
                    method->internal);
                if (!exchange)
                    amq_server_connection_exception (connection, ASL_RESOURCE_ERROR,
                        "Unable to declare exchange");
            }
        }
        if (exchange) {
            if (exchange->type == exchange_type)
                amq_server_agent_exchange_declare_ok (
                    connection->thread, (dbyte) channel->key);
            else
                amq_server_connection_exception (connection, ASL_NOT_ALLOWED,
                    "Exchange exists with different type");

            amq_exchange_unlink (&exchange);
        }
    }
    else
        amq_server_connection_exception (connection, ASL_COMMAND_INVALID,
            "Unknown exchange type");
  </action>

  <action name = "delete">
    <local>
    amq_exchange_t
        *exchange;
    </local>
    exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
    if (exchange) {
        amq_server_agent_exchange_delete_ok (
            connection->thread, (dbyte) channel->key);
        amq_exchange_destroy (&exchange);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such exchange defined");
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
    icl_shortstr_t
        queue_name;
    </local>
    //
    //  Find queue and create if necessary
    if (*method->queue)
        icl_shortstr_cpy (queue_name, method->queue);
    else
        icl_shortstr_fmt (queue_name, "tmp_%06d", icl_atomic_inc32 (&queue_index));

    queue = amq_queue_table_search (amq_vhost->queue_table, queue_name);
    if (!queue) {
        if (method->passive)
            amq_server_channel_close (
                channel, ASL_NOT_FOUND, "No such queue defined");
        else {
            queue = amq_queue_new (
                amq_vhost,
                connection->context_id,
                queue_name,
                method->durable,
                method->exclusive,
                method->auto_delete);
            if (queue) {
                //  Make default binding, if wanted
                if (amq_vhost->default_exchange)
                    amq_exchange_bind_queue (
                        amq_vhost->default_exchange, NULL, queue, queue->name, NULL);

                //  Add to connection's exclusive queue list
                if (method->exclusive)
                    amq_server_connection_own_queue (connection, queue);
            }
            else
                amq_server_connection_exception (connection, ASL_RESOURCE_ERROR,
                    "Unable to declare queue");
        }
    }
    if (queue) {
        if (method->exclusive && queue->owner_id != connection->context_id)
            amq_server_channel_close (connection, ASL_ACCESS_REFUSED,
                "Queue cannot be made exclusive to this connection");
        else {
            amq_server_agent_queue_declare_ok (
                connection->thread,
                (dbyte) channel->key,
                queue->name,
                amq_queue_message_count (queue),
                amq_queue_consumer_count (queue));
        }
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "bind">
    <local>
    amq_exchange_t
        *exchange;                      //  Exchange to bind to
    amq_queue_t
        *queue;
    </local>
    exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
    if (exchange) {
        queue = amq_queue_table_search (amq_vhost->queue_table, method->queue);
        if (queue) {
            amq_exchange_bind_queue (
                exchange, channel, queue, method->routing_key, method->arguments);
            amq_queue_unlink (&queue);
        }
        else
            amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
        amq_exchange_unlink (&exchange);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "delete">
    <local>
    amq_queue_t
        *queue;
    </local>
    queue = amq_queue_table_search (amq_vhost->queue_table, method->queue);
    if (queue) {
        amq_server_agent_queue_delete_ok (
            connection->thread, (dbyte) channel->key, amq_queue_message_count (queue));
        amq_queue_destroy (&queue);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "purge">
    <local>
    amq_queue_t
        *queue;
    </local>
    queue = amq_queue_table_search (amq_vhost->queue_table, method->queue);
    if (queue) {
        amq_queue_purge (queue, channel);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>
</class>

<!-- BASIC -->

<class name = "basic">
  <action name = "publish" sameas = "jms" />
  <action name = "get"     sameas = "jms" />

  <action name = "consume">
    //  The channel is responsible for creating/cancelling consumers
    amq_server_channel_consume (channel,
        method->queue,
        self->class_id,
        0,                              //  No prefetch-size limit
        0,                              //  No prefetch-count limit
        method->no_local,
        TRUE,                           //  auto-ack own messages
        method->exclusive
        );
  </action>

  <action name = "cancel"  sameas = "jms" />
</class>

<!-- JMS -->

<class name = "jms">
  <action name = "publish">
    <local>
    amq_exchange_t
        *exchange;
    </local>
    if (*method->exchange) {
        //  Lookup exchange specified in method
        exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
        if (exchange->internal) {
            amq_server_channel_close (
                channel, ASL_ACCESS_REFUSED, "Exchange is for internal use only");
            amq_exchange_unlink (&exchange);
        }
    }
    else
        //  Get default exchange for virtual host
        exchange = amq_exchange_link (amq_vhost->default_exchange);

    if (exchange) {
        amq_content_$(class.name)_set_routing_key (
            self->content,
            method->exchange,
            method->routing_key,
            connection->context_id);

        amq_exchange_publish (
            exchange,
            channel,
            self->class_id,
            self->content,
            method->mandatory,
            method->immediate);

        amq_exchange_unlink (&exchange);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "get">
    <local>
    amq_queue_t
        *queue;
    </local>
    queue = amq_queue_table_search (amq_vhost->queue_table, method->queue);
    if (queue) {
        amq_queue_get (queue, channel, self->class_id);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "consume">
    //  The channel is responsible for creating/cancelling consumers
    amq_server_channel_consume (channel,
        method->queue,
        self->class_id,
        method->prefetch_size,
        method->prefetch_count,
        method->no_local,
        method->auto_ack,
        method->exclusive
        );
  </action>

  <action name = "cancel">
    amq_server_channel_cancel (channel, method->consumer_tag, TRUE);
  </action>
</class>

</protocol>
