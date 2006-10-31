<?xml?>
<class
    name      = "amq_queue_base"
    comment   = "Queue manager base class"
    version   = "1.0"
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
        *consumer_list;                 //  List of consumers from the queue
    ipr_looseref_list_t
        *content_list;                  //  List of message contents

    //  Statistics
    size_t
        accept_count,                   //  Number of messages accepted
        dispatch_count;                 //  Number of messages dispatched
</context>

<method name = "new">
    <argument name = "queue" type = "amq_queue_t *">Parent queue</argument>
    //
    self->queue            = queue;
    self->consumer_list    = amq_consumer_by_queue_new ();
    self->content_list     = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <action>
    s_free_consumer_queue (self->consumer_list);
    </action>
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
    //
    //  Consumer must have been removed from its per-channel list
    assert (consumer->by_channel_next == consumer);
    amq_consumer_destroy (&consumer);
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
//  Returns CONSUMER_BUSY if there are busy consumers

static int
s_get_next_consumer (
    $(selftype) *self,
    char *producer_id,
    amq_consumer_t **consumer_p)
{
    amq_consumer_t
        *consumer;
    smt_thread_t
        *thread;
    int
        rc = CONSUMER_NONE;
    amq_server_connection_t
        *connection;
    amq_server_channel_t
        *channel;
    Bool
        channel_active,
        channel_busy;

    //  We expect to process the first consumer on the active list
    consumer = amq_consumer_by_queue_first (self->consumer_list);
    while (consumer) {

        //  Skip paused consumers
        if (consumer->paused) {
            consumer = amq_consumer_by_queue_next (&consumer);
            continue;
        }

        channel_active = FALSE;
        channel_busy   = FALSE;
        channel = amq_server_channel_link (consumer->channel);
        if (channel) {
            connection = amq_server_connection_link (channel->connection);
            if (connection) {
                thread = smt_thread_link (channel->thread);
                if (thread) {
                    channel_active = channel->active;
                    //  ML: We shouldn't need a lock here since count is 
                    //  always updated atomically, so at worst we get some
                    //  past value
                    channel_busy = (thread->reply_queue->count > 100);
                }
                smt_thread_unlink (&thread);
            }
        }
        else
            connection = NULL;
            
        if (!channel_active)
            ;                           //  Skip this consumer
        else
        if (channel_busy)
            rc = CONSUMER_BUSY;         //  Unless we have better news
        else
        if (consumer->no_local == FALSE)
            rc = CONSUMER_FOUND;        //  We have our consumer
        else
        if (strneq (connection->id, producer_id)) {
            //  If the consumer is an application then we can compare the
            //  content producer_id with the connection id of the consumer.
            rc = CONSUMER_FOUND;        //  We have our consumer
        }
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

    if (queue) {
        while ((consumer = amq_consumer_by_queue_pop (queue))) {
            amq_server_channel_cancel (consumer->channel, consumer->tag, FALSE, TRUE);
            amq_consumer_destroy (&consumer);
        }
    }
}
</private>

<method name = "selftest" />

</class>
