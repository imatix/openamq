<?xml?>
<class
    name      = "amq_queue"
    comment   = "Queue class"
    version   = "1.0"
    copyright = "Copyright (c) 2004 JPMorgan"
    script    = "icl_gen"
    >
<doc>
Implements a queue object; held per virtual host and tracking the current
status of a queue destination. Acts as a container for queue consumers.
</doc>

<inherit class = "ipr_hash_str" />
<option name = "nullify"  value = "1" />
<option name = "bigtable" value = "1" />

<import class = "ipr_classes"  />
<import class = "amq_db"       />
<import class = "amq_db_hack"  />
<import class = "amq_consumer" />
<import class = "amq_message"  />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>
<private>
#include "amq_server_agent.h"
</private>

<context>
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */

    /*  Object properties                                                    */
    amq_db_t
        *db;                            /*  Database for virtual host        */
    amq_db_dest_t
        *dest;                          /*  Destination record               */
    amq_consumer_list_t
        *consumers;                     /*  Consumers for this queue         */
    amq_smessage_list_t
        *messages;                      /*  Non-pers. messages for queue     */
    qbyte
        window;                         /*  Total window availability        */
    qbyte
        last_mesg_id;                   /*  Last dispatched message          */
    size_t
        saved_mesgs;                    /*  Nbr. outstanding messages        */
</context>

<method name = "new">
    <argument name = "vhost"     type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "client_id" type = "qbyte"        >Owner, if any</argument>
    <argument name = "temporary" type = "Bool"         >Temporary queue?</argument>

    /*  De-normalise from parent object, for simplicity of use               */
    self->vhost = vhost;
    self->db    = vhost->db;

    /*  Initialise other properties                                          */
    self->dest      = amq_db_dest_new ();
    self->consumers = amq_consumer_list_new ();
    self->messages  = amq_smessage_list_new ();

    self->dest->type = AMQP_SERVICE_QUEUE;
    ipr_shortstr_cpy (self->dest->name, key);

    /*  Fetch existing queue if caller specified a name                      */
    if (*key)
        amq_db_dest_fetch_byname (self->db, self->dest, AMQ_DB_FETCH_EQ);

    if (temporary) {
        /*  Create or attach to temporary queue                              */
        if (self->dest->id) {
            /*  Temporary queue exists, check client_id                      */
            ASSERT (client_id);
            if (self->dest->client_id == client_id) {
                /*  ***TODO*** purge queue of all messages
                 */
            }
            else {
                coprintf ("$(selfname) E: temporary queue '%s' already in use", key);
                $(selfname)_destroy (&self);
            }
        }
        else {
            /*  Unnamed temporary queue - create it now                      */
            self->dest->temporary = TRUE;
            self->dest->client_id = client_id;
            amq_db_dest_insert (self->db, self->dest);
        }
    }
    else {
        /*  Create configured destination                                    */
        if (self->dest->id == 0)
            amq_db_dest_insert (self->db, self->dest);
    }
</method>

<method name = "destroy">
    amq_smessage_list_destroy (&self->messages);
    amq_consumer_list_destroy (&self->consumers);
    if (self->dest->temporary)
        amq_db_dest_delete (self->db, self->dest);
    amq_db_dest_destroy (&self->dest);
</method>

<method name = "consume" return = "queue">
    <argument name = "consumer" type = "amq_consumer_t *">Parent consumer</argument>
    <argument name = "suffix"   type = "char *"          >Destination name suffix</argument>
    <declare  name = "queue"    type = "$(selftype) *" default = "NULL"/>

    queue = $(selfname)_full_search (
        consumer->vhost->queues, consumer->handle->dest_name, suffix);
    if (queue) {
        if (queue->dest->client_id == consumer->client_id
        || !queue->dest->temporary) {
            amq_consumer_list_queue (queue->consumers, consumer);
            queue->window += consumer->window;
        }
        else {
            coprintf ("$(selfname) W: consume failed, not owner of temp.queue");
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

<method name = "dispatch" template = "function">
    <doc>
    Dispatches a specific message or all pending messages on a destination
    to all interested consumers.
    </doc>
    <argument name = "cur_message" type = "amq_smessage_t *">Message, if any</argument>
    <local>
    amq_db_mesg_t
        *mesg;
    amq_smessage_t
        *message;
    amq_consumer_t
        *consumer;                      /*  Next consumer to process         */
    int
        finished;
    </local>

    /*  If we have a current message, save to disk or memory                 */
    if (cur_message) {
        if (self->saved_mesgs > 0 || cur_message->persistent) {
            /*  Save to persistent storage                                   */
            self->saved_mesgs++;
            amq_smessage_save (cur_message, self->dest->id);
            amq_smessage_destroy (&cur_message);
        }
        else
            /*  Added to memory queue                                        */
            amq_smessage_list_queue (self->messages, cur_message);
    }

    /*  Now process messages from memory or from disk                        */
    if (self->saved_mesgs) {
        /*  Get oldest candidate message to dispatch                         */
        mesg = amq_db_mesg_new ();
        mesg->id      = self->last_mesg_id;
        mesg->dest_id = self->dest->id;

        finished = amq_db_mesg_fetch_bydest (self->db, mesg, AMQ_DB_FETCH_GT);
        while (self->window && !finished) {
            if (mesg->client_id == 0) {
                consumer = s_get_next_consumer (self);
                message  = amq_smessage_new (consumer->handle);
                amq_smessage_load (message, mesg->id);
                self->last_mesg_id = mesg->id;

                s_dispatch_message (self, consumer, message, mesg->id);
                /*  ***TODO*** use channel transaction if transacted
                */
                mesg->client_id = consumer->client_id;
                amq_db_mesg_update (self->db, mesg);
            }
            else
                /*  Already dispatched                                       */
                self->last_mesg_id = mesg->id;

            finished = amq_db_mesg_fetch_bydest (self->db, mesg, AMQ_DB_FETCH_NEXT);
        }
        amq_db_mesg_destroy (&mesg);
    }
    else {
        message = amq_smessage_list_first (self->messages);
        while (self->window && message) {
            consumer = s_get_next_consumer (self);
            amq_smessage_list_unlink (message);
            s_dispatch_message (self, consumer, message, 0);
            message = amq_smessage_list_first (self->messages);
        }
    }
</method>

<private name = "header">
static amq_consumer_t *
    s_get_next_consumer ($(selftype) *self);
static void
    s_dispatch_message ($(selftype) *self, amq_consumer_t *consumer,
                         amq_smessage_t *message, qbyte mesg_id);
</private>

<private name = "footer">
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

static void
s_dispatch_message (
    $(selftype)    *self,
    amq_consumer_t *consumer,
    amq_smessage_t *message,
    qbyte           mesg_id)
{
    amq_dispatch_t
        *dispatch;                      /*  Dispatched message queue entry   */

    ASSERT (consumer);
    dispatch = amq_dispatch_new (consumer, message, mesg_id);

    amq_server_agent_handle_notify (
        consumer->thread,
        (dbyte) consumer->handle->key,
        dispatch->message_nbr,
        FALSE, TRUE, FALSE,
        icl_mem_strdup (self->dest->name),
        message,
        consumer->unreliable? dispatch: NULL);

    /*  Move consumer to end of list to round-robin it                       */
    amq_consumer_list_unlink (consumer);
    amq_consumer_list_queue  (self->consumers, consumer);
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
