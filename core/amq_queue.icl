<?xml?>
<class
    name      = "amq_queue"
    comment   = "Queue class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >
<doc>
Implements a queue object; held per virtual host and tracking the current
status of a queue destination. Acts as a container for queue consumers.
The queue object is backed by a persistent database table based on the
ipr_db_queue class.
</doc>

<inherit class = "ipr_hash_str" />
<inherit class = "ipr_db_queue" />
<option name = "nullify"  value = "1" />
<option name = "bigtable" value = "1" />

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
    <field name = "spool size"  type = "longint" >Spooled size, or 0</field>
</data>

<import class = "ipr_classes"  />
<import class = "amq_db"       />
<import class = "amq_consumer" />
<import class = "amq_message"  />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<private>
#include "amq_server_agent.h"
#define TRACE_DISPATCH                  /*  Trace dispatching progress?      */
#undef  TRACE_DISPATCH
</private>

<context>
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    ipr_looseref_t
        *queue_ref;                     /*  Item in vhost queue list         */

    /*  Object properties                                                    */
    amq_db_t
        *ddb;                           /*  Deprecated database handle       */
    amq_db_dest_t
        *dest;                          /*  Destination record               */
    amq_consumer_list_t
        *consumers;                     /*  Consumers for this queue         */
    amq_smessage_list_t
        *messages;                      /*  Non-pers. messages for queue     */
    qbyte
        window;                         /*  Total window availability        */
    qbyte
        last_id;                        /*  Last dispatched message          */
    size_t
        outstanding;                    /*  Nbr. outstanding messages        */
    Bool
        dirty;                          /*  Queue needs dispatching          */
</context>

<method name = "new">
    <argument name = "vhost"     type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "client_id" type = "qbyte"        >Owner, if any</argument>
    <argument name = "temporary" type = "Bool"         >Temporary queue?</argument>

    <!-- Remove these inherited arguments from the API -->
    <dismiss argument = "db"        value = "vhost->db"         />
    <dismiss argument = "filename"  value = "key"               />
    <dismiss argument = "table"     value = "vhost->queue_hash" />

    /*  De-normalise from parent object, for simplicity of use               */
    self->vhost = vhost;
    self->db    = vhost->db;
    self->ddb   = vhost->ddb;

    /*  Initialise other properties                                          */
    self->dest      = amq_db_dest_new  ();
    self->consumers = amq_consumer_list_new ();
    self->messages  = amq_smessage_list_new ();

    self->dest->type = AMQP_SERVICE_QUEUE;
    ipr_shortstr_cpy (self->dest->name, key);

    /*  Attach to list of queues for virtual host                            */
    self->queue_ref = ipr_looseref_new (vhost->queue_refs, self);

    /*  Fetch existing queue if caller specified a name                      */
    if (*key)
        amq_db_dest_fetch_byname (self->ddb, self->dest, AMQ_DB_FETCH_EQ);

    /*  If new queue, create it now                                          */
    if (self->dest->id == 0) {
        self->dest->temporary = temporary;
        self->dest->client_id = client_id;
        amq_db_dest_insert (self->ddb, self->dest);
    }
</method>

<method name = "destroy">
    amq_smessage_list_destroy (&self->messages);
    amq_consumer_list_destroy (&self->consumers);
    if (self->dest->temporary)
        amq_db_dest_delete (self->ddb, self->dest);
    amq_db_dest_destroy (&self->dest);
</method>

<method name = "consume" return = "queue">
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    <argument name = "suffix"   type = "char *"          >Destination name suffix</argument>
    <declare  name = "queue"    type = "$(selftype) *" default = "NULL"/>

    queue = $(selfname)_full_search (
        consumer->vhost->queue_hash, consumer->handle->dest_name, suffix);
    if (queue) {
        if (queue->dest->client_id == consumer->client_id
        || !queue->dest->temporary) {
            amq_consumer_list_queue (queue->consumers, consumer);
            queue->window += consumer->window;
        }
        else {
            coprintf ("$(selfname) W: consume failed, not queue owner");
            queue = NULL;               /*  Not owner of temporary queue     */
        }
    }
</method>

<method name = "cancel" template = "function">
    <doc>
    Cancels a consumer on a specific queue, updating the queue as needed.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    self->window -= consumer->window;
</method>

<method name = "accept" template = "function">
    <doc>
    Saves a message to a queue.  Transacted messages are held in memory pending
    a channel commit or roll back.  Non-transacted persistent messages are saved
    to the persistent queue and n-t n-p messages are saved to the queue memory.
    The channel can be specified as NULL, which forces non-persistent messages
    to be saved to the queue memory list.
    </doc>
    <argument name = "channel" type = "amq_channel_t *" >Current channel</argument>
    <argument name = "message" type = "amq_smessage_t *">Message, if any</argument>

    ASSERT (message);
    if (channel && channel->transacted) {
        /*  Transacted messages are held per-channel                         */
        message->queue = self;
        amq_smessage_list_queue (channel->messages, message);
    }
    else
    if (self->outstanding > 0 || message->persistent) {
        /*  Persistent messages are saved on persistent queue storage        */
        self->outstanding++;
        amq_smessage_save (message, self, NULL);
        amq_smessage_destroy (&message);
    }
    else {
        /*  Non-persistent messages are held per queue                       */
        amq_smessage_list_queue (self->messages, message);
    }
    self->dirty = TRUE;                 /*  Queue has new data               */

    /*  We move all dirty queues to the start of the vhost list so that
        dispatching can be rapid when the vhost has lots of queues           */
    ipr_looseref_list_push (self->vhost->queue_refs, self->queue_ref);
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatches a specific message or all pending messages on a destination
    to all interested consumers.
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
    </local>

    /*  Now process messages from memory or from disk                        */
#   ifdef TRACE_DISPATCH
    coprintf ("$(selfname) I: dispatch window=%d outstanding=%d", self->window, self->outstanding);
#   endif

    /*  First, process any messages in memory: in the case that messages
        are held both in memory and disk, the ones in memory will be the
        oldest.
     */
    message = amq_smessage_list_first (self->messages);
    while (self->window && message) {
        consumer = s_get_next_consumer (self);
        if (consumer) {
            amq_smessage_list_unlink (message);
            self->item_id = 0;          /*  Non-persistent message           */
            s_dispatch_message (consumer, message);

            /*  Invalidate any browsers for this message                     */
            browser_ref = ipr_looseref_list_first (message->browsers);
            while (browser_ref) {
                amq_browser_destroy ((amq_browser_t **) &browser_ref->object);
                browser_ref = ipr_looseref_list_next (message->browsers, browser_ref);
            }
            /*  Get next message (is now first on queue)                     */
            message = amq_smessage_list_first (self->messages);
        }
        else
            break;                      /*  No more consumers                */
    }
    /*  Now process any messages on disk                                     */
    if (self->outstanding) {
        /*  Get oldest candidate message to dispatch                         */
        self->item_id = self->last_id;
        finished = amq_queue_fetch (self, IPR_QUEUE_GT);
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

                    /*  Update client id, using channel txn if any       */
                    self->item_client_id = consumer->client_id;
                    amq_queue_update (self, consumer->channel->txn);
                }
                else
                    break;              /*  No more consumers                */
            }
            ASSERT (self->item_id > self->last_id);
            self->last_id = self->item_id;
            finished = amq_queue_fetch (self, IPR_QUEUE_NEXT);
        }
    }
    self->dirty = FALSE;                /*  Queue has been dispatched        */
</method>

<method name = "query" template = "function">
    <doc>
    Loads a query set referring to every available message in the queue.
    </doc>
    <argument name = "browser_set" type = "amq_browser_array_t *">Query set</argument>
    <local>
    amq_smessage_t
        *message;
    amq_browser_t
        *browser;
    qbyte
        set_index = 0;                  /*  Index into query set             */
    int
        finished;
    </local>
    /*  First, process any messages in memory: in the case that messages
        are held both in memory and disk, the ones in memory will be the
        oldest.
     */
    amq_browser_array_reset (browser_set);
    message = amq_smessage_list_first (self->messages);
    while (message) {
        /*  Track browser for message so we can invalidate it if/when
            we dispatch the message.                                         */
        browser = amq_browser_new (browser_set, set_index++, self, 0, message);
        ipr_looseref_new (message->browsers, browser);
        message = amq_smessage_list_next (self->messages, message);
    }
    /*  Now process any messages on disk                                     */
    if (self->outstanding) {
        /*  Get oldest candidate message to dispatch                         */
        self->item_id = self->last_id;
        finished = amq_queue_fetch (self, IPR_QUEUE_GT);
        while (!finished) {
            if (self->item_client_id == 0)
                amq_browser_new (browser_set, set_index++, self, self->item_id, NULL);
            finished = amq_queue_fetch (self, IPR_QUEUE_NEXT);
        }
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
    if (browser->message)
        message = browser->message;
    else
    if (browser->item_id) {
        self->item_id = browser->item_id;
        if (amq_queue_fetch (self, IPR_QUEUE_EQ) == 0) {
            message = amq_smessage_new (handle);
            amq_smessage_load (message, self);
        }
    }
    if (message)
        amq_server_agent_handle_notify (
            handle->thread,
            (dbyte) handle->key,
            0,
            FALSE, FALSE, FALSE,
            self->dest->name,
            message,
            NULL);
    else
        rc = 1;                         /*  No message to dispatch           */
</method>

<private name = "header">
static amq_consumer_t *
    s_get_next_consumer ($(selftype) *self);
static void
    s_dispatch_message (amq_consumer_t *consumer, amq_smessage_t *message);
</private>

<private name = "footer">
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

    ASSERT (consumer);
    dispatch = amq_dispatch_new (consumer, message);

#   ifdef TRACE_DISPATCH
    coprintf ("$(selfname) I: ==> dispatch nbr=%d", dispatch->message_nbr);
#   endif
    amq_server_agent_handle_notify (
        consumer->thread,
        (dbyte) consumer->handle->key,
        dispatch->message_nbr,
        FALSE, TRUE, FALSE,
        consumer->queue->dest->name,
        message,
        consumer->unreliable? dispatch: NULL);

    /*  Move consumer to end of list to round-robin it                       */
    amq_consumer_list_unlink (consumer);
    amq_consumer_list_queue  (consumer->queue->consumers, consumer);
}
</private>

<method name = "full search" return = "queue">
    <argument name = "queues" type = "$(selfname)_table_t *">Table to search</argument>
    <argument name = "prefix" type = "char *"               >Queue prefix</argument>
    <argument name = "suffix" type = "char *"               >Queue suffix</argument>
    <declare  name = "queue"  type = "$(selftype) *" default = "NULL"/>
    <doc>
    Handles queue name hierarchies, whereby the queue name is built
    from a prefix specified at handle-open time, and a suffix specified
    at the individual command level.
    </doc>
    <local>
    ipr_shortstr_t
        fullname;
    </local>
    ipr_shortstr_fmt (fullname, "%s%s", prefix, suffix);
    queue = amq_queue_search (queues, fullname);
</method>

<method name = "selftest" />

</class>
