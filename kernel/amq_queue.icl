<?xml?>
<class
    name      = "amq_queue"
    comment   = "Queue destination class"
    version   = "1.3"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class defines an AMQP queue destination, which holds messages and
distributes them to a set of consumers on a round-robin basis.  Each queue
is recorded in the db_dest table, so can be referenced by its id in this
table.  Queues are also optionally backed-up onto persistent storage if
the queue has persistent messages and/or is configured as persistent.
This class provides the methods needed to send messages to a queue and
dispatch pending messages to consumers.
</doc>

<inherit class = "amq_dest" />

<data>
    <field name = "sender id"   type = "longint" >Original sender</field>
    <field name = "client id"   type = "longint" >Owned by this client</field>
    <field name = "header size" type = "longint" >Saved header size</field>
    <field name = "body size"   type = "longint" >Message body size</field>
    <field name = "priority"    type = "octet"   >Priority, 0-9</field>
    <field name = "expiration"  type = "longint" >Expiry date/time</field>
    <field name = "mime type"   type = "shortstr">Content MIME type</field>
    <field name = "encoding"    type = "shortstr">Content encoding</field>
    <field name = "identifier"  type = "shortstr">Message identifier</field>
    <field name = "headers"     type = "longstr" >Application headers</field>
    <field name = "content"     type = "longstr" >First body fragment</field>
    <field name = "store size"  type = "longint" >Stored file size, or 0</field>
</data>

<private>
#include "amq_server_agent.h"
#define TRACE_DISPATCH                  /*  Trace dispatching progress?      */
#undef  TRACE_DISPATCH
</private>

<context>
    ipr_looseref_t
        *queue_ref;                     /*  Item in vhost queue list         */
    amq_consumer_list_t
        *consumers;                     /*  Consumers for this queue         */
    amq_smessage_list_t
        **message_list;                 /*  Pending non-persistent messages  */
    qbyte
        window;                         /*  Total window availability        */
    qbyte
        last_id;                        /*  Last dispatched message          */
    size_t
        nbr_consumers;                  /*  Number of consumers              */
    Bool
        dirty;                          /*  Queue needs dispatching          */
</context>

<method name = "new">
    <dismiss argument = "table" value = "vhost->queue_hash" />
    self->consumers = amq_consumer_list_new ();
    self->queue_ref = ipr_looseref_new (vhost->queue_refs, self);
    s_create_priority_lists (self);
</method>

<method name = "destroy">
    s_destroy_priority_lists (self);
    amq_consumer_list_destroy (&self->consumers);
    ipr_looseref_destroy      (&self->queue_ref);
</method>

<method name = "consume" return = "queue">
    <doc>
    Lookup and return queue, if any, for specified name. Concats the handle
    destination with the supplied suffix to give a full destination name.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    <argument name = "suffix"   type = "char *"          >Destination name suffix</argument>
    <declare  name = "queue"    type = "$(selftype) *" default = "NULL"/>

    /*  Look for queue                                                       */
    queue = self_full_search (
        consumer->vhost->queue_hash,
        consumer->handle->dest_name,
        suffix,
        AMQP_SERVICE_QUEUE);

   if (queue) {
        /*  Allowed as consumer if shared queue or owner of tempq            */
        if (queue->temporary == FALSE
        ||  queue->client_id == consumer->client_id) {
            if (queue->opt_max_consumers == 0
            ||  queue->nbr_consumers < queue->opt_max_consumers) {
                amq_consumer_list_queue (queue->consumers, consumer);
                queue->nbr_consumers++;
                queue->window += consumer->window;
            }
            else {
                amq_global_set_error (AMQP_NOT_ALLOWED, "Destination consumer limit reached");
                queue = NULL;
            }
        }
        else {
            amq_global_set_error (AMQP_ACCESS_REFUSED, "Not owner of temporary queue");
            queue = NULL;
        }
    }
    else
        amq_global_set_error (AMQP_NOT_FOUND, "No such destination defined");
</method>

<method name = "cancel" template = "function">
    <doc>
    Cancels a consumer on a specific queue, updating the queue as needed.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    self->nbr_consumers--;
    self->window -= consumer->window;
</method>

<method name = "accept" template = "function">
    <doc>
    Saves a message to a message queue. Transacted messages are held in memory
    pending a channel commit or roll back.  Non-transacted persistent messages
    are saved to the persistent message queue and n-t n-p messages are saved to
    the message queue priority lists.  The channel can be specified as NULL,
    which forces non-persistent messages to be saved to the message queue memory
    list. The caller should destroy the message after calling this method.
    </doc>
    <argument name = "channel" type = "amq_channel_t *" >Current channel</argument>
    <argument name = "message" type = "amq_smessage_t *">Message, if any</argument>
    <argument name = "txn"     type = "ipr_db_txn_t *"  >Transaction, if any</argument>
    <local>
    uint
        level;                          /*  Priority level                   */
    </local>

    assert (message);
    if (self->opt_persistent)
        message->persistent = TRUE;     /*  Force message to be persistent   */

    /*  First check that posting to this queue is currently allowed          */
    if (self->nbr_consumers < self->opt_min_consumers)
        amq_global_set_error (AMQP_NOT_ALLOWED, "Destination needs consumers but has none");
    else
    if (self->opt_max_messages > 0
    &&  self->disk_queue_size + self->memory_queue_size > self->opt_max_messages)
        amq_global_set_error (AMQP_NOT_ALLOWED, "Destination is filled to capacity");
    else
    if (self->opt_max_message_size > 0
    &&  message->body_size > self->opt_max_message_size)
        amq_global_set_error (AMQP_NOT_ALLOWED, "Message exceeds limits for destination");
    else

    if (channel && channel->transacted) {
        /*  Transacted messages are held per-channel                         */
        if (amq_txn_limit == 0 || channel->transact_count < amq_txn_limit) {
            message->queue = self;
            amq_smessage_list_queue (channel->transact_list, message);
            amq_smessage_link (message);
            channel->transact_count++;
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: queue transacted message, txn_count=%d",
                channel->transact_count);
#           endif
        }
        else
            amq_global_set_error (AMQP_RESOURCE_ERROR, "Transaction too large");
    }
    else {
        /*  In certain cases we force the message to be persistent           */
        level = s_priority_level (self, message);
        if (!message->persistent) {

            /*  If we have reached the limit for in-memory messages,
                force the message to disk unless it has the highest
                priority
             */
            if (self->opt_memory_queue_max > 0
            &&  self->memory_queue_size >= self->opt_memory_queue_max
            &&  message->priority &lt; AMQP_MAX_PRIORITIES)
                message->persistent = TRUE;

            /*  If there are already messages on disk, send all low
                priority messages to disk as well so they are delivered
                in order
              */
            else
            if (self->disk_queue_size > 0
            &&  level == 0)
                message->persistent = TRUE;
        }
        if (message->persistent) {
            /*  Persistent messages are saved on persistent queue storage    */
            self->disk_queue_size++;
            amq_smessage_save (message, self, txn);
    #       ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: save persistent message to storage");
    #       endif
        }
        else {
            /*  Non-persistent messages are held per message queue           */
            self->memory_queue_size++;
            amq_smessage_list_queue (self->message_list [level], message);
            amq_smessage_link (message);
    #       ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: save non-persistent message to queue memory");
    #       endif
        }
        self->dirty = TRUE;             /*  Message queue has new data       */

        /*  We move all dirty queues to the start of the vhost list so
            that dispatching can be rapid when the vhost has lots of them    */
        ipr_looseref_list_push (self->vhost->queue_refs, self->queue_ref);
    }
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatches all pending messages waiting on the specified message queue.
    </doc>
    <local>
    amq_smessage_t
        *message;
    amq_consumer_t
        *consumer;                      /*  Next consumer to process         */
    ipr_looseref_t
        *browser_ref;
    int
        finished;
    uint
        level;                          /*  Priority level                   */
    </local>

    /*  First, process any messages in memory: in the case that messages
        are held both in memory and disk, the ones in memory will be the
        oldest.
     */
    if (self->window && self->memory_queue_size) {
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: dispatching non-persistent window=%d pending=%d",
            self->window, self->memory_queue_size);
#       endif

        /*  Start with highest-priority messages                             */
        level = self->opt_priority_levels;
        while (level) {
            level--;
            message = amq_smessage_list_first (self->message_list [level]);
            while (self->window && message) {
                consumer = s_get_next_consumer (self);
                if (consumer) {
                    /*  Keep message alive after it's been dispatched            */
                    amq_smessage_link        (message);
                    amq_smessage_list_unlink (message);
                    self->memory_queue_size--;
                    self->item_id = 0;      /*  Non-persistent message           */
                    s_dispatch_message (consumer, message);

                    /*  Invalidate any browsers for this message                 */
                    browser_ref = ipr_looseref_list_first (message->browsers);
                    while (browser_ref) {
                        amq_browser_destroy ((amq_browser_t **) &browser_ref->object);
                        browser_ref = ipr_looseref_list_next (message->browsers, browser_ref);
                    }
                    /*  Get next message (is now first on queue)                 */
                    message = amq_smessage_list_first (self->message_list [level]);

                    /*  Work down to next priority level if needed               */
                    while (message == NULL && level > 0) {
                        level--;
                        message = amq_smessage_list_first (self->message_list [level]);
                    }
                }
                else
                    break;                  /*  No more consumers                */
            }
        }
    }
    /*  Now process any messages on disk                                     */
    if (self->window && self->disk_queue_size) {
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: dispatching persistent window=%d pending=%d",
            self->window, self->disk_queue_size);
#       endif

        /*  Get oldest candidate message to dispatch                         */
        self->item_id = self->last_id;
        finished = self_fetch (self, IPR_QUEUE_GT);
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: fetch last=%d rc=%d id=%d finished=%d",
            self->last_id, finished, self->item_id, finished);
#       endif

        while (self->window && !finished) {
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: message id=%d client=%d", self->item_id, self->item_client_id);
#           endif
            if (self->item_client_id == 0) {
                consumer = s_get_next_consumer (self);
                if (consumer) {
                    message = amq_smessage_new (consumer->handle);
                    amq_smessage_load  (message, self);
                    s_dispatch_message (consumer, message);
                    self->item_client_id = consumer->client_id;
                    self_update (self, NULL);
                }
                else
                    break;              /*  No more consumers                */
            }
            assert (self->item_id > self->last_id);
            self->last_id = self->item_id;
            finished = self_fetch (self, IPR_QUEUE_NEXT);
        }
        /*  Close active cursor if any                                       */
        ipr_db_cursor_close (self->db);
    }
    self->dirty = FALSE;                /*  queue has been dispatched        */
</method>

<method name = "query" template = "function">
    <doc>
    Loads a query set referring to every available message in the
    message queue.
    </doc>
    <argument name = "browser_set" type = "amq_browser_array_t *">Query set</argument>
    <local>
    /*  TODO
        implement maximum query size (0 = all)
     */
    amq_smessage_t
        *message;
    amq_browser_t
        *browser;
    qbyte
        set_index = 0;                  /*  Index into query set             */
    int
        finished;
    uint
        level;                          /*  Priority level                   */
    </local>

    /*  First, process any messages in memory: in the case that messages
        are held both in memory and disk, the ones in memory will be the
        oldest.
     */
    amq_browser_array_reset (browser_set);

    /*  Process highest priority messages first                              */
    level = self->opt_priority_levels;
    while (level) {
        level--;
        message = amq_smessage_list_first (self->message_list [level]);
        while (message) {
            /*  Track browser for message so we can invalidate it if/when
                we dispatch the message.                                     */
            browser = amq_browser_new (browser_set, set_index++, self, 0, message);
            ipr_looseref_new (message->browsers, browser);
            message = amq_smessage_list_next (self->message_list [level], message);
        }
    }
    /*  Now process any messages on disk                                     */
    if (self->disk_queue_size) {
        /*  Get oldest candidate message to dispatch                         */
        self->item_id = self->last_id;
        finished = self_fetch (self, IPR_QUEUE_GT);
        while (!finished) {
            if (self->item_client_id == 0)
                amq_browser_new (browser_set, set_index++, self, self->item_id, NULL);

            finished = self_fetch (self, IPR_QUEUE_NEXT);
        }
        /*  Close active cursor if any                                       */
        ipr_db_cursor_close (self->db);
    }
</method>

<method name = "browse" template = "function">
    <doc>
    Sends a single message to the user, where the message is specified
    by an amq_browse_t object.  If the message does not exist or has already
    been dispatched to another user, sends the user a 310 error.
    </doc>
    <argument name = "handle"  type = "amq_handle_t *" >Parent handle</argument>
    <argument name = "browser" type = "amq_browser_t *">Browser reference</argument>
    <local>
    amq_smessage_t
        *message = NULL;
    </local>
    if (browser->message) {
        message = browser->message;
        amq_smessage_link (message);    /*  Keep alive after sending         */
    }
    else
    if (browser->item_id) {
        self->item_id = browser->item_id;
        if (self_fetch (self, IPR_QUEUE_EQ) == 0) {
            message = amq_smessage_new (handle);
            amq_smessage_load (message, self);
        }
        /*  Close active cursor if any                                       */
        ipr_db_cursor_close (self->db);
    }
    if (message)
        amq_server_agent_handle_notify (
            handle->thread,
            (dbyte) handle->key,
            browser->index,
            FALSE, FALSE, FALSE,
            self->key,
            message,
            NULL);
    else
        rc = 1;                         /*  No message to dispatch           */
</method>

<private name = "header">
static void
    s_create_priority_lists ($(selftype) *self);
static void
    s_destroy_priority_lists ($(selftype) *self);
static int
    s_priority_level ($(selftype) *self, amq_smessage_t *message);
static amq_consumer_t *
    s_get_next_consumer ($(selftype) *self);
static void
    s_dispatch_message (amq_consumer_t *consumer, amq_smessage_t *message);
</private>

<private name = "footer">
/*  Create 1 to AMQP_MAX_PRIORITIES priority lists for dispatching           */

static void
s_create_priority_lists ($(selftype) *self)
{
    uint
        level;

    /*  Create one to 10 priority queues                                     */
    if (self->opt_priority_levels > AMQP_MAX_PRIORITIES)
        self->opt_priority_levels = AMQP_MAX_PRIORITIES;
    else
    if (self->opt_priority_levels == 0)
        self->opt_priority_levels = 1;

    self->message_list = icl_mem_alloc (
        self->opt_priority_levels * sizeof (amq_smessage_list_t *));

    for (level = 0; level < self->opt_priority_levels; level++)
        self->message_list [level] = amq_smessage_list_new ();
}

/*  Destroy the message queue's priority lists                               */

static void
s_destroy_priority_lists ($(selftype) *self)
{
    uint
        level;

    for (level = 0; level < self->opt_priority_levels; level++)
        amq_smessage_list_destroy (&self->message_list [level]);

    icl_mem_free (self->message_list);
}

/*  Calculate priority level for this message                                */

static int
s_priority_level ($(selftype) *self, amq_smessage_t *message)
{
    uint
        level;

    level = message->priority / (AMQP_MAX_PRIORITIES / self->opt_priority_levels);
    if (level > self->opt_priority_levels - 1)
        level = self->opt_priority_levels - 1;

    return (level);
}


/*  Return next consumer for queue, NULL if there are none                   */

static amq_consumer_t
*s_get_next_consumer ($(selftype) *self)
{
    amq_consumer_t
        *consumer;                      /*  Next consumer to process         */

    consumer = amq_consumer_list_first (self->consumers);
    while (consumer) {
        if (consumer->window && !consumer->handle->paused)
            break;
        consumer = amq_consumer_list_next (self->consumers, consumer);
    }
    return (consumer);
}

/*  Note that consumer->queue is also self at this stage                     */

static void
s_dispatch_message (
    amq_consumer_t *consumer,
    amq_smessage_t *message)
{
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */

    assert (consumer);
    dispatch = amq_dispatch_new (consumer, message);

#   ifdef TRACE_DISPATCH
    coprintf ("$(selfname) I: ==> dispatch nbr=%d", dispatch->message_nbr);
#   endif
    amq_server_agent_handle_notify (
        consumer->thread,
        (dbyte) consumer->handle->key,
        dispatch->message_nbr,
        FALSE, TRUE, FALSE,
        consumer->queue->key,
        message,
        consumer->unreliable? dispatch: NULL);

    /*  Move consumer to end of list to round-robin it                       */
    amq_consumer_list_unlink (consumer);
    amq_consumer_list_queue  (consumer->queue->consumers, consumer);
}
</private>

<method name = "selftest" />

</class>
