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
        discarded;                      //  Have we already discarded
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
    size_t
        queue_limit = amq_server_config_queue_limit (amq_server_config);
    </local>
    //
    /*  Limitations of current design:
        - no acknowledgements
        - no windowing
     */
    //  We queue and then call the dispatcher, which has all the logic
    //  needed to find a consumer for the message...
    if (amq_server_config_trace_queue (amq_server_config))
        asl_log_print (amq_broker->debug_log,
            "Q: publish  queue=%s message=%s",
            self->queue->name, content->message_id);

    //  If queue is full, drop something...
    //  For exclusive queues, drop oldest messages
    //  For shared queues, drop this message
    if (queue_limit && ipr_looseref_list_count (self->content_list) == queue_limit) {
        if (self->queue->exclusive) {
            amq_content_basic_t
                *oldest;
            oldest = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
            amq_content_basic_unlink (&oldest);
        }
        else
            content = NULL;             //  Discard current content

        if (!self->discarded) {
            asl_log_print (amq_broker->alert_log,
                "W: queue %s exceeded %d messages, dropping data",
                self->queue->name, queue_limit);
            self->discarded = TRUE;
        }
    }
    if (content) {
        amq_content_basic_link (content);
        ipr_looseref_queue (self->content_list, content);
    }
    ipr_meter_count (amq_broker->imeter);
    rc = self_dispatch (self);

    if (rc == CONSUMER_NONE) {
        if (content && immediate) {
            ipr_looseref_pop (self->content_list);
            if (amq_server_channel_alive (channel)
            && !content->returned) {
                amq_server_agent_basic_return (
                    channel->connection->thread,
                    channel->number,
                    content,
                    ASL_NOT_DELIVERED,
                    "No immediate consumers for Basic message",
                    content->exchange,
                    content->routing_key,
                    NULL);
                content->returned = TRUE;

                if (amq_server_config_trace_queue (amq_server_config))
                    asl_log_print (amq_broker->debug_log,
                        "Q: return   queue=%s message=%s",
                        self->queue->name, content->message_id);
            }
            amq_content_basic_unlink (&content);
        }
        else
            amq_queue_pre_dispatch (self->queue);
    }
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatch message content to the active consumers, as far as possible.
    Will deliver messages in strict order to available consumers.  If
    options such as 'no-local' are used, these will cause the entire queue
    to block until a suitable consumer becomes available.  Returns the
    number of messages dispatched.
    </doc>
    <local>
    amq_consumer_t
        *consumer;                      //  Consumer reference
    amq_content_basic_t
        *content;                       //  Content object reference
    </local>
    //
    if (amq_server_config_trace_queue (amq_server_config))
        asl_log_print (amq_broker->debug_log,
            "Q: dispatch queue=%s nbr_messages=%d nbr_consumers=%d",
            self->queue->name,
            ipr_looseref_list_count (self->content_list),
            amq_consumer_by_queue_count (self->active_consumers));

    while (ipr_looseref_list_count (self->content_list)
    && amq_consumer_by_queue_count (self->active_consumers)) {
        content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
        assert (content);

        rc = s_get_next_consumer (
            self, content->producer_id, content->cluster_id, &consumer);

        if (rc == CONSUMER_FOUND) {
            //TODO: need reference count to ensure channel does not disappear
            amq_server_agent_basic_deliver (
                consumer->channel->connection->thread,
                consumer->channel->number,
                content,
                consumer->tag,
                0,                          //  Delivery tag
                FALSE,                      //  Redelivered
                content->exchange,
                content->routing_key,
                consumer);
            if (amq_server_config_trace_queue (amq_server_config))
                asl_log_print (amq_broker->debug_log,
                    "Q: deliver  queue=%s message=%s",
                    self->queue->name, content->message_id);

            //  Move consumer to end of queue to implement a round-robin
            amq_consumer_by_queue_queue (self->active_consumers, consumer);
            amq_content_basic_unlink (&content);
            amq_consumer_unlink (&consumer);
            ipr_meter_count (amq_broker->xmeter);
            rc++;
        }
        else
        if (rc == CONSUMER_BUSY) {
            ipr_looseref_push (self->content_list, content);
            break;                      //  No consumers right now
        }
        else
        if (rc == CONSUMER_NONE) {
            if (amq_server_config_trace_queue (amq_server_config))
                asl_log_print (amq_broker->debug_log,
                    "Q: finish  queue=%s reason=no_consumers", self->queue->name);

            //  If no consumer for content, push back to front of queue
            ipr_looseref_push (self->content_list, content);
            break;                      //  No available consumers
        }
    }
</method>

<method name = "get" template = "function">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *" />
    <argument name = "cluster id" type = "char *">Stamp content with cluster id</argument>
    <local>
    amq_content_basic_t
        *content;                       //  Content object reference
    </local>
    //
    //  Get next message off list, if any
    if (amq_server_channel_alive (channel)) {
        content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
        if (content) {
            if (cluster_id)
                amq_content_basic_set_cluster_id (content, cluster_id);
            amq_server_agent_basic_get_ok (
                channel->connection->thread,
                channel->number,
                content,
                0,                      //  Delivery tag
                FALSE,                  //  Redelivered
                content->exchange,
                content->routing_key,
                ipr_looseref_list_count (self->content_list),
                NULL);
            amq_content_basic_unlink (&content);
            ipr_meter_count (amq_broker->xmeter);
        }
        else
            amq_server_agent_basic_get_empty (
                channel->connection->thread, channel->number, NULL);
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
