<?xml?>
<class
    name      = "amq_queue_jms"
    comment   = "JMS content queue manager"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the JMS content queue manager.  This class
runs lock-free as a child of the asynchronous queue class.
</doc>

<inherit class = "amq_queue_base" />

<context>
    ipr_looseref_list_t
        *content_list;                  //  List of message contents
</context>

<method name = "new">
    self->content_list = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <local>
    amq_content_jms_t
        *content;                       //  Content object reference
    </local>
    //
    while ((content = (amq_content_jms_t *) ipr_looseref_pop (self->content_list)))
        amq_content_jms_destroy (&content);

    ipr_looseref_list_destroy (&self->content_list);
</method>

<method name = "publish" template = "function">
    <doc>
    Publish message content onto queue.  If the message was marked
    as "immediate" and could not be dispatched, bounces it back to
    the producer.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "content"   type = "amq_content_jms_t *">Message content</argument>
    <argument name = "immediate" type = "Bool">Warn if no consumers?</argument>
    //
    /*  Limitations of current design:
        - no acknowledgements
        - no windowing
     */
    //  We queue and then call the dispatcher, which has all the logic
    //  needed to find a consumer for the message...
    if (amq_server_config_trace_queue (amq_server_config))
        icl_console_print ("Q: publish  queue=%s message=%s",
            self->queue->key,
            content->message_id);

    amq_content_jms_possess (content);
    ipr_looseref_queue (self->content_list, content);

    //  Dispatch and handle case where no message was sent
    if (self_dispatch (self) == 0) {
        if (immediate) {
            //  Bounce content back to publisher
            if (amq_server_config_trace_queue (amq_server_config))
                icl_console_print ("Q: bounce   queue=%s message=%s",
                    self->queue->key,
                    content->message_id);

            ipr_looseref_pop (self->content_list);
            if (amq_server_channel_alive (channel))
                amq_server_agent_jms_bounce (
                    channel->connection->thread,
                    (dbyte) channel->key,
                    content,
                    ASL_NOT_DELIVERED,
                    "No immediate consumers for JMS message",
                    content->exchange,
                    content->destination);
            amq_content_jms_destroy (&content);
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
    amq_content_jms_t
        *content;                       //  Content object reference
    </local>
    //
    if (amq_server_config_trace_queue (amq_server_config))
        icl_console_print ("Q: dispatch queue=%s nbr_messages=%d nbr_consumers=%d",
            self->queue->key,
            ipr_looseref_list_count (self->content_list),
            amq_consumer_by_queue_count (self->active_consumers));

    while (ipr_looseref_list_count (self->content_list)
    && amq_consumer_by_queue_count (self->active_consumers)) {
        content = (amq_content_jms_t *) ipr_looseref_pop (self->content_list);
        assert (content);
        consumer = s_get_next_consumer (self, content->producer_id);
        if (!consumer) {
            if (amq_server_config_trace_queue (amq_server_config))
                icl_console_print ("Q: finish  queue=%s reason=no_consumers",
                    self->queue->key);

            //  If no consumer for content, push back to front of queue
            ipr_looseref_push (self->content_list, content);
            break;                      //  No available consumers
        }
        if (amq_server_agent_jms_deliver (
            consumer->channel->connection->thread,
            (dbyte) consumer->channel->key,
            content,
            0,                          //  Delivery tag
            FALSE,                      //  Redelivered
            content->exchange,
            content->destination,
            self->queue->scope,
            self->queue->name) == 0) {

            if (amq_server_config_trace_queue (amq_server_config))
                icl_console_print ("Q: deliver  queue=%s message=%s",
                    self->queue->key,
                    content->message_id);

            //  Move consumer to end of queue to implement a round-robin
            amq_consumer_by_queue_queue (self->active_consumers, consumer);
            amq_consumer_unlink (&consumer);
            amq_content_jms_destroy (&content);
            rc++;
        }
        else
            //  In the rare case this fails, push content back to front of queue
            ipr_looseref_push (self->content_list, content);
    }
</method>

<method name = "browse" template = "function">
    <doc>
    Returns next message off queue, if any.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *" />
    <local>
    amq_content_jms_t
        *content;                       //  Content object reference
    </local>
    //
    //  Get next message off list, if any
    if (amq_server_channel_alive (channel)) {
        content = (amq_content_jms_t *) ipr_looseref_pop (self->content_list);
        if (content) {
            amq_server_agent_jms_browse_ok (
                channel->connection->thread,
                (dbyte) channel->key,
                content,
                0,                      //  Delivery tag
                FALSE,                  //  Redelivered
                content->exchange,
                content->destination,
                ipr_looseref_list_count (self->content_list));
            amq_content_jms_destroy (&content);
        }
        else
            amq_server_agent_jms_browse_empty (
                channel->connection->thread,
                (dbyte) channel->key);
    }
</method>

</class>
