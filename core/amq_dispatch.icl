<?xml?>
<class
    name      = "amq_dispatch"
    comment   = "Dispatched message class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

<inherit class = "ipr_list_item" />

<import class = "ipr_classes" />
<import class = "amq_global"  />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    /*  References to parent objects                                         */
    amq_consumer_t
        *consumer;                      /*  Parent consumer                  */
    amq_channel_t
        *channel;                       /*  Parent channel                   */
    amq_handle_t
        *handle;                        /*  Parent handle for dispatch       */
    amq_queue_t
        *queue;                         /*  Parent queue for dispatch        */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_smessage_t
        *message;                       /*  Message dispatched               */
    qbyte
        queue_id;                       /*  Queue id                         */
    qbyte
        message_nbr;                    /*  Message number                   */
    Bool
        acknowledged;                   /*  Finished processing?             */
</context>

<method name = "new">
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    <argument name = "message"  type = "amq_smessage_t *">Message object</argument>

    /*  De-normalise from parent object, for simplicity of use               */
    self->consumer    = consumer;
    self->handle      = consumer->handle;
    self->channel     = consumer->channel;
    self->queue       = consumer->queue;
    self->db          = consumer->db;

    /*  Initialise other properties                                          */
    self->message     = message;        /*  We now 0wn this message          */
    self->queue_id    = self->queue->item_id;
    self->message_nbr = ++(self->channel->message_nbr);

    amq_dispatch_list_queue (self->channel->dispatched, self);

    /*  Dispatched message decrements message windows                        */
    assert (self->queue->window);
    assert (self->consumer->window);
    self->queue->window--;
    self->consumer->window--;
    coprintf ("$(selfname) new: queue:%d consumer:%d", self->queue->window, self->consumer->window);
</method>

<method name = "destroy">
    amq_smessage_destroy (&self->message);
</method>

<method name = "ack" return = "next">
    <doc>
    Acknowledges the specified message, if not already acknowledged.
    Persistent messages are deleted from the disk file; non-persistent
    messages are marked as 'acknowledged'.
    Returns the next message on the dispatch list.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "next" type = "amq_dispatch_t *">Next message in list</declare>

    next = amq_dispatch_list_next (self->channel->dispatched, self);
    if (!self->acknowledged) {
        /*  Queue and consumer can accept a new message                      */
        if (self->consumer->window < self->consumer->prefetch) {
            self->queue->window++;
            self->consumer->window++;
            coprintf ("$(selfname) ack: queue:%d consumer:%d", self->queue->window, self->consumer->window);
        }
        amq_queue_dispatch (self->queue);
        next = amq_dispatch_list_next (self->channel->dispatched, self);

        if (self->queue_id) {
            /*  Purge from persistent queue if necessary                     */
            amq_smessage_delete (self->message, self->channel->txn);
            self->queue->disk_queue_size--;
        }
        if (self->channel->transacted)
            /*  Keep dispatch object in memory so we can rollback the ack    */
            self->acknowledged = TRUE;
        else
            self_destroy (&self);
    }
</method>

<method name = "commit" return = "next">
    <doc>
    Commit the specified message, if it has been acknowledged by the client.
    This happens when the client is working in transacted mode.
    Returns the next message on the dispatch list.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "next" type = "amq_dispatch_t *">Next message in list</declare>

    next = amq_dispatch_list_next (self->channel->dispatched, self);
    if (self->acknowledged)
        self_destroy (&self);
</method>

<method name = "rollback" return = "next">
    <doc>
    Rollback the specified mesage if is has been acknowledged by the client.
    This happens when the client is working in transacted mode.
    Returns the next message on the dispatch list.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "next" type = "amq_dispatch_t *">Next message in list</declare>

    next = amq_dispatch_list_next (self->channel->dispatched, self);
    if (self->acknowledged) {
        self->acknowledged = FALSE;
        if (self->consumer->window > 0) {
            self->queue->window--;
            self->consumer->window--;
            coprintf ("$(selfname) rollback: queue:%d consumer:%d", self->queue->window, self->consumer->window);
        }
    }
</method>

<method name = "unget" template = "function">
    <doc>
    Ungets the specified message.  Non-persistent messages are pushed back
    to the queue's memory list.  Persistent messages are updated on disk so
    that their 'client id' field is zero (meaning, non-dispatched).
    </doc>
    if (self->queue_id == 0) {
        coprintf ("Unget non-persistent message %d", self->message_nbr);
        /*  Push back non-persistent message                                 */
        /*    - update window AFTER so it won't bounce to same consumer      */
        amq_queue_accept (self->queue, self->channel, self->message, NULL);
        self->message = NULL;           /*  Passed to queue_accept           */
    }
    else {
        coprintf ("Unget persistent message %d", self->message_nbr);
        /*  Ensure message is no longer assigned to this client              */
        self->queue->item_id = self->queue_id;
        amq_queue_fetch (self->queue, IPR_QUEUE_EQ);
        if (self->queue->item_client_id) {
            self->queue->item_client_id = 0;
            amq_queue_update (self->queue, self->channel->txn);
        }
        /*  Reset queue properties to cover this message                     */
        self->queue->disk_queue_size++;
        if (self->queue->last_id > self->queue->item_id)
            self->queue->last_id = self->queue->item_id - 1;
    }
    /*  After ungetting we can dispatch the queue again; we update the
        window after dispatching so that this message won't go back to
        the same client.
     */
    amq_queue_dispatch (self->queue);
    if (self->consumer->window < self->consumer->prefetch) {
        self->queue->window++;
        self->consumer->window++;
        coprintf ("$(selfname) unget: queue:%d consumer:%d", self->queue->window, self->consumer->window);
    }
</method>

<method name = "selftest">
</method>

</class>
