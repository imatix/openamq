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

<import class = "amq_server_classes" />

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
    <local>
    amq_consumer_t
        *consumer;                      //  Consumer reference
    </local>
    //  We destroy consumers by asking the respective channels to do the work
    while ((consumer = amq_consumer_by_queue_pop (self->active_consumers))) {
        if (amq_server_channel_alive (consumer->channel)) {
            amq_server_channel_cancel (consumer->channel, consumer->tag, FALSE);
            amq_consumer_unlink (&consumer);
        }
        else
            amq_consumer_destroy (&consumer);
    }
    while ((consumer = amq_consumer_by_queue_pop (self->paused_consumers))) {
        if (amq_server_channel_alive (consumer->channel)) {
            amq_server_channel_cancel (consumer->channel, consumer->tag, FALSE);
            amq_consumer_unlink (&consumer);
        }
        else
            amq_consumer_destroy (&consumer);
    }
    amq_consumer_by_queue_destroy (&self->active_consumers);
    amq_consumer_by_queue_destroy (&self->paused_consumers);
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
        ||  consumer->channel->connection->context_id == producer_id))
            break;                      //  We have our consumer
        else
            consumer = amq_consumer_by_queue_next (&consumer);
    }
    return (consumer);
}
</private>

<method name = "selftest" />

</class>
