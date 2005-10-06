<?xml?>
<class
    name      = "amq_queue_base"
    comment   = "Queue manager base class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
This class provides a template for content queue classes and
especially for consumer registration and cancellation, which is
independent of the queue content type.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<public name = "include">
#include "amq_server_classes.h"
</public>

<context>
    amq_queue_t
        *queue;                         //  Parent queue
    amq_consumer_by_queue_t
        *active_consumers,              //  Active consumers
        *paused_consumers;              //  Paused consumers

    //  Statistics
    size_t
        accept_count,                   //  Number of messages accepted
        dispatch_count;                 //  Number of messages dispatched
</context>

<method name = "new">
    <argument name = "queue" type = "amq_queue_t *">Parent queue</argument>
    //
    self->queue            = queue;
    self->active_consumers = amq_consumer_by_queue_new ();
    self->paused_consumers = amq_consumer_by_queue_new ();
</method>

<method name = "destroy">
    s_free_consumer_queue (&self->active_consumers);
    s_free_consumer_queue (&self->paused_consumers);
</method>

<method name = "consume" template = "function">
    <doc>
    Attach consumer to appropriate queue consumer list.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    <argument name = "active"   type = "Bool">Create active consumer?</argument>
    //
    if (active)
        amq_consumer_by_queue_queue (self->active_consumers, consumer);
    else
        amq_consumer_by_queue_queue (self->paused_consumers, consumer);
</method>

<method name = "cancel" template = "function">
    <doc>
    Cancel consumer.  This method synchronises with the server_channel
    cancel method so each party handles their consumer lists separately.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    //
    //  Consumer must have been removed from its per-channel list
    assert (consumer->by_channel_next == consumer);
    amq_consumer_destroy (&consumer);
</method>

<method name = "flow" template = "function">
    <doc>
    Pause or restart consumer.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer reference</argument>
    <argument name = "active"   type = "Bool">Active consumer?</argument>
    //
    amq_consumer_by_queue_remove (consumer);
    if (active) {
        amq_consumer_by_queue_queue (self->active_consumers, consumer);
        amq_queue_pre_dispatch (self->queue);
    }
    else
        amq_consumer_by_queue_queue (self->paused_consumers, consumer);
</method>

<private name = "header">
static amq_consumer_t *
    s_get_next_consumer ($(selftype) *self, qbyte producer_id);
static void
    s_free_consumer_queue (amq_consumer_by_queue_t **queue);
</private>

<private name = "footer">
//  Return next consumer for queue, NULL if there are none,
//  given publisher_id of current content to dispatch

static amq_consumer_t *
s_get_next_consumer ($(selftype) *self, qbyte producer_id)
{
    amq_consumer_t
        *consumer;

    //  We expect to process the first consumer on the active list
    consumer = amq_consumer_by_queue_first (self->active_consumers);
    while (consumer) {
        if (amq_server_channel_alive (consumer->channel)
        &&  consumer->channel->active
        && (consumer->no_local == FALSE
        ||  consumer->channel->connection->context_id != producer_id))
            break;                      //  We have our consumer
        else
            consumer = amq_consumer_by_queue_next (&consumer);
    }
    return (consumer);
}

static void
s_free_consumer_queue (amq_consumer_by_queue_t **queue)
{
    amq_consumer_t
        *consumer,
        *consumer_ref;

    while ((consumer = amq_consumer_by_queue_pop (*queue))) {
        if (amq_server_channel_cancel (consumer->channel, consumer->tag, FALSE)) {
            consumer_ref = consumer;
            amq_consumer_unlink  (&consumer_ref);
            amq_consumer_destroy (&consumer);
        }
        else
            amq_consumer_unlink (&consumer);
    }
    amq_consumer_by_queue_destroy (queue);
}
</private>

<method name = "selftest" />

</class>
