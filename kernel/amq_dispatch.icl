<?xml?>
<class
    name      = "amq_dispatch"
    comment   = "Dispatched message class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

<inherit class = "ipr_list_item" />

<import class = "amq_global" />

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
        *handle;                        /*  Parent handle                    */
    amq_queue_t
        *queue;                         /*  Parent message queue             */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_smessage_t
        *message;                       /*  Message dispatched               */
    qbyte
        queue_id;                       /*  Message id in queue              */
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
    self->channel     = consumer->handle->channel;
    self->queue       = consumer->queue;
    self->db          = consumer->handle->channel->db;

    /*  Initialise other properties                                          */
    self->message     = message;        /*  We now 0wn this message          */
    self->queue_id    = self->queue->item_id;
    self->message_nbr = ++(self->channel->message_nbr);

    amq_smessage_link         (self->message);
    amq_dispatch_list_queue   (self->channel->dispatch_list, self);
    amq_consumer_window_close (self->consumer);
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

    next = amq_dispatch_list_next (self->channel->dispatch_list, self);
    if (!self->acknowledged) {
        amq_consumer_window_open (self->consumer);
        amq_queue_dispatch (self->queue);

        /*  Now commit the acknowledgement if not transacted                 */
        self->acknowledged = TRUE;
        if (!self->channel->transacted) {
            self_commit (self, NULL);
            self_purge  (self);
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
        /*  Push back non-persistent message                                 */
        /*    - update window AFTER so it won't bounce to same consumer      */
        amq_queue_accept (self->queue, NULL, self->message, NULL);
        amq_smessage_destroy (&self->message);
    }
    else {
        /*  Ensure message is no longer assigned to this client              */
        self->queue->item_id = self->queue_id;
        if (amq_queue_fetch (self->queue, IPR_QUEUE_EQ) == 0) {
            if (self->queue->item_client_id) {
                self->queue->item_client_id = 0;
                amq_queue_update (self->queue, NULL);
            }
            /*  Reset queue properties to cover this message                     */
            self->queue->disk_queue_size++;
            if (self->queue->last_id >= self->queue->item_id)
                self->queue->last_id  = self->queue->item_id - 1;
        }
        else
            coprintf ("E: message not found for update (dest=%s)", self->queue->dest->key);
    }
    /*  After ungetting we can dispatch the queue again; we update the
        window after dispatching so that this message won't go back to
        the same client.
     */
    amq_queue_dispatch (self->queue);
    amq_consumer_window_open (self->consumer);
</method>

<method name = "commit" return = "next">
    <doc>
    Commit the specified message, if it has been acknowledged by the client.
    This happens when the client is working in transacted mode.
    Returns the next message on the dispatch list.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "txn" type = "ipr_db_txn_t *">Current transaction</argument>
    <declare name = "next" type = "amq_dispatch_t *">Next message in list</declare>

    next = amq_dispatch_list_next (self->channel->dispatch_list, self);
    if (self->acknowledged) {
        if (self->queue_id) {
            /*  Purge from persistent queue if necessary                     */
            amq_queue_delete_message (self->queue, txn);
            self->queue->item_id = self->queue_id;
            self->queue->disk_queue_size--;
        }
    }
</method>

<method name = "purge" return = "next">
    <doc>
    Purge the specified message if it has been acknowledged by the client.
    Returns the next message on the dispatch list.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "next" type = "amq_dispatch_t *">Next message in list</declare>

    next = amq_dispatch_list_next (self->channel->dispatch_list, self);
    if (self->acknowledged) {
        /*  We can have multiple references to the same message, so we purge
            only when this is the last reference... hopefully there won't be
            dangling references somewhere else apart from dispatch objects.
         */
        if (self->message->links == 1)
            amq_smessage_purge (self->message);
        self_destroy (&self);
    }
</method>

<method name = "rollback" return = "next">
    <doc>
    Rollback the specified mesage if is has been acknowledged by the client.
    This happens when the client is working in transacted mode.
    Returns the next message on the dispatch list.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "txn" type = "ipr_db_txn_t *">Current transaction</argument>
    <declare name = "next" type = "amq_dispatch_t *">Next message in list</declare>

    next = amq_dispatch_list_next (self->channel->dispatch_list, self);
    if (self->acknowledged) {
        self->acknowledged = FALSE;
        if (self->queue_id) {
            self->queue->item_id = self->queue_id;
            if (amq_queue_fetch (self->queue, IPR_QUEUE_EQ) == 0) {
                self->queue->item_client_id = 0;
                amq_queue_update (self->queue, txn);
            }
            else
                coprintf ("E: message not found for update (dest=%s)", self->queue->dest->key);
        }
        amq_consumer_window_close (self->consumer);
    }
</method>

<method name = "selftest">
</method>

</class>
