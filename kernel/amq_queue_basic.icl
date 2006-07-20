<?xml?>
<class
    name      = "amq_queue_basic"
    comment   = "Basic content queue manager"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements the basic content queue manager. This class
runs lock-free as a child of the asynchronous queue class.
</doc>

<inherit class = "amq_queue_base" />

<context>
    Bool
        warned,                         //  Have we already warned?
        dropped,                        //  Are we already dropping?
        trimmed;                        //  Are we already trimming?
</context>

<method name = "new">
</method>

<method name = "destroy">
    <local>
    amq_content_basic_t
        *content;                       //  Content object reference
    </local>
    //
    while ((content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list)))
        amq_content_basic_unlink (&content);
</method>

<method name = "publish" template = "function">
    <doc>
    Publish message content onto queue.  If the message was marked as
    "immediate" and could not be dispatched, returns it back to the
    producer.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "content"   type = "amq_content_basic_t *">Message content</argument>
    <argument name = "immediate" type = "Bool">Warn if no consumers?</argument>
    <local>
    qbyte
        queue_size;                     //  Current queue size
    ipr_looseref_t
        *looseref;                      //  Queued message
    amq_server_connection_t
        *connection;
    </local>
    //
    /*  Limitations of current design:
        - no acknowledgements
        - no windowing
     */
    if (amq_server_config_debug_queue (amq_server_config))
        asl_log_print (amq_broker->debug_log,
            "Q: publish  queue=%s message=%s", self->queue->name, content->message_id);

    self->queue->contents_in++;
    self->queue->traffic_in += content->body_size;

    //  Check queue limits
    queue_size = ipr_looseref_list_count (self->content_list);
    if (queue_size > self->queue->limit_min) {
        qbyte
            limit_nbr,
            limit_action = 0;           //  Ultimate action to execute
        amq_content_basic_t
            *oldest;

        connection = channel?
            amq_server_connection_link (channel->connection): NULL;
        if (connection) {
            if (connection->group == AMQ_CONNECTION_GROUP_NORMAL)
                for (limit_nbr = 0; limit_nbr < self->queue->limits; limit_nbr++)
                    if (queue_size &gt;= self->queue->limit_value [limit_nbr])
                        limit_action = self->queue->limit_action [limit_nbr];
            amq_server_connection_unlink (&connection);
        }
        switch (limit_action) {
            case AMQ_QUEUE_LIMIT_WARN:
                if (!self->warned) {
                    asl_log_print (amq_broker->alert_log,
                        "I: yellow alert on queue=%s, reached %d messages",
                        self->queue->name, queue_size);
                    self->warned = TRUE;
                }
                break;
            case AMQ_QUEUE_LIMIT_DROP:
                if (!self->dropped) {
                    asl_log_print (amq_broker->alert_log,
                        "W: orange alert on queue=%s, dropping new messages", 
                        self->queue->name);
                    self->dropped = TRUE;
                }
                self->queue->dropped++;
                content = NULL;         
                break;
            case AMQ_QUEUE_LIMIT_TRIM:
                if (!self->trimmed) {
                    asl_log_print (amq_broker->alert_log,
                        "W: orange alert on queue=%s, trimming old messages",
                        self->queue->name);
                    self->trimmed = TRUE;
                }
                oldest = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
                amq_content_basic_unlink (&oldest);
                self->queue->dropped++;
                break;
            case AMQ_QUEUE_LIMIT_KILL:
                asl_log_print (amq_broker->alert_log,
                        "E: red alert on queue=%s, killing queue", self->queue->name);
                if (self->queue->exclusive)
                    amq_server_connection_error (self->queue->connection,
                        ASL_RESOURCE_ERROR, "Queue overflow, connection killed");
                else
                    amq_queue_self_destruct (self->queue);
                break;
        }
    }
    else {
        //  Reset warning flags if queue drops below critical
        self->warned  = FALSE;
        self->dropped = FALSE;
        self->trimmed = FALSE;
    }
    if (content) {
        //  If immediate, and no consumers, return the message
        if (immediate && amq_consumer_by_queue_count (self->active_consumers) == 0) {
            if (amq_server_config_debug_queue (amq_server_config))
                asl_log_print (amq_broker->debug_log,
                    "Q: return   queue=%s message=%s",
                    self->queue->name, content->message_id);

            content->returned = TRUE;
            connection = channel?
                amq_server_connection_link (channel->connection): NULL;
            if (connection) {
                amq_server_agent_basic_return (
                    connection->thread,
                    channel->number,
                    content,
                    ASL_NOT_DELIVERED,
                    "No immediate consumers for Basic message",
                    content->exchange,
                    content->routing_key,
                    NULL);
                amq_server_connection_unlink (&connection);
            }
        }
        else {
            content->immediate = immediate;
            amq_content_basic_link (content);
            looseref = ipr_looseref_queue (self->content_list, content);
        }
    }
    self_dispatch (self);
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatch message content to the active consumers, as far as possible.
    Will deliver messages in strict order to available consumers.  If
    there are no consumers at all, will hold messages on the queue unless
    they are marked 'immediate'.  If there are consumers, but a message
    cannot be delivered to any of them, the message will be discarded or
    returned (if 'immediate').
    </doc>
    <local>
    amq_consumer_t
        *consumer;                      //  Consumer reference
    amq_content_basic_t
        *content;                       //  Content object reference
    amq_server_connection_t
        *connection;
    amq_server_channel_t
        *channel;
    </local>
    //
    if (amq_server_config_debug_queue (amq_server_config))
        asl_log_print (amq_broker->debug_log,
            "Q: dispatch queue=%s nbr_messages=%d nbr_consumers=%d",
            self->queue->name,
            ipr_looseref_list_count (self->content_list),
            amq_consumer_by_queue_count (self->active_consumers));

    while (ipr_looseref_list_count (self->content_list)
    && amq_consumer_by_queue_count (self->active_consumers)) {
        //  Look for a valid consumer for this content
        content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
        rc = s_get_next_consumer (self, content->producer_id, &consumer);

        if (rc == CONSUMER_FOUND) {
            if (amq_server_config_debug_queue (amq_server_config))
                asl_log_print (amq_broker->debug_log,
                    "Q: deliver  queue=%s message=%s",
                    self->queue->name, content->message_id);

            self->queue->contents_out++;
            self->queue->traffic_out += content->body_size;
            channel = amq_server_channel_link (consumer->channel);
            if (channel) {
                connection = amq_server_connection_link (channel->connection);
                if (connection) {
                    amq_server_agent_basic_deliver (
                        connection->thread,
                        channel->number,
                        content,
                        consumer->tag,
                        0,                  //  Delivery tag
                        FALSE,              //  Redelivered
                        content->exchange,
                        content->routing_key,
                        consumer);
                    amq_server_connection_unlink (&connection);
                }
                amq_server_channel_unlink (&channel);
            }
            //  Move consumer to end of queue to implement a round-robin
            amq_consumer_by_queue_queue (self->active_consumers, consumer);
            amq_content_basic_unlink (&content);
            amq_consumer_unlink (&consumer);
        }
        else
        if (rc == CONSUMER_BUSY) {
            if (amq_server_config_debug_queue (amq_server_config))
                asl_log_print (amq_broker->debug_log,
                    "Q: busy     queue=%s message=%s",
                    self->queue->name, content->message_id);

            //  No consumers right now, push content back onto list
            ipr_looseref_push (self->content_list, content);
            break;
        }
        else
        if (rc == CONSUMER_NONE) {
            //  No consumers at all for content, send back to originator
            //  if the immediate flag was set, else discard it.
            if (content->immediate && !content->returned) {
                amq_server_channel_t
                    *channel;           //  Channel to send message back to

                if (amq_server_config_debug_queue (amq_server_config))
                    asl_log_print (amq_broker->debug_log,
                        "Q: return   queue=%s message=%s",
                        self->queue->name, content->message_id);

#ifdef __DISABLED_CLUSTER_TODO__
//  Need a way to lookup the content to see what channel to return it
//  on.  If not the cluster-tag then what?
                //  We use content's cluster_id for return path
                channel = amq_server_channel_cluster_search (content->cluster_id);
                if (channel) {
                    connection = amq_server_connection_link (channel->connection);
                    if (connection) {
                        content->returned = TRUE;
                        amq_server_agent_basic_return (
                            connection->thread,
                            channel->number,
                            content,
                            ASL_NOT_DELIVERED,
                            "No immediate consumers for Basic message",
                            content->exchange,
                            content->routing_key,
                            NULL);
                        amq_server_connection_unlink (&connection);
                    }
                    amq_server_channel_unlink (&channel);
                }
#endif
            }
            else {
                if (amq_server_config_debug_queue (amq_server_config))
                    asl_log_print (amq_broker->debug_log,
                        "Q: discard  queue=%s message=%s",
                        self->queue->name, content->message_id);
            }
            amq_content_basic_unlink (&content);
        }
    }
</method>

<method name = "get" template = "function">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *" />
    <local>
    amq_content_basic_t
        *content;                       //  Content object reference
    amq_server_connection_t
        *connection;
    </local>
    //
    //  Get next message off list, if any
    content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
    connection = channel?
        amq_server_connection_link (channel->connection): NULL;

    if (connection) {
        if (content) {
            amq_server_agent_basic_get_ok (
                connection->thread,
                channel->number,
                content,
                0,                      //  Delivery tag
                FALSE,                  //  Redelivered
                content->exchange,
                content->routing_key,
                ipr_looseref_list_count (self->content_list),
                NULL);

            amq_content_basic_unlink (&content);
        }
        else
            amq_server_agent_basic_get_empty (
                connection->thread, channel->number, NULL);

        amq_server_connection_unlink (&connection);
    }
</method>

<method name = "purge" template = "function">
    <doc>
    Destroy all queue contents, return number of contents destroyed.
    </doc>
    <local>
    amq_content_basic_t
        *content;                       //  Content object reference
    </local>
    //
    while ((content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list))) {
        amq_content_basic_unlink (&content);
        rc++;
    }
</method>

</class>
