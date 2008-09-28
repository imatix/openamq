<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    amq_consumer_t
        *consumer;
    Bool
        have_active_consumers,
        rejected;
    </local>
    //
    if (amq_server_config_debug_queue (amq_server_config))
        smt_log_print (amq_broker->debug_log,
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

        for (limit_nbr = 0; limit_nbr < self->queue->limits; limit_nbr++)
            if (queue_size &gt;= self->queue->limit_value [limit_nbr])
                limit_action = self->queue->limit_action [limit_nbr];

        switch (limit_action) {
            case AMQ_QUEUE_LIMIT_WARN:
                if (!self->warned) {
                    smt_log_print (amq_broker->alert_log,
                        "I: yellow alert on queue=%s, reached %d messages",
                        self->queue->name, queue_size);
                    self->warned = TRUE;
                }
                break;
            case AMQ_QUEUE_LIMIT_DROP:
                if (!self->dropped) {
                    smt_log_print (amq_broker->alert_log,
                        "W: orange alert on queue=%s, reached %d, dropping new messages", 
                        self->queue->name, queue_size);
                    self->dropped = TRUE;
                }
                self->queue->dropped++;
                content = NULL;         
                break;
            case AMQ_QUEUE_LIMIT_TRIM:
                if (!self->trimmed) {
                    smt_log_print (amq_broker->alert_log,
                        "W: orange alert on queue=%s, reached %d, trimming old messages",
                        self->queue->name, queue_size);
                    self->trimmed = TRUE;
                }
                oldest = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
                amq_content_basic_unlink (&oldest);
                self->queue->dropped++;
                break;
            case AMQ_QUEUE_LIMIT_KILL:
                smt_log_print (amq_broker->alert_log,
                        "E: red alert on queue=%s, reached %d, killing queue", 
                        self->queue->name, queue_size);
                if (self->queue->exclusive)
                    amq_server_connection_error (self->queue->connection,
                        ASL_RESOURCE_ERROR, "Queue overflow, connection killed",
                        AMQ_SERVER_BASIC, AMQ_SERVER_BASIC_PUBLISH);
                else
                    amq_queue_self_destruct (self->queue);
                break;
        }
    }
    else 
    if (queue_size == 0) {
        //  Reset warning flags if queue becomes empty
        self->warned  = FALSE;
        self->dropped = FALSE;
        self->trimmed = FALSE;
    }
    if (content) {
        //  If immediate, and no consumers, return the message
        rejected = FALSE;
        if (immediate) {
            //  Check if there are any active consumers
            have_active_consumers = FALSE;
            consumer = amq_consumer_by_queue_first (self->consumer_list);
            while (consumer) {
                if (!consumer->paused) {
                    have_active_consumers = TRUE;
                    amq_consumer_unlink (&consumer);
                    break;
                }
                consumer = amq_consumer_by_queue_next (&consumer);
            }
            if (!have_active_consumers) {
                rejected = TRUE;

                if (amq_server_config_debug_queue (amq_server_config))
                    smt_log_print (amq_broker->debug_log,
                        "Q: return   queue=%s message=%s",
                        self->queue->name, content->message_id);

                content->returned = TRUE;
                //  Connection and channel will be null for messages published
                //  by an internal agent rather than an external application.
                //  In that case we do not return undeliverable messages.
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
        }
        if (!rejected) {
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
    Bool
        have_active_consumers;
    </local>
    //
    if (amq_server_config_debug_queue (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "Q: dispatch queue=%s nbr_messages=%d nbr_consumers=%d",
            self->queue->name,
            ipr_looseref_list_count (self->content_list),
            amq_consumer_by_queue_count (self->consumer_list));

    //  This is an optimization to stop us looping over content below
    //  if there are only passive consumers on the queue.
    //  Get count of active consumers
    have_active_consumers = FALSE;
    consumer = amq_consumer_by_queue_first (self->consumer_list);
    while (consumer) {
        if (!consumer->paused) {
            have_active_consumers = TRUE;
            amq_consumer_unlink (&consumer);
            break;
        }
        consumer = amq_consumer_by_queue_next (&consumer);
    }
    if (amq_server_config_debug_queue (amq_server_config) 
    && !have_active_consumers)
        smt_log_print (amq_broker->debug_log,
            "Q: paused   queue=%s nbr_messages=%d nbr_consumers=%d",
            self->queue->name,
            ipr_looseref_list_count (self->content_list),
            amq_consumer_by_queue_count (self->consumer_list));

    while (have_active_consumers 
    && ipr_looseref_list_count (self->content_list)) {
        //  Look for a valid consumer for this content
        content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
        rc = s_get_next_consumer (self, content->producer_id, &consumer);

        if (rc == CONSUMER_FOUND) {
            if (amq_server_config_debug_queue (amq_server_config))
                smt_log_print (amq_broker->debug_log,
                    "Q: deliver  queue=%s message=%s",
                    self->queue->name, content->message_id);

            self->queue->contents_out++;
            self->queue->traffic_out += content->body_size;
            channel = amq_server_channel_link (consumer->channel);
            if (channel) {
                amq_server_channel_spend (channel);
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
            amq_consumer_by_queue_queue (self->consumer_list, consumer);
            amq_content_basic_unlink (&content);
            amq_consumer_unlink (&consumer);
        }
        else
        if (rc == CONSUMER_BUSY) {
            if (amq_server_config_debug_queue (amq_server_config))
                smt_log_print (amq_broker->debug_log,
                    "Q: busy     queue=%s message=%s",
                    self->queue->name, content->message_id);

            //  No consumers right now, push content back onto list
            ipr_looseref_push (self->content_list, content);
            break;
        }
        else
        //  CONSUMER_PAUSED can be returned if a consumer was paused since we
        //  checked before entering the loop.  In this case, just treat it the
        //  same as CONSUMER_BUSY.  TODO: Find a way to avoid this situation
        if (rc == CONSUMER_PAUSED) {
            if (amq_server_config_debug_queue (amq_server_config))
                smt_log_print (amq_broker->debug_log,
                    "Q: paused   queue=%s message=%s",
                    self->queue->name, content->message_id);

            //  No consumers right now, push content back onto list
            ipr_looseref_push (self->content_list, content);
            break; 
        }
        else
        if (rc == CONSUMER_NONE) {
            //  No consumers at all for content, send back to originator
            //  if the immediate flag was set, else discard it.
            //  FIXME: This is old clustering code, what we should probably
            //  be doing here is returning the content to the originating
            //  connection, if possible?  Isn't the check in _publish
            //  sufficient?
            if (content->immediate && !content->returned) {
                if (amq_server_config_debug_queue (amq_server_config))
                    smt_log_print (amq_broker->debug_log,
                        "Q: return   queue=%s message=%s",
                        self->queue->name, content->message_id);
            }
            else {
                if (amq_server_config_debug_queue (amq_server_config))
                    smt_log_print (amq_broker->debug_log,
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
    icl_shortstr_t
        sender_id;
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
        else {
            icl_shortstr_fmt (sender_id, "%s|%d", connection->key, channel->number);
            amq_server_agent_basic_get_empty (
                connection->thread, channel->number, sender_id);
        }

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
