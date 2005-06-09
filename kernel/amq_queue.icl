<?xml?>
<class
    name      = "amq_queue"
    comment   = "Message queue class"
    version   = "1.3"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class defines a message queue.  Message queues are used for several
purposes in the server: for point to point queues (AMQP_SERVICE_QUEUE),
for topic destinations (AMQP_SERVICE_TOPIC), and for subscriptions. For
topics (aka pubsub), there is one primary queue and one queue per
subscription.  Subscription queues hold references rather than complete
messages.  We use reference counting to track these references and delete
the original message when the last reference has been dispatched, ack'd
and deleted.  A queue holds messages in memory and/or on disk using
persistent storage depending on its configuration, size, and the types
of messages (persistent or not) passed to it.  Each instance of this
object is tied to a single matching instance of amq_dest.  The two classes
were split to keep the code within sane limits.
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
    <field name = "links"       type = "longint" >Reference count</field>

    <!-- These are the properties of a persistent message reference
         The dest id is the key of a db_dest which lets us find the
         queue object for the original message even after a server
         stop/restart.  Kind of complex.
      -->
    <field name = "dest id"     type = "longint" >Identifier of db_dest</field>
    <field name = "message id"  type = "longint" >Message identifier in queue</field>
</data>

<import class = "amq_global" />
<import class = "amq_db" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<private name = "header">
#include "amq_server_agent.h"
#define TRACE_DISPATCH                  /*  Trace dispatching progress?      */
#undef  TRACE_DISPATCH
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
    Bool
        dynamic;                        /*  Dynamic queue?                   */
    Bool
        exclusive;                      /*  Exclusive consumer on queue      */
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
    Bool
        forced;                         /*  Queue was forced persistent      */

    /*  Statistics                                                           */
    size_t
        accept_count,                   /*  Number of messages accepted      */
        dispatch_count;                 /*  Number of messages dispatched    */
    Bool
        monitor_pending;                /*  When changed after monitoring    */
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
    self_locate (self, filename);
    if (file_exists (filename)) {
        self->disk_queue_size = self_count (self);
        if (self->disk_queue_size) {
            if (self->dest->opt_auto_purge || self->dest->dynamic)
                self_purge (self);
            else
               if (amq_global_verbose ())
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
    Attach consumer to queue and dispatch any outstanding
    messages on the queue.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Consumer</argument>
    consumer->queue = self;
    self->nbr_consumers++;
    self->window += consumer->window;
    if (consumer->dynamic)
        self->dynamic = TRUE;           /*  When any consumer is dynamic     */
    self_pre_dispatch (self);
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

    /*  If dynamic and no more consumers, destroy the queue                  */
    if (self->dynamic && self->nbr_consumers == 0 && amq_global_monitor () == 0) {
        coprintf ("I: destroying dynamic queue '%s'", self->dest->key);
        amq_dest_destroy (&self->dest);
    }
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
    <argument name = "channel"   type = "amq_channel_t *" >Current channel</argument>
    <argument name = "message"   type = "amq_smessage_t *">Message, if any</argument>
    <argument name = "immediate" type = "Bool"            >Assert immediate delivery?</argument>
    <argument name = "txn"       type = "ipr_db_txn_t *"  >Transaction, if any</argument>
    assert (message);
    if (self->dest->opt_persistent)
        message->persistent = TRUE;     /*  Force message to be persistent   */

    /*  First check that posting to this queue is currently allowed          */
    if (immediate && self->nbr_consumers == 0)
        amq_global_set_error (AMQP_NOT_ALLOWED, "Destination has no consumers, immediate delivery failed");
    else
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
        /*  Force persistent if queue is full, unless message is critical    */
        if (self->dest->opt_memory_queue_max > 0
        &&  self->memory_queue_size >= self->dest->opt_memory_queue_max
        &&  message->priority &lt; AMQP_MAX_PRIORITIES) {
            if (!self->forced) {
                coprintf ("$(selfname) I: queue full - forcing message to persistent storage");
                self->forced = TRUE;
            }
            s_accept_persistent (self, message, txn);
        }
        else
        /*  Force persistent if there are already queued messages on disk    */
        if (self->disk_queue_size > 0 && s_priority_level (self, message) == 0)
            s_accept_persistent (self, message, txn);
        else
        if (message->persistent)
            s_accept_persistent (self, message, txn);
        else {
            s_accept_transient (self, message, txn);
            self->forced = FALSE;
        }
        self->accept_count++;
        self->monitor_pending = TRUE;
    }
</method>

<method name = "pre dispatch" template = "function">
    <doc>
    Flags the queue as "dirty" and moves it to the front of the dispatch
    list so that the virtual host will dispatch it next.
    </doc>
    self->dirty = TRUE;
    amq_dest_list_push (self->vhost->dest_list, self->dest);
</method>

<method name = "publish" template = "function">
    <doc>
    Publishes a message to a subscriber queue.
    </doc>
    <argument name = "message" type = "amq_smessage_t *">Message, if any</argument>
    <argument name = "txn"     type = "ipr_db_txn_t *"  >Transaction, if any</argument>
    <local>
    uint
        level;                          /*  Priority level                   */
    </local>
    assert (message);

    /*  If there are already messages on disk, send low priority
        messages to disk as well so they are delivered in order
    */
    level = s_priority_level (self, message);
    if (self->dest->opt_persistent
    || (self->disk_queue_size > 0 && level == 0))
        message->persistent = TRUE;

    if (message->persistent) {
        s_insert_byreference (self, message, txn);
        self->disk_queue_size++;
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: %s - save persistent message to storage (%d)",
            self->dest->key, self->disk_queue_size);
#       endif
    }
    else {
        /*  Non-persistent references are held per message queue             */
        amq_mesgref_new (self->message_list [level], message);
        self->memory_queue_size++;
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: %s - save non-persistent message to queue memory (%d)",
            self->dest->key, self->memory_queue_size);
#       endif
    }
    self_pre_dispatch (self);
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
                    s_dispatch_message (self, consumer, message_ref->message);
                    amq_mesgref_destroy (&message_ref);
                    self->memory_queue_size--;
                    self->item_id = 0;  /*  Non-persistent message           */

                    /*  Get next message (is now first on queue)             */
                    message_ref = amq_mesgref_list_first (self->message_list [level]);

                    /*  Work down to next priority level if needed           */
                    while (message_ref == NULL && level > 0) {
                        level--;
                        message_ref = amq_mesgref_list_first (self->message_list [level]);
                    }
                }
                else
                    break;              /*  No more consumers                */
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
                    message = s_fetch_byreference (self, consumer->handle);
                    s_dispatch_message   (self, consumer, message);
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
        if (self_fetch (self, IPR_QUEUE_EQ) == 0)
            message = s_fetch_byreference (self, handle);

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
            message);
    else
        rc = 1;                         /*  No message to dispatch           */
</method>

<method name = "delete message" template = "function">
    <doc>
    Deletes a persistent message or a persistent reference along with the
    message if it's the last reference.
    </doc>
    <argument name = "txn" type = "ipr_db_txn_t *">Transaction, if any</argument>
    s_delete_byreference (self, txn);
</method>

<method name = "purge">
    if (amq_global_verbose ())
        coprintf ("I: purging %ld messages from %s", self->disk_queue_size, self->dest->key);
    self->disk_queue_size = 0;
</method>


<private name = "header">
static void
    s_accept_persistent ($(selftype) *self, amq_smessage_t *message, ipr_db_txn_t *txn);
static void
    s_accept_transient ($(selftype) *self, amq_smessage_t *message, ipr_db_txn_t *txn);
static int
    s_priority_level ($(selftype) *self, amq_smessage_t *message);
static void
    s_insert_byreference ($(selftype) *self, amq_smessage_t *message, ipr_db_txn_t *txn);
static amq_smessage_t *
    s_fetch_byreference ($(selftype) *self, amq_handle_t *handle);
static void
    s_delete_byreference ($(selftype) *self, ipr_db_txn_t *txn);
static amq_consumer_t *
    s_get_next_consumer ($(selftype) *self);
static void
    s_dispatch_message ($(selftype) *self, amq_consumer_t *consumer, amq_smessage_t *message);
</private>

<private name = "footer">
/*  Save a persistent message to the queue                                   */

static void
s_accept_persistent (
    $(selftype)    *self,
    amq_smessage_t *message,
    ipr_db_txn_t   *txn)
{
    amq_hitset_t
        *hitset;

    if (self->dest->service_type == AMQP_SERVICE_TOPIC) {
        hitset = amq_hitset_new (self->vhost);
        if (amq_hitset_match (hitset, message)) {
            amq_smessage_save (message, self, txn);
            self->disk_queue_size++;
            amq_hitset_publish (hitset, message, txn);
        }
        amq_hitset_destroy (&hitset);
    }
    else {                              /*  AMQP_SERVICE_QUEUE               */
        amq_smessage_save (message, self, txn);
        self->disk_queue_size++;
        self_pre_dispatch (self);
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: %s - save persistent message to storage (%d)",
            self->dest->key, self->disk_queue_size);
#       endif
    }
}


/*  Save a transient message to the queue                                    */

static void
s_accept_transient (
    $(selftype)    *self,
    amq_smessage_t *message,
    ipr_db_txn_t   *txn)
{
    amq_hitset_t
        *hitset;

    if (self->dest->service_type == AMQP_SERVICE_TOPIC) {
        /*  For topics we don't save on the topic queue                      */
        hitset = amq_hitset_new (self->vhost);
        if (amq_hitset_match (hitset, message))
            amq_hitset_publish (hitset, message, txn);

        amq_hitset_destroy (&hitset);
    }
    else {                              /*  AMQP_SERVICE_QUEUE               */
        amq_mesgref_new (self->message_list [s_priority_level (self, message)], message);
        self->memory_queue_size++;
        self_pre_dispatch (self);
#       ifdef TRACE_DISPATCH
        coprintf ("$(selfname) I: %s - save non-persistent message to queue memory (%d)",
            self->dest->key, self->memory_queue_size);
#       endif
    }
}

/*  Calculate priority level for this message                                */

static int
s_priority_level (
    $(selftype) *self,
    amq_smessage_t *message)
{
    uint
        level;

    level = message->priority / (AMQP_MAX_PRIORITIES / self->dest->opt_priority_levels);
    if (level > self->dest->opt_priority_levels - 1)
        level = self->dest->opt_priority_levels - 1;

    return (level);
}


/*  Insert persistent reference to message                                   */

static void
s_insert_byreference (
    $(selftype)    *self,               /*  Queue to insert into             */
    amq_smessage_t *message,            /*  Message to refer to              */
    ipr_db_txn_t   *txn                 /*  Transaction, if any              */
)
{
    /*  Insert reference in this queue                                       */
    /*  This code looks a little like smessage_save                          */
    self->item_client_id  = 0;
    self->item_sender_id  = message->handle->client_id;
    self->item_dest_id    = message->queue->dest->db_dest->id;
    self->item_message_id = message->queue_id;
    amq_queue_insert (self, txn);

    /*  We keep a reference count in the original persistent message         */
    message->queue->item_id = message->queue_id;
    amq_queue_link (message->queue, txn);
}

/*  Create message fetched directly or via persistent reference              */

static amq_smessage_t
*s_fetch_byreference (
    $(selftype)  *self,                 /*  Queue holding reference          */
    amq_handle_t *handle                /*  Handle that owns message         */
)
{
    amq_smessage_t
        *message;                       /*  Populated message object         */
    amq_db_dest_t
        *db_dest;                       /*  Entry in db_dest table           */
    amq_dest_t
        *dest;                          /*  Resolved destination object      */

    message = amq_smessage_new (handle);
    if (self->item_dest_id) {
        /*  We need to translate the dest_id into a queue object...          */
        db_dest = amq_db_dest_new ();
        db_dest->id = self->item_dest_id;

        if (amq_db_dest_fetch (self->vhost->ddb, db_dest, AMQ_DB_FETCH_EQ))
            coprintf ("E: destination not found (id=%ld)", db_dest->id);
        else {
            dest = self->vhost->topic_dest;
            assert (dest);
            assert (dest->queue);
            dest->queue->item_id = self->item_message_id;
            amq_smessage_load (message, dest->queue);
        }
        amq_db_dest_destroy (&db_dest);
    }
    else
        amq_smessage_load (message, self);

    return (message);
}

/*  Delete persistent reference and possibly original message                */

static void
s_delete_byreference (
    $(selftype)  *self,                 /*  Queue holding reference          */
    ipr_db_txn_t *txn                   /*  Transaction, if any              */
)
{
    amq_db_dest_t
        *db_dest;
    amq_dest_t
        *dest;

    if (self->item_dest_id) {
        /*  We need to translate the dest_id into a queue object...          */
        db_dest = amq_db_dest_new ();
        db_dest->id = self->item_dest_id;

        if (amq_db_dest_fetch (self->vhost->ddb, db_dest, AMQ_DB_FETCH_EQ))
            coprintf ("E: destination not found (id=%ld)", db_dest->id);
        else {
            dest = self->vhost->topic_dest;
            assert (dest);
            assert (dest->queue);
            dest->queue->item_id = self->item_message_id;
            amq_queue_delete (dest->queue, txn);
            dest->queue->disk_queue_size--;
        }
        amq_db_dest_destroy (&db_dest);
    }
    amq_queue_delete (self, txn);
}


/*  Return next consumer for queue, NULL if there are none                   */

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
    $(selftype)    *self,
    amq_consumer_t *consumer,
    amq_smessage_t *message)
{
    qbyte
        message_nbr;

    assert (consumer);

    /*  The queue releases its handle on the message after dispatching but
        we want to hold onto it a little longer.  The message is passed to
        both the server agent and optionally to the dispatcher.  Both will
        destroy it when finished with it.  We now link on behalf of the
        server agent...
     */
    amq_smessage_link (message);

    /*  Get a new message number - per channel                               */
    message_nbr = ++(consumer->handle->channel->message_nbr);

    /*  Create a dispatch object unless it's a no-ack consumer               */
    if (!consumer->no_ack)
        amq_dispatch_new (consumer, message, message_nbr);

#   ifdef TRACE_DISPATCH
    coprintf ("$(selfname) I: ==> dispatch nbr=%d", message_nbr);
#   endif
    amq_server_agent_handle_notify (
        consumer->handle->thread,
        (dbyte) consumer->handle->key,
        message_nbr,
        FALSE,                          /*  Recovery - restarting            */
        TRUE,                           /*  Delivered message                */
        FALSE,                          /*  Redelivered message              */
        message->dest_name,
        message);

    /*  Move consumer to end of list to round-robin it                       */
    amq_consumer_by_queue_queue (consumer->queue->active_consumers, consumer);

    self->dispatch_count++;
    self->monitor_pending = TRUE;
}
</private>

<method name = "selftest" />

</class>
