<?xml version="1.0"?>
<protocol
    comment = "OpenAMQ server protocol methods"
    script  = "asl_gen"
    chassis = "server"
    >
<inherit name = "amq_cluster" />
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

                    if (exchange) {
                        //  Create exchange on all cluster peers
                        if (amq_cluster->enabled
                        &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER)
                            amq_cluster_tunnel_out (amq_cluster,
                                AMQ_CLUSTER_ALL, self, AMQ_CLUSTER_DURABLE, channel);
                    }
                    else
                        amq_server_connection_error (connection,
                            ASL_RESOURCE_ERROR, "Unable to declare exchange");
                }
            }
        }
        if (exchange) {
            if (exchange->type == exchange_type)
                amq_server_agent_exchange_declare_ok (connection->thread, channel->number);
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
    exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
    if (exchange) {
        //  Delete exchange on all cluster peers
        if (amq_cluster->enabled
        &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER)
            amq_cluster_tunnel_out (amq_cluster,
                AMQ_CLUSTER_ALL, self, AMQ_CLUSTER_DURABLE, channel);

        //  Tell client delete was successful
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
    if (strnull (method->queue)) {
        if (amq_cluster->enabled)
            icl_shortstr_fmt (method->queue, "%s:tmp_%06d",
                amq_broker->name, icl_atomic_inc32 (&queue_index));
        else
            icl_shortstr_fmt (method->queue, "tmp_%06d", icl_atomic_inc32 (&queue_index));
    }
    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (!queue) {
        if (method->passive)
            amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
        else {
            queue = amq_queue_new (
                vhost,
                method->exclusive? connection: NULL,
                method->queue,
                method->durable,
                method->exclusive,
                //  Setting the 'exclusive' flag always implies 'auto-delete'
                method->exclusive? TRUE: method->auto_delete);

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

                if (amq_cluster->enabled
                &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER) {
                    if (method->exclusive)
                        //  Forward private queue default binding to all nodes
                        amq_cluster_bind_exchange (amq_cluster, NULL, queue->name, NULL);
                    else
                        //  Forward queue.declare to all nodes
                        amq_cluster_tunnel_out (amq_cluster,
                            AMQ_CLUSTER_ALL, self, AMQ_CLUSTER_DURABLE, channel);
                }
            }
            else
                amq_server_connection_error (connection,
                    ASL_RESOURCE_ERROR, "Unable to declare queue");
        }
    }
    if (queue) {
        if (queue->connection == NULL)
            icl_console_print ("W: queue.declare aborted by connection close");
        else
        if (method->exclusive && queue->connection != connection) {
            icl_console_print ("### QUEUE name=%s/%s connection=%pp/%pp",
                        method->queue, queue->name, connection, queue->connection);
            amq_server_channel_error (
                channel,
                ASL_ACCESS_REFUSED,
                "Queue cannot be made exclusive to this connection");
        }
        else
            amq_server_agent_queue_declare_ok (
                connection->thread,
                channel->number,
                queue->name,
                amq_queue_message_count (queue),
                amq_queue_consumer_count (queue));

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
    exchange = amq_exchange_table_search (vhost->exchange_table, method->exchange);
    if (exchange) {
        queue = amq_queue_table_search (vhost->queue_table, method->queue);
        if (queue) {
            amq_exchange_bind_queue (
                exchange, channel, queue, method->routing_key, method->arguments);
            amq_server_agent_queue_bind_ok (
                connection->thread, channel->number);

            //  Tell cluster about new queue binding
            if (amq_cluster->enabled
            &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER) {
                if (queue->clustered)
                    //  Make same binding on all cluster peers
                    amq_cluster_tunnel_out (amq_cluster,
                        AMQ_CLUSTER_ALL, self, AMQ_CLUSTER_DURABLE, channel);
                else
                    //  Make exchange-to-exchange binding on all cluster peers
                    amq_cluster_bind_exchange (amq_cluster,
                        method->exchange, method->routing_key, method->arguments);
            }
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
    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (queue) {
        //  Delete the queue on all cluster peers
        if (amq_cluster->enabled
        &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER
        &&  queue->clustered)
            amq_cluster_tunnel_out (amq_cluster,
                AMQ_CLUSTER_ALL, self, AMQ_CLUSTER_DURABLE, channel);

        //  Tell client we deleted the queue ok
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
    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (queue) {
        //  Purge queue on all cluster peers, using
        if (amq_cluster->enabled
        &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER
        &&  queue->clustered)
            amq_cluster_tunnel_out (amq_cluster,
                AMQ_CLUSTER_ALL, self, AMQ_CLUSTER_TRANSIENT, channel);

        amq_queue_purge (queue, channel);
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
    if (strlen (method->consumer_tag) > 127
    &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER)
        amq_server_connection_error (connection,
            ASL_SYNTAX_ERROR, "Consumer tag exceeds limit of 127 chars");
    else {
        queue = amq_queue_table_search (vhost->queue_table, method->queue);
        if (queue) {
            //  The channel is responsible for creating/cancelling consumers
            amq_server_channel_consume (channel, queue, self);
            amq_queue_unlink (&queue);
        }
        else
            amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
    }
  </action>

  <action name = "publish">
    <local>
    static qbyte
        sequence = 0;                   //  For intra-cluster broadcasting
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
            self->sequence = ++sequence;
            amq_content_$(class.name)_set_routing_key (
                self->content,
                method->exchange,
                method->routing_key,
                connection->id);

            //  Set cluster-id on all fresh content coming from applications
            if (connection->group != AMQ_CONNECTION_GROUP_CLUSTER)
                amq_content_$(class.name)_set_cluster_id (self->content, channel->cluster_id);

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
    queue = amq_queue_table_search (vhost->queue_table, method->queue);
    if (queue) {
        //  Pass request to cluster master if we are not he
        if (amq_cluster->enabled
        &&  connection->group != AMQ_CONNECTION_GROUP_CLUSTER
        &&  queue->clustered
        && !amq_broker->master)
            amq_cluster_tunnel_out (amq_cluster,
                AMQ_CLUSTER_MASTER, self, AMQ_CLUSTER_TRANSIENT, channel);
        else
            amq_queue_get (queue, channel, self->class_id, NULL);

        amq_queue_unlink (&queue);
    }
    else
        amq_server_channel_error (channel, ASL_NOT_FOUND, "No such queue defined");
  </action>

  <action name = "cancel">
    amq_server_channel_cancel (channel, method->consumer_tag, TRUE);
  </action>
</class>

<!-- File -->
<class name = "file">
  <action name = "consume" sameas = "basic" />
  <action name = "publish" sameas = "basic" />
  <action name = "cancel"  sameas = "basic" />
</class>

<class name = "tunnel">
  <action name = "request">
    method = NULL;    //  Prevent compiler warning on unused method variable
    if (amq_cluster->enabled
    &&  connection->group == AMQ_CONNECTION_GROUP_CLUSTER)
        amq_cluster_tunnel_in (amq_cluster, self->content, channel);
    else
        amq_server_connection_error (connection, ASL_NOT_ALLOWED, "Method not allowed");
  </action>
</class>

</protocol>
