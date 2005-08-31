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
  <action name = "open">
    amq_server_channel_open (channel, method->virtual_host);
  </action>
</class>

<!-- EXCHANGE -->

<class name = "exchange">
  <action name = "declare">
    <local>
    amq_exchange_t
        *exchange;
    int
        exchange_class;
    </local>
    //
    exchange_class = amq_exchange_class_lookup (method->class);
    if (exchange_class) {
        //  Find exchange and create if necessary
        exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
        if (!exchange) {
            if (method->passive)
                amq_server_channel_close (
                    channel, ASL_NOT_FOUND, "No such exchange defined");
            else {
                exchange = amq_exchange_new (
                    amq_vhost->exchange_table,
                    exchange_class,
                    method->exchange,
                    method->durable,
                    method->auto_delete,
                    method->internal);
                if (!exchange)
                    amq_server_channel_close (
                        channel, ASL_RESOURCE_ERROR, "Unable to declare exchange");
            }
        }
        if (exchange) {
            if (exchange->class == exchange_class)
                amq_server_agent_exchange_declare_ok (
                    channel->connection->thread, (dbyte) channel->key);
            else
                amq_server_channel_close (
                    channel, ASL_NOT_ALLOWED, "Exchange exists with different class");

            amq_exchange_unlink (&exchange);
        }
    }
    else
        amq_server_channel_close (
            channel, ASL_COMMAND_INVALID, "Unknown exchange class");
  </action>

  <action name = "bind">
    <local>
    amq_exchange_t
        *exchange,                  //  Exchange we're working with
        *bind_to;                   //  Exchange to bind to
    </local>
    bind_to = amq_exchange_search (amq_vhost->exchange_table, method->bind_to);
    if (bind_to) {
        exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
        if (exchange) {
            amq_exchange_bind_exchange (bind_to, channel, exchange, method->arguments);
            amq_exchange_unlink (&exchange);
        }
        else
            amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
        amq_exchange_unlink (&bind_to);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "delete">
    <local>
    amq_exchange_t
        *exchange;
    </local>
    exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
    if (exchange) {
        amq_server_agent_exchange_delete_ok (
            channel->connection->thread, (dbyte) channel->key);
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
    static icl_shortstr_t
        queue_name;
    </local>
    //
    //  Find queue and create if necessary
    if (*method->queue)
        icl_shortstr_cpy (queue_name, method->queue);
    else
        icl_shortstr_fmt (queue_name, "tmp_%06d", apr_atomic_inc32 (&queue_index));

    queue = amq_queue_search (amq_vhost->queue_table, method->scope, queue_name);
    if (!queue) {
        if (method->passive)
            amq_server_channel_close (
                channel, ASL_NOT_FOUND, "No such queue defined");
        else {
            queue = amq_queue_new (
                amq_vhost,
                method->scope,
                queue_name,
                method->durable,
                method->private,
                method->auto_delete);
            if (!queue)
                amq_server_channel_close (
                    channel, ASL_RESOURCE_ERROR, "Unable to declare queue");
        }
    }
    if (queue) {
        amq_server_agent_queue_declare_ok (
            channel->connection->thread, (dbyte) channel->key, queue_name, NULL, 0, 0);
        amq_queue_list_queue (amq_vhost->queue_list, queue);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "bind">
    <local>
    amq_exchange_t
        *bind_to;                   //  Exchange to bind to
    amq_queue_t
        *queue;
    </local>
    bind_to = amq_exchange_search (amq_vhost->exchange_table, method->bind_to);
    if (bind_to) {
        queue = amq_queue_search (amq_vhost->queue_table, method->scope, method->queue);
        if (queue) {
            amq_exchange_bind_queue (bind_to, channel, queue, method->arguments);
            amq_queue_unlink (&queue);
        }
        else
            amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
        amq_exchange_unlink (&bind_to);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "delete">
    <local>
    amq_queue_t
        *queue;
    </local>
    queue = amq_queue_search (amq_vhost->queue_table, method->scope, method->queue);
    if (queue) {
        amq_server_agent_queue_delete_ok (
            channel->connection->thread, (dbyte) channel->key, 0);
        amq_queue_destroy (&queue);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>
</class>

<!-- BASIC -->

<class name = "basic">
  <action name = "publish" sameas = "jms" />
  <action name = "browse"  sameas = "jms" />

  <action name = "consume">
    //  The channel is responsible for creating/cancelling consumers
    amq_server_channel_consume (channel,
        method->scope,
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
    exchange = amq_exchange_search (amq_vhost->exchange_table, method->exchange);
    if (exchange) {
        if (!exchange->internal) {
            amq_content_$(class.name)_set_routing (
                self->content,
                method->exchange,
                method->destination,
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
            amq_server_channel_close (
                channel, ASL_ACCESS_REFUSED, "Exchange is for internal use only");
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such exchange defined");
  </action>

  <action name = "browse">
    <local>
    amq_queue_t
        *queue;
    </local>
    queue = amq_queue_search (amq_vhost->queue_table, method->scope, method->queue);
    if (queue) {
        amq_queue_browse (queue, channel, self->class_id);
        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_close (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "consume">
    //  The channel is responsible for creating/cancelling consumers
    amq_server_channel_consume (channel,
        method->scope,
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
