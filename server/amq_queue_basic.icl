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

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_server_classes" />

<context>
    amq_queue_t
        *queue;                         //  Parent queue
    amq_consumer_by_queue_t
        *consumer_list;                 //  List of consumers from the queue
    ipr_looseref_list_t
        *content_list;                  //  List of message contents

    //  Statistics
    size_t
        accept_count,                   //  Number of messages accepted
        dispatch_count;                 //  Number of messages dispatched
    int64_t
        delivery_tag;                   //  Message delivery tag
</context>

<method name = "new">
    <argument name = "queue" type = "amq_queue_t *">Parent queue</argument>
    //
    self->queue            = queue;
    self->consumer_list    = amq_consumer_by_queue_new ();
    self->content_list     = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <local>
    amq_content_basic_t
        *content;                       //  Content object reference
    </local>
    //
    s_free_consumer_queue (self->consumer_list);
    while ((content = (amq_content_basic_t *) ipr_looseref_pop (self->content_list)))
        amq_content_basic_unlink (&content);
</method>

<method name = "free">
    amq_consumer_by_queue_destroy (&self->consumer_list);
    ipr_looseref_list_destroy (&self->content_list);
</method>

<method name = "stop" template = "function">
    <footer>
    s_free_consumer_queue (self->consumer_list);
    </footer>
</method>

<method name = "consume" template = "function">
    <doc>
    Attach consumer to appropriate queue consumer list.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    //
    amq_consumer_by_queue_queue (self->consumer_list, consumer);
</method>

<method name = "cancel" template = "function">
    <doc>
    Cancel consumer.  This method synchronises with the server_channel
    cancel method so each party handles their consumer lists separately.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    //  Consumer must have been removed from its per-channel list
    assert (consumer->by_channel_next == consumer);
    if (consumer->pending_content) {
        content = amq_content_basic_link (consumer->pending_content);
        if (content) {
            if (amq_server_config_debug_queue (amq_server_config))
                smt_log_print (amq_broker->debug_log,
                    "Q: requeue  queue=%s message=%s **for redelivery**", self->queue->name, content->message_id);
            ipr_looseref_push (self->content_list, content);
            self_dispatch (self);
            amq_consumer_content_release (consumer);
        }
    }
    amq_consumer_destroy (&consumer);
</method>

<method name = "consumer ack" template = "function">
    <doc>
    Acknowledge messages on consumer, if any.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer to ack</argument>
    <local>
    amq_content_basic_t
        *content;
    </local>
    //
    content = amq_content_basic_link (consumer->pending_content);
    if (content) {
        if (amq_server_config_debug_queue (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "Q: ack mesg queue=%s message=%s", self->queue->name, content->message_id);
        amq_consumer_content_release (consumer);
        amq_content_basic_unlink (&content);
        self_dispatch (self);
    }
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
    icl_shortstr_t
        client_identifier;
    amq_consumer_t
        *consumer;
    Bool
        have_active_consumers,
        rejected;
    amq_queue_t
        *queue;                         //  Parent queue object
    </local>
    //
    queue = self->queue;
    if (amq_server_config_debug_queue (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "Q: publish  queue=%s message=%s", queue->name, content->message_id);

    queue->contents_in++;
    queue->traffic_in += content->body_size;

    //  Grab a link to the queue's parent connection (if any) and client
    //  identification so that we can log it or kill the client if needed
    icl_shortstr_cpy (client_identifier, "");
    connection = amq_server_connection_link (queue->connection);
    if (connection) {
        icl_shortstr_fmt (client_identifier, "(%s, %s, %s, %s)",
            connection->client_address,
            connection->client_product,
            connection->client_version,
            connection->client_instance);
        amq_server_connection_unlink (&connection);
    }

    //  Check warning limit
    queue_size = ipr_looseref_list_count (self->content_list);
    if (queue->warn_limit && queue_size >= queue->warn_limit && !queue->warned) {
        smt_log_print (amq_broker->alert_log,
            "W: queue=%s hit %d messages: no action %s",
            queue->name, queue_size, client_identifier);
        queue->warned = TRUE;
    }
    //  Check just one of drop/trim/kill
    if (queue->drop_limit && queue_size >= queue->drop_limit) {
        if (!queue->dropped) {
            smt_log_print (amq_broker->alert_log,
                "W: queue=%s hit %d messages: dropping new messages %s",
                queue->name, queue_size, client_identifier);
            queue->dropped = TRUE;
        }
        queue->drop_count++;
        content = NULL;
    }
    else
    if (queue->trim_limit && queue_size >= queue->trim_limit) {
        amq_content_basic_t
            *oldest;                    //  Oldest content to trim
        if (!queue->trimmed) {
            smt_log_print (amq_broker->alert_log,
                "W: queue=%s hit %d messages: trimming old messages %s",
                queue->name, queue_size, client_identifier);
            queue->trimmed = TRUE;
        }
        oldest = (amq_content_basic_t *) ipr_looseref_pop (self->content_list);
        amq_content_basic_unlink (&oldest);
        queue->drop_count++;
    }
    else
    if (queue->kill_limit && queue_size >= queue->kill_limit) {
        smt_log_print (amq_broker->alert_log,
                "E: queue=%s hit %d messages: KILLING QUEUE %s",
                queue->name, queue_size, client_identifier);
        if (queue->exclusive && connection)
            amq_server_connection_error (connection,
                ASL_RESOURCE_ERROR, "Queue overflow, connection killed",
                AMQ_SERVER_BASIC, AMQ_SERVER_BASIC_PUBLISH);
        else
            amq_queue_self_destruct (queue);
    }
    else
    if (queue_size == 0) {
        //  Reset warning flags if queue becomes empty
        queue->warned  = FALSE;
        queue->dropped = FALSE;
        queue->trimmed = FALSE;
    }
    amq_server_connection_unlink (&connection);

    if (content) {
        //  If immediate, and no consumers, return the message
        rejected = FALSE;
        if (immediate) {
            //  Check if there are any active consumers
            have_active_consumers = FALSE;
            consumer = amq_consumer_by_queue_first (self->consumer_list);
            while (consumer) {
                if (!consumer->paused && !consumer->pending_content) {
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
                        queue->name, content->message_id);

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
        //  Consumer is available if it's not paused, nor has pending content
        if (!consumer->paused && !consumer->pending_content) {
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
                    //  Hold content if consumer is acknowledged
                    //  Queue must be shared queue
                    if (!self->queue->exclusive && !consumer->no_ack
                    &&  !amq_server_config_no_ack (amq_server_config))
                        amq_consumer_content_hold (consumer, content);

                    self->delivery_tag++;
                    amq_server_agent_basic_deliver (
                        connection->thread,
                        channel->number,
                        content,
                        consumer->tag,
                        self->delivery_tag,
                        content->redelivered,
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

<method name = "consumer count" template = "function">
    <doc>
    Return number of consumers for queue.
    </doc>
    //
    rc = amq_consumer_by_queue_count (self->consumer_list);
</method>

<method name = "message count" template = "function">
    <doc>
    Returns number of messages on queue.
    </doc>
    //
    rc = ipr_looseref_list_count (self->content_list);
</method>

<private name = "header">
#define CONSUMER_FOUND  0
#define CONSUMER_NONE   1
#define CONSUMER_BUSY   2

static int
    s_get_next_consumer ($(selftype) *self, char *producer_id, amq_consumer_t **consumer_p);
static void
    s_free_consumer_queue (amq_consumer_by_queue_t *queue);
</private>

<private name = "footer">
//  Find next consumer for queue and message
//  - producer_id is used for local consumers,
//  Returns CONSUMER_FOUND if a valid consumer is found
//  Returns CONSUMER_NONE if no valid consumers are found
//  Returns CONSUMER_BUSY if there are busy/paused consumers

static int
s_get_next_consumer (
    $(selftype) *self,
    char *producer_id,
    amq_consumer_t **consumer_p)
{
    amq_consumer_t
        *consumer;
    int
        rc = CONSUMER_NONE;
    amq_server_connection_t
        *connection;
    amq_server_channel_t
        *channel;
    Bool
        channel_active;

    //  We expect to process the first consumer on the active list
    consumer = amq_consumer_by_queue_first (self->consumer_list);
    while (consumer) {
        channel_active = FALSE;
        channel = amq_server_channel_link (consumer->channel);
        if (channel) {
            connection = amq_server_connection_link (channel->connection);
            if (connection)
                channel_active = channel->active;
        }
        else
            connection = NULL;

        if (channel_active) {
            if (icl_atomic_get32 ((volatile qbyte *) &channel->credit) < 1)
                rc = CONSUMER_BUSY;     //  Skip this consumer if busy
            else
            if (consumer->no_local) {
                //  If the consumer is an application then we can compare the
                //  content producer_id with the connection id of the consumer.
                if (strneq (connection->id, producer_id))
                    rc = CONSUMER_FOUND; //  We have our consumer
            }
            else
                rc = CONSUMER_FOUND;    //  We have our consumer
        }
        else
            rc = CONSUMER_BUSY;         //  Skip this consumer

        amq_server_connection_unlink (&connection);
        amq_server_channel_unlink (&channel);
        if (rc == CONSUMER_FOUND)
            break;                      //  Return this consumer
        else
            consumer = amq_consumer_by_queue_next (&consumer);
    }
    *consumer_p = consumer;
    return (rc);
}

static void
s_free_consumer_queue (amq_consumer_by_queue_t *queue)
{
    amq_consumer_t
        *consumer;
    amq_server_channel_t
        *channel;

    if (queue) {
        while ((consumer = amq_consumer_by_queue_pop (queue))) {
            channel = amq_server_channel_link (consumer->channel);
            if (channel) {
                amq_server_channel_cancel (channel, consumer->tag, FALSE, TRUE);
                amq_server_channel_unlink (&channel);
            }
            amq_consumer_destroy (&consumer);
        }
    }
}
</private>

<method name = "selftest" />

</class>
