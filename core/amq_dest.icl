<?xml?>
<class
    name      = "amq_dest"
    comment   = "Destination class"
    version   = "1.2"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
Implements a destination object, which is a queue structure.  Supports
shared message queues, temporary reply queues, and topic history queues.
Destinations are held per virtual host. Is a container for consumers for
the destination.  The dest object is backed by a persistent database
table based on the ipr_db_queue class.
</doc>

<inherit class = "ipr_hash_str" />
<inherit class = "ipr_db_queue" />
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
    <field name = "store size"  type = "longint" >Stored file size, or 0</field>
</data>

<import class = "amq_global" />
<import class = "amq_db"     />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"

/*  Types of dest object                                                     */
#define AMQ_DEST_TYPE_QUEUE     1       /*  Shared message queue             */
#define AMQ_DEST_TYPE_TEMPQ     2       /*  Temporary reply queue            */
#define AMQ_DEST_TYPE_TOPIC     3       /*  Topic history queue              */
#define AMQ_DEST_TYPE_VALID(t)  (t > 0 && t < 4)
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
        *dest_ref;                      /*  Item in vhost dest list          */

    /*  Object properties                                                    */
    int
        type;                           /*  Destination type                 */
    qbyte
        client_id;                      /*  Owner, if any                    */
    amq_consumer_list_t
        *consumers;                     /*  Consumers for this destination   */
    amq_smessage_list_t
        **message_list;                 /*  Pending non-persistent messages  */
    qbyte
        window;                         /*  Total window availability        */
    qbyte
        last_id;                        /*  Last dispatched message          */
    size_t
        disk_queue_size;                /*  Size of disk queue               */
    size_t
        memory_queue_size;              /*  Size of memory queue             */
    size_t
        nbr_consumers;                  /*  Number of consumers              */
    Bool
        dirty;                          /*  Dest needs dispatching           */

    /*  Dest options, loaded from dest configuration in amq_vhost.cfg        */
    size_t
        opt_min_consumers;              /*  Minimum required consumers       */
    size_t
        opt_max_consumers;              /*  Maximum allowed consumers        */
    size_t
        opt_memory_queue_max;           /*  Max. size of memory queue        */
    size_t
        opt_max_messages;               /*  Max. allowed messages            */
    size_t
        opt_max_message_size;           /*  Max. allowed message size        */
    size_t
        opt_priority_levels;            /*  Number of priority levels        */
    Bool
        opt_browsable;                  /*  Browsing allowed?                */
    Bool
        opt_persistent;                 /*  Force persistent messages?       */
</context>

<method name = "new">
    <argument name = "vhost"      type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "type"       type = "int"          >Destination type</argument>
    <argument name = "name"       type = "char *"       >External name</argument>
    <argument name = "client_id"  type = "qbyte"        >Owner, if any</argument>
    
    <!-- Remove these inherited arguments from the API -->
    <dismiss argument = "db"        value = "vhost->db"         />
    <dismiss argument = "filename"  value = "key"               />
    <dismiss argument = "table"     value = "vhost->dest_hash"  />

    assert (AMQ_DEST_TYPE_VALID (type));

    /*  De-normalise from parent object, for simplicity of use               */
    self->vhost = vhost;
    self->db    = vhost->db;

    self->type      = type;
    self->client_id = client_id;
    s_get_configuration (self, name);

    /*  Create data structures for dispatchable destinations                 */
    if (type != AMQ_DEST_TYPE_TOPIC) {
        self->consumers = amq_consumer_list_new ();
        self->dest_ref = ipr_looseref_new (vhost->dest_refs, self);
        s_create_priority_lists (self);
    }
</method>

<method name = "destroy">
    s_destroy_priority_lists (self);
    ipr_looseref_destroy (&self->dest_ref);
    amq_consumer_list_destroy (&self->consumers);
</method>

<method name = "map name">
    <doc>
    Turns an external destination name and type into an internal name;
    the mapping function creates names that are globally unique and
    file-system safe (contains no special characters).  A given external
    name is always mapped to the same internal name.
    </doc>
    <argument name = "mapped" type = "char *">Mapped name</argument>
    <argument name = "name"   type = "char *">Destination name</argument>
    <argument name = "type"   type = "int"   >Destination type</argument>
    <local>
    byte
        hash = 0;
    char
        *name_ptr;
    </local>

    /*  Calculate mod-255 hash for name                                      */
    for (name_ptr = name; *name_ptr; name_ptr++)
        hash += *name_ptr;
        
    ipr_shortstr_fmt (mapped, "%s-%s-%02x",
        type == AMQ_DEST_TYPE_QUEUE? "queue":
        type == AMQ_DEST_TYPE_TEMPQ? "queue":
        type == AMQ_DEST_TYPE_TOPIC? "topic": "",
        name, hash);
    for (name_ptr = mapped; *name_ptr; name_ptr++)
        if (!(isalnum (*name_ptr) || *name_ptr == '-'))
            *name_ptr = '-';
</method>            

<method name = "full search" return = "dest">
    <doc>
    Handles destination name pasting, whereby the dest name is built
    from a prefix specified at handle-open time, and a suffix specified
    at the individual command level.
    </doc>
    <argument name = "dests"  type = "$(selfname)_table_t *">Table to search</argument>
    <argument name = "prefix" type = "char *">Dest prefix</argument>
    <argument name = "suffix" type = "char *">Dest suffix</argument>
    <argument name = "type"   type = "int"   >Destination type</argument>
    <declare  name = "dest"   type = "$(selftype) *" default = "NULL"/>
    <local>
    ipr_shortstr_t
        pasted;
    ipr_shortstr_t
        internal;                       /*  Internal dest name               */
    </local>

    /*  Paste destination name, then map to internal name                    */
    ipr_shortstr_fmt (pasted, "%s%s", prefix, suffix);
    self_map_name    (internal, pasted, type);
    dest = self_search (dests, internal);
</method>

<method name = "consume" return = "dest">
    <doc>
    Lookup and return dest, if any, for specified name. Concats the handle
    destination with the supplied suffix to give a full dest name.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    <argument name = "suffix"   type = "char *"          >Destination name suffix</argument>
    <declare  name = "dest"     type = "$(selftype) *" default = "NULL"/>

    /*  Look for queue - queues and temp-queues are in same namespace        */
    dest = self_full_search (
        consumer->vhost->dest_hash,
        consumer->handle->dest_name,
        suffix,
        AMQ_DEST_TYPE_QUEUE);

   if (dest) {
        /*  Allowed as consumer if shared queue or owner of tempq            */
        if (dest->type == AMQ_DEST_TYPE_QUEUE || dest->client_id == consumer->client_id) {
            if (dest->opt_max_consumers == 0
            ||  dest->nbr_consumers < dest->opt_max_consumers) {
                amq_consumer_list_queue (dest->consumers, consumer);
                dest->nbr_consumers++;
                dest->window += consumer->window;
            }
            else {
                amq_global_set_error (AMQP_NOT_ALLOWED, "Destination consumer limit reached");
                dest = NULL;
            }
        }
        else {
            amq_global_set_error (AMQP_ACCESS_REFUSED, "Not owner of temporary queue");
            dest = NULL;
        }
    }
    else
        amq_global_set_error (AMQP_NOT_FOUND, "No such destination defined");
</method>

<method name = "cancel" template = "function">
    <doc>
    Cancels a consumer on a specific dest, updating the dest as needed.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    self->nbr_consumers--;
    self->window -= consumer->window;
</method>

<method name = "accept" template = "function">
    <doc>
    Saves a message to a destination. Transacted messages are held in memory
    pending a channel commit or roll back.  Non-transacted persistent messages
    are saved to the persistent dest and n-t n-p messages are saved to the dest
    priority lists.  The channel can be specified as NULL, which forces
    non-persistent messages to be saved to the dest memory list.
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

    /*  First check that posting to this dest is currently allowed           */
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
            message->dest = self;
            amq_smessage_list_queue (channel->transact_list, message);
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
            /*  Persistent messages are saved on persistent dest storage     */
            self->disk_queue_size++;
            amq_smessage_save (message, self, txn);
            amq_smessage_destroy (&message);
    #       ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: save persistent message to storage");
    #       endif
        }
        else {
            /*  Non-persistent messages are held per dest                    */
            self->memory_queue_size++;
            amq_smessage_list_queue (self->message_list [level], message);
    #       ifdef TRACE_DISPATCH
            coprintf ("$(selfname) I: save non-persistent message to dest memory");
    #       endif
        }
        self->dirty = TRUE;             /*  Dest has new data                */

        /*  We move all dirty dests to the start of the vhost list so that
            dispatching can be rapid when the vhost has lots of dests       */
        ipr_looseref_list_push (self->vhost->dest_refs, self->dest_ref);
    }
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatches all pending messages waiting on the specified dest.
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
                    /*  Get next message (is now first on dest)                  */
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
    self->dirty = FALSE;                /*  Dest has been dispatched         */
</method>

<method name = "query" template = "function">
    <doc>
    Loads a query set referring to every available message in the
    destination.
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
    Bool
        destroy_message = FALSE;        /*  Destroy when sent?               */
    </local>
    if (browser->message) {
        message = browser->message;
        destroy_message = FALSE;
    }
    else
    if (browser->item_id) {
        self->item_id = browser->item_id;
        if (self_fetch (self, IPR_QUEUE_EQ) == 0) {
            message = amq_smessage_new (handle);
            destroy_message = TRUE;
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
            NULL,
            destroy_message);
    else
        rc = 1;                         /*  No message to dispatch           */
</method>

<private name = "header">
static void
    s_get_configuration ($(selftype) *self, char *name);
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
/*  Load dest options from virtual host configuration values                 */

static void
s_get_configuration ($(selftype) *self, char *name)
{
    if (self->type == AMQ_DEST_TYPE_QUEUE) {
        if (ipr_config_locate (self->vhost->config, "/config/queues/queue", name) == -1)
            ipr_config_locate (self->vhost->config, "/config/template/queue", "default");

    }
    else
    if (self->type == AMQ_DEST_TYPE_TEMPQ) {
        ipr_config_locate (self->vhost->config, "/config/template/tempq", "default");
    }
    else
    if (self->type == AMQ_DEST_TYPE_TOPIC) {
        if (ipr_config_locate (self->vhost->config, "/config/topics/topic", name) == -1)
            ipr_config_locate (self->vhost->config, "/config/template/topic", "default");
    }
    if (self->vhost->config->located) {
        self->opt_min_consumers    = ipr_config_attrn (self->vhost->config, "min-consumers");
        self->opt_max_consumers    = ipr_config_attrn (self->vhost->config, "max-consumers");
        self->opt_memory_queue_max = ipr_config_attrn (self->vhost->config, "memory-queue-max");
        self->opt_max_messages     = ipr_config_attrn (self->vhost->config, "max-messages");
        self->opt_max_message_size = ipr_config_attrn (self->vhost->config, "max-message-size");
        self->opt_priority_levels  = ipr_config_attrn (self->vhost->config, "priority-levels");
        self->opt_browsable        = (Bool) ipr_config_attrn (self->vhost->config, "browsable");
        self->opt_persistent       = (Bool) ipr_config_attrn (self->vhost->config, "persistent");

        /*  Tune storage parameters (for physical backing file)              */
        self_tune (self,
                   ipr_config_attrn (self->vhost->config, "record-size"),
                   ipr_config_attrn (self->vhost->config, "extent-size"),
                   ipr_config_attrn (self->vhost->config, "block-size"));

        /*  auto-purge option means delete all messages at restart           */
        if (ipr_config_attrn (self->vhost->config, "auto-purge")
        ||  self->type == AMQ_DEST_TYPE_TEMPQ)
            self_purge (self);
        else {
            if (self_exists (self)) {
                self->disk_queue_size = self_count (self);
                coprintf ("I: %s has %d existing messages", self->key, self->disk_queue_size);
            }
        }
    }
    else {
        /*  Set defaults if no configuration found                           */
        self->opt_browsable = 1;
        if (self->type == AMQ_DEST_TYPE_TEMPQ)
            self_purge (self);
    }
}


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

/*  Destroy the destination's priority lists                                 */

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


/*  Return next consumer for dest, NULL if there are none                    */

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

/*  Note that consumer->dest is also self at this stage                      */

static void
s_dispatch_message (
    amq_consumer_t *consumer,
    amq_smessage_t *message)
{
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message dest entry    */

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
        consumer->dest->key,
        message,
        consumer->unreliable? dispatch: NULL,
        FALSE);                         /*  Don't auto-destroy message       */

    /*  Move consumer to end of list to round-robin it                       */
    amq_consumer_list_unlink (consumer);
    amq_consumer_list_queue  (consumer->dest->consumers, consumer);
}
</private>

<method name = "selftest" />

</class>
