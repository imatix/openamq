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

<private>
#define TRACE_DISPATCH                  /*  Trace dispatching progress?      */
#undef  TRACE_DISPATCH
</private>

<context>
    /*  References to parent objects                                         */
    amq_consumer_t
        *consumer;                      /*  Parent consumer                  */
    amq_channel_t
        *channel;                       /*  Parent channel                   */
    amq_handle_t
        *handle;                        /*  Parent handle                    */
    amq_mesgq_t
        *mesgq;                         /*  Parent message queue             */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_smessage_t
        *message;                       /*  Message dispatched               */
    qbyte
        mesgq_id;                       /*  Message id in mesgq              */
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
    self->mesgq       = consumer->mesgq;
    self->db          = consumer->db;

    /*  Initialise other properties                                          */
    self->message     = message;        /*  We now 0wn this message          */
    self->mesgq_id    = self->mesgq->item_id;
    self->message_nbr = ++(self->channel->message_nbr);

    amq_dispatch_list_queue (self->channel->dispatch_list, self);

    /*  Dispatched message decrements message windows                        */
    assert (self->mesgq->window);
    assert (self->consumer->window);
    self->mesgq->window--;
    self->consumer->window--;
#   ifdef TRACE_DISPATCH
    coprintf ("$(selfname) new: mesgq:%d consumer:%d",
        self->mesgq->window, self->consumer->window);
#   endif
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
        /*  Message queue and consumer can accept a new message              */
        if (self->consumer->window < self->consumer->prefetch) {
            self->mesgq->window++;
            self->consumer->window++;
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) ack: mesgq:%d consumer:%d",
                self->mesgq->window, self->consumer->window);
#           endif
        }
        amq_mesgq_dispatch (self->mesgq);

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
    to the mesgq's memory list.  Persistent messages are updated on disk so
    that their 'client id' field is zero (meaning, non-dispatched).
    </doc>

    if (self->mesgq_id == 0) {
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) unget: non-persistent message %d", self->message_nbr);
#       endif
        /*  Push back non-persistent message                                 */
        /*    - update window AFTER so it won't bounce to same consumer      */
        amq_mesgq_accept (self->mesgq, NULL, self->message, NULL);
        self->message = NULL;           /*  Passed to mesgq_accept           */
    }
    else {
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) unget: persistent message %d", self->message_nbr);
#       endif
        /*  Ensure message is no longer assigned to this client              */
        self->mesgq->item_id = self->mesgq_id;
        amq_mesgq_fetch (self->mesgq, IPR_QUEUE_EQ);
        if (self->mesgq->item_client_id) {
            self->mesgq->item_client_id = 0;
            amq_mesgq_update (self->mesgq, NULL);
        }
        /*  Reset mesgq properties to cover this message                     */
        self->mesgq->disk_queue_size++;
        if (self->mesgq->last_id >= self->mesgq->item_id)
            self->mesgq->last_id  = self->mesgq->item_id - 1;
    }
    /*  After ungetting we can dispatch the mesgq again; we update the
        window after dispatching so that this message won't go back to
        the same client.
     */
    amq_mesgq_dispatch (self->mesgq);
    if (self->consumer->window < self->consumer->prefetch) {
        self->mesgq->window++;
        self->consumer->window++;
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) unget: mesgq:%d consumer:%d",
            self->mesgq->window, self->consumer->window);
#       endif
    }
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
        if (self->mesgq_id) {
            /*  Purge from persistent mesgq if necessary                     */
            self->mesgq->item_id = self->mesgq_id;
            self->mesgq->disk_queue_size--;
            amq_mesgq_delete (self->mesgq, txn);
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) delete: mesgq:%d pending=%d",
                self->mesgq->window, self->mesgq->disk_queue_size);
#           endif
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
        if (self->mesgq_id) {
            self->mesgq->item_id = self->mesgq_id;
            amq_mesgq_fetch (self->mesgq, IPR_QUEUE_EQ);
            self->mesgq->item_client_id = 0;
            amq_mesgq_update (self->mesgq, txn);
        }
        if (self->consumer->window > 0) {
            self->mesgq->window--;
            self->consumer->window--;
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) rollback: mesgq:%d consumer:%d",
                self->mesgq->window, self->consumer->window);
#           endif
        }
    }
</method>

<method name = "selftest">
</method>

</class>
