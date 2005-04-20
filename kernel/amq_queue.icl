<?xml?>
<class
    name      = "amq_queue"
    comment   = "Message queue class"
    version   = "1.3"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class defines a message queue.
Queues hold messages received from publishers and messages being sent
to subscribers.  This class implements both point-to-point queues and
publish-and-subscribe topics.  A queue holds messages in memory and/or
on disk using persistent storage.
</doc>

<inherit class = "ipr_db_queue"  />

<data>
    <!-- These are the properties of a persistent message -->
    <field name = "dest name"   type = "shortstr">Destination name</field>
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

<import class = "amq_global" />
<import class = "amq_db" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<private>
#include "amq_server_agent.h"
#undef  TRACE_DISPATCH
#define TRACE_DISPATCH                  /*  Trace dispatching progress?      */
</private>

<context>
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    amq_dest_t
        *dest;                          /*  Parent destination               */

    /*  Object properties                                                    */
    amq_consumer_by_queue_t
        *active_consumers,              /*  Active consumers                 */
        *inactive_consumers;            /*  Inactive or busy consumers       */
    size_t
        nbr_consumers;                  /*  Number of consumers              */
    amq_mesgref_list_t
        **message_list;                 /*  Pending non-persistent messages  */
    qbyte
        window;                         /*  Total window availability        */
    qbyte
        last_id;                        /*  Last dispatched message          */
    size_t
        disk_queue_size;                /*  Size of disk queue               */
    size_t
        memory_queue_size;              /*  Size of memory queue             */
    Bool
        dirty;                          /*  Queue needs dispatching          */
</context>

<method name = "new">
    <doc>
    Creates a new queue object.  Each queue has a list of consumers, and
    various message lists.
    </doc>
    <argument name = "vhost" type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "dest"  type = "amq_dest_t *" >Parent destination</argument>
    <dismiss argument = "db" value = "vhost->db" />
        <local>
    uint
        level;
    </local>
    assert (dest);

    self->vhost = vhost;
    self->dest  = dest;
    self->active_consumers   = amq_consumer_by_queue_new ();
    self->inactive_consumers = amq_consumer_by_queue_new ();

    /*  Create 1..n priority lists                                           */
    self->message_list = icl_mem_alloc (
        self->dest->opt_priority_levels * sizeof (amq_mesgref_t *));
    for (level = 0; level < self->dest->opt_priority_levels; level++)
        self->message_list [level] = amq_mesgref_list_new ();

    /*  Tune physical backing file storage parameters                        */
    self_tune (self,
        self->dest->opt_record_size,
        self->dest->opt_extent_size,
        self->dest->opt_block_size);

    /*  auto-purge option means delete all messages at restart               */
    //TODO: purge temporary destinations only if requested
    //TODO: queues should not get temporary flag from dests
    if (self->dest->opt_auto_purge || self->dest->temporary)
        self_purge (self);
    else {
        self_locate (self, filename);
        if (file_exists (filename)) {
            self->disk_queue_size = self_count (self);
            coprintf ("I: %s has %d existing messages", filename, self->disk_queue_size);
        }
    }
</method>

<method name = "destroy">
    <local>
    uint
        level;
    amq_mesgref_t
        *message_ref;                   /*  Reference to message             */
    </local>
    /*  Destroy message lists                                                */
    for (level = 0; level < self->dest->opt_priority_levels; level++) {
        /*  Destroy the messages referred to by each list                    */
        message_ref = amq_mesgref_list_first (self->message_list [level]);
        while (message_ref) {
            amq_smessage_destroy (&message_ref->message);
            message_ref = amq_mesgref_list_next (self->message_list [level], message_ref);
        }
        amq_mesgref_list_destroy (&self->message_list [level]);
    }
    icl_mem_free (self->message_list);
    amq_consumer_by_queue_destroy (&self->active_consumers);
    amq_consumer_by_queue_destroy (&self->inactive_consumers);
</method>

<method name = "attach" template = "function">
    <doc>
    Attach consumer to queue and to handle, and dispatch any outstanding
    messages on the queue.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer</argument>
    consumer->queue = self;
    self->nbr_consumers++;
    self->window += consumer->window;
    amq_consumer_by_queue_queue  (self->active_consumers,      consumer);
    amq_consumer_by_handle_queue (consumer->handle->consumers, consumer);
    amq_queue_dispatch (self);
</method>

<method name = "detach" template = "function">
    <doc>
    Detach consumer from queue.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer</argument>
    self->nbr_consumers--;
    self->window -= consumer->window;
    if (self->window < 0)
        self->window = 0;
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
    if (self->dest->opt_persistent)
        message->persistent = TRUE;     /*  Force message to be persistent   */

    /*  First check that posting to this queue is currently allowed          */
    if (self->nbr_consumers < self->dest->opt_min_consumers)
        amq_global_set_error (AMQP_NOT_ALLOWED, "Destination needs consumers but has none");
    else
    if (self->dest->opt_max_messages > 0
    &&  self->disk_queue_size + self->memory_queue_size > self->dest->opt_max_messages)
        amq_global_set_error (AMQP_NOT_ALLOWED, "Destination is filled to capacity");
    else
    if (self->dest->opt_max_message_size > 0
    &&  message->body_size > self->dest->opt_max_message_size)
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
            coprintf ("$(selfname) I: %s - queue transacted message, txn_count=%d",
                self->dest->key, channel->transact_count);
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
            if (self->dest->opt_memory_queue_max > 0
            &&  self->memory_queue_size >= self->dest->opt_memory_queue_max
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
            coprintf ("$(selfname) I: %s - save persistent message to storage",
                self->dest->key);
    #       endif
        }
        else {
            /*  Non-persistent messages are held per message queue           */
            self->memory_queue_size++;
            amq_mesgref_new (self->message_list [level], message);
            if (self->dest->service_type == AMQP_SERVICE_TOPIC)
                amq_vhost_publish (self->vhost, self->dest->key, message);
    #       ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: %s - save non-persistent message to queue memory",
                self->dest->key);
    #       endif
        }
        /*  Mark queue as 'dirty' and push destination to front of list
            for eventual dispatching
         */
        self->dirty = TRUE;             /*  Message queue has new data       */
        amq_dest_list_push (self->vhost->dest_list, self->dest);
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
    amq_mesgref_t
        *message_ref;                   /*  Message on list (reference)      */
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
        coprintf ("$(selfname) I: %s - dispatching non-persistent window=%d pending=%d",
            self->dest->key, self->window, self->memory_queue_size);
#       endif

        /*  Start with highest-priority messages                             */
        level = self->dest->opt_priority_levels;
        while (level) {
            level--;
            message_ref = amq_mesgref_list_first (self->message_list [level]);
            while (self->window && message_ref) {
                consumer = s_get_next_consumer (self);
                if (consumer) {
                    s_dispatch_message (consumer, message_ref->message);
                    amq_mesgref_destroy (&message_ref);
                    self->memory_queue_size--;
                    self->item_id = 0;      /*  Non-persistent message           */

                    /*  Get next message (is now first on queue)                 */
                    message_ref = amq_mesgref_list_first (self->message_list [level]);

                    /*  Work down to next priority level if needed               */
                    while (message_ref == NULL && level > 0) {
                        level--;
                        message_ref = amq_mesgref_list_first (self->message_list [level]);
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
        coprintf ("$(selfname) I: %s - dispatching persistent window=%d pending=%d",
            self->dest->key, self->window, self->disk_queue_size);
#       endif

        /*  Get oldest candidate message to dispatch                         */
        self->item_id = self->last_id;
        finished = self_fetch (self, IPR_QUEUE_GT);
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: %s - fetch last=%d rc=%d id=%d finished=%d",
            self->dest->key, self->last_id, finished, self->item_id, finished);
#       endif

        while (self->window && !finished) {
#           ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: %s - message id=%d client=%d",
                self->dest->key, self->item_id, self->item_client_id);
#           endif
            if (self->item_client_id == 0) {
                consumer = s_get_next_consumer (self);
                if (consumer) {
                    message = amq_smessage_new (consumer->handle);
                    amq_smessage_load    (message, self);
                    s_dispatch_message   (consumer, message);
                    amq_smessage_destroy (&message);
                    self->item_client_id = consumer->handle->client_id;
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
    //TODO: implement maximum query size (0 = all)
    amq_mesgref_t
        *message_ref;                   /*  Message on list (reference)      */
    qbyte
        set_index = 0;                  /*  Index into query set             */
    amq_browser_t
        *browser;                       /*  Browser for message              */
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
    level = self->dest->opt_priority_levels;
    while (level) {
        level--;
        message_ref = amq_mesgref_list_first (self->message_list [level]);
        while (message_ref) {
            /*  Track browser for message so we can invalidate it if/when
                we dispatch the message.                                     */
            browser = amq_browser_new (
                browser_set, set_index++, self, 0, message_ref->message);
            ipr_looseref_new (message_ref->browsers, browser);
            message_ref = amq_mesgref_list_next (self->message_list [level], message_ref);
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
    Sends a single message to the user, where the message is specified by an
    amq_browse_t object.  If the message does not exist or has already been
    dispatched to another user, sends the user a 310 error.
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
            message->dest_name,
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
/*  Calculate priority level for this message                                */

static int
s_priority_level ($(selftype) *self, amq_smessage_t *message)
{
    uint
        level;

    level = message->priority / (AMQP_MAX_PRIORITIES / self->dest->opt_priority_levels);
    if (level > self->dest->opt_priority_levels - 1)
        level = self->dest->opt_priority_levels - 1;

    return (level);
}


/*  Return next consumer for queue, NULL if there are none                   */
//TODO: maintain inactive consumers on a separate list

static amq_consumer_t *
s_get_next_consumer ($(selftype) *self)
{
    amq_consumer_t
        *consumer;

    /*  When a consumer is used we move it to the end of the list, so the
        next consumer will be at start start of the list.
     */
    consumer = amq_consumer_by_queue_first (self->active_consumers);
    while (consumer) {
        if (consumer->window && !consumer->handle->paused)
            break;
        consumer = amq_consumer_by_queue_next (self->active_consumers, consumer);
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
        consumer->handle->thread,
        (dbyte) consumer->handle->key,
        dispatch->message_nbr,
        FALSE, TRUE, FALSE,
        message->dest_name,
        message,
        consumer->no_ack? dispatch: NULL);

    /*  Move consumer to end of list to round-robin it                       */
    amq_consumer_by_queue_queue (consumer->queue->active_consumers, consumer);
}
</private>

<method name = "selftest" />

</class>
