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
        *handle;                        /*  Parent handle for dispatch       */
    amq_dest_t 
        *dest;                          /*  Parent dest for dispatch         */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_smessage_t
        *message;                       /*  Message dispatched               */
    qbyte
        dest_id;                        /*  Dest id                          */
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
    self->dest        = consumer->dest;
    self->db          = consumer->db;

    /*  Initialise other properties                                          */
    self->message     = message;        /*  We now 0wn this message          */
    self->dest_id     = self->dest->item_id;
    self->message_nbr = ++(self->channel->message_nbr);

    amq_dispatch_list_queue (self->channel->dispatch_list, self);

    /*  Dispatched message decrements message windows                        */
    assert (self->dest->window);
    assert (self->consumer->window);
    self->dest->window--;
    self->consumer->window--;
#   ifdef TRACE_DISPATCH
    coprintf ("$(selfname) new: dest:%d consumer:%d", self->dest->window, self->consumer->window);
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
        /*  Dest and consumer can accept a new message                       */
        if (self->consumer->window < self->consumer->prefetch) {
            self->dest->window++;
            self->consumer->window++;
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) ack: dest:%d consumer:%d",
                self->dest->window, self->consumer->window);
#           endif
        }
        amq_dest_dispatch (self->dest);

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
    to the dest's memory list.  Persistent messages are updated on disk so
    that their 'client id' field is zero (meaning, non-dispatched).
    </doc>

    if (self->dest_id == 0) {
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) unget: non-persistent message %d", self->message_nbr);
#       endif
        /*  Push back non-persistent message                                 */
        /*    - update window AFTER so it won't bounce to same consumer      */
        amq_dest_accept (self->dest, NULL, self->message, NULL);
        self->message = NULL;           /*  Passed to dest_accept            */
    }
    else {
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) unget: persistent message %d", self->message_nbr);
#       endif
        /*  Ensure message is no longer assigned to this client              */
        self->dest->item_id = self->dest_id;
        amq_dest_fetch (self->dest, IPR_QUEUE_EQ);
        if (self->dest->item_client_id) {
            self->dest->item_client_id = 0;
            amq_dest_update (self->dest, NULL);
        }
        /*  Reset dest properties to cover this message                      */
        self->dest->disk_queue_size++;
        if (self->dest->last_id >= self->dest->item_id)
            self->dest->last_id  = self->dest->item_id - 1;
    }
    /*  After ungetting we can dispatch the dest again; we update the
        window after dispatching so that this message won't go back to
        the same client.
     */
    amq_dest_dispatch (self->dest);
    if (self->consumer->window < self->consumer->prefetch) {
        self->dest->window++;
        self->consumer->window++;
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) unget: dest:%d consumer:%d", self->dest->window, self->consumer->window);
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
        if (self->dest_id) {
            /*  Purge from persistent dest if necessary                      */
            self->dest->item_id = self->dest_id;
            self->dest->disk_queue_size--;
            amq_dest_delete (self->dest, txn);
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) delete: dest:%d pending=%d",
                self->dest->window, self->dest->disk_queue_size);
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
        if (self->dest_id) {
            self->dest->item_id = self->dest_id;
            amq_dest_fetch (self->dest, IPR_QUEUE_EQ);
            self->dest->item_client_id = 0;
            amq_dest_update (self->dest, txn);
        }
        if (self->consumer->window > 0) {
            self->dest->window--;
            self->consumer->window--;
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) rollback: dest:%d consumer:%d",
                self->dest->window, self->consumer->window);
#           endif
        }
    }
</method>

<method name = "selftest">
</method>

</class>
