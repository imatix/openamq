<?xml?>
<class
    name      = "amq_dispatch"
    comment   = "Dispatched message class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "ipr_list_item" />
<option name = "nullify" value = "1" />

<import class = "amq_handle"  />
<import class = "ipr_classes" />

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
        closed;                         /*  Finished processing?             */
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
    ASSERT (self->queue->window);
    ASSERT (self->consumer->window);
    self->queue->window--;
    self->consumer->window--;
</method>

<method name = "destroy">
    amq_smessage_destroy (&self->message);
</method>

<method name = "ack" template = "function" return = "next">
    <doc>
    Acknowledge a specific message.  Returns the next message on the
    dispatch list.
    </doc>
    <declare name = "next" type = "amq_dispatch_t *" />

    /*  Queue and consumer can accept a new message                          */
    self->queue->window++;
    self->consumer->window++;
    amq_queue_dispatch (self->queue);
    next = amq_dispatch_list_next (self->channel->dispatched, self);

    if (self->queue_id) {
        /*  Purge from persistent queue if necessary                         */
        self->queue->item_id = self->queue_id;
        amq_queue_delete (self->queue, self->channel->txn);
        self->queue->outstanding--;
    }
    if (self->channel->transacted)
        self->closed = TRUE;            /*  Keep, to allow rollback          */
    else
        amq_dispatch_destroy (&self);
</method>

<method name = "unget" template = "function">
    <doc>
    Unget a specific message.
    </doc>

    if (self->queue_id == 0) {
        /*  Push back non-persistent message                                 */
        /*    - update window AFTER so it won't bounce to same consumer      */
        amq_queue_accept   (self->queue, self->channel, self->message);
        amq_queue_dispatch (self->queue);
        self->queue->window++;
        self->consumer->window++;
    }
    else {
        self->queue->item_id = self->queue_id;
        amq_queue_fetch (self->queue, IPR_QUEUE_EQ);

        self->queue->item_client_id = 0;
        amq_queue_update (self->queue, self->channel->txn);

        /*  Reset last message id to cover this message                      */
        if (self->queue->last_id > self->queue->item_id)
            self->queue->last_id = self->queue->item_id - 1;

        /*  Queue and consumer can accept a new message                      */
        self->queue->window++;
        self->consumer->window++;
        amq_queue_dispatch (self->queue);
    }
    amq_dispatch_destroy (&self);
</method>

<method name = "selftest">
</method>

</class>
