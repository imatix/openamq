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
    Publish message content onto queue. Returns true if the queue had
    one or more active consumers, else false.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "content"   type = "amq_content_jms_t *">Message content</argument>
    <argument name = "immediate" type = "Bool">Warn if no consumers?</argument>
    <local>
    amq_consumer_t
        *consumer;
    </local>
    //
    /*  Limitations of current design:
        - no acknowledgements
        - no windowing
     */
    amq_content_jms_possess (content);
    consumer = s_get_next_consumer (self, content->producer_id);
    if (consumer) {
        amq_server_agent_jms_deliver (
            consumer->channel->connection->thread,
            (dbyte) consumer->channel->key,
            content,
            0,                          //  Delivery tag
            FALSE,                      //  Redelivered
            content->exchange,         
            content->destination,
            self->queue->domain,       
            self->queue->name);        

        //  Move consumer to end of queue to implement a round-robin
        amq_consumer_by_queue_queue (self->active_consumers, consumer);
        amq_consumer_unlink (&consumer);
        rc = TRUE;
    }
    else {
        if (immediate == FALSE) {
            //  No consumers, so put onto queue for now
            ipr_looseref_queue (self->content_list, content);
            amq_queue_pre_dispatch (self->queue);
        }
        else
            //  Bounce message if immediate delivery was requested
            amq_server_agent_jms_bounce (
                channel->connection->thread,
                (dbyte) channel->key,
                content,
                ASL_NOT_DELIVERED,
                "No immediate consumers for JMS message",
                content->exchange,
                content->destination);
    }
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatch message content to the active consumers, as far as possible.
    </doc>
    <local>
    amq_consumer_t
        *consumer;                      //  Consumer reference
    amq_content_jms_t
        *content;                       //  Content object reference
    </local>
    //
    while (ipr_looseref_list_count (self->content_list)
    && amq_consumer_by_queue_count (self->active_consumers)) {
        content = (amq_content_jms_t *) ipr_looseref_pop (self->content_list);
        assert (content);
        consumer = s_get_next_consumer (self, content->producer_id);
        if (!consumer)
            break;                      //  No available consumers

        amq_server_agent_jms_deliver (
            consumer->channel->connection->thread,
            (dbyte) consumer->channel->key,
            content,
            0,                          //  Delivery tag
            FALSE,                      //  Redelivered
            content->exchange,
            content->destination,
            self->queue->domain,
            self->queue->name);

        //  Move consumer to end of queue to implement a round-robin
        amq_consumer_by_queue_queue (self->active_consumers, consumer);
        amq_consumer_unlink (&consumer);
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
        }
        else
            amq_server_agent_jms_browse_empty (
                channel->connection->thread,
                (dbyte) channel->key);
    }
</method>

</class>
