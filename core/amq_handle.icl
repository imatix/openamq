<?xml?>
<class
    name      = "amq_handle"
    comment   = "Destination handle class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

<inherit class = "ipr_hash_int"  />

<import class = "ipr_classes" />
<import class = "amq_global"  />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"

/*  Max. number of handles we accept (limited by hash-table size)            */
#define AMQ_HANDLE_MAX          255

/*  Handle states                                                            */
#define AMQ_HANDLE_CLOSED       0
#define AMQ_HANDLE_OPEN         1
#define AMQ_HANDLE_CLOSING      2
</public>

<private>
#include "amq_server_agent.h"

/*  This structure holds one queued message reference
    We collect these when the client uses HANDLE QUERY and we hold them
    in an array for easy access via HANDLE BROWSE                            */

typedef struct {
    amq_queue_t
        *queue;                         /*  Queue which holds the message    */
    amq_smessage_t
        *message;                       /*  Message, if non-persistent       */
    qbyte
        item_id;                        /*  Queue item id, if persistent     */
} AMQ_QUEUED_MESSAGE_REF;
</private>

<context>
    /*  References to parent objects                                         */
    smt_thread_handle_t
        *thread;                        /*  Parent thread                    */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    amq_connection_t
        *connection;                    /*  Parent connection                */
    amq_channel_t
        *channel;                       /*  Parent channel                   */
    qbyte
        client_id;                      /*  Parent client record             */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_db_t
        *ddb;                           /*  Deprecated database handle       */
    ipr_looseref_list_t
        *consumers;                     /*  List of consumers per handle     */
    amq_queue_t
        *queue;                         /*  If refers to actual queue        */
    amq_browser_array_t
        *browser_set;                   /*  Results of last HANDLE QUERY     */
    int
        state;                          /*  Handle state                     */
    dbyte
        service_type;                   /*  Service type for handle          */
    dbyte
        channel_id;                     /*  Channel for this handle          */
    Bool
        paused;                         /*  If TRUE, handle is paused        */
    ipr_shortstr_t
        dest_name;                      /*  Destination name prefix          */
    ipr_shortstr_t
        mime_type;                      /*  Default new message MIME type    */
    ipr_shortstr_t
        encoding;                       /*  Default new message encoding     */
</context>

<method name = "new">
    <doc>
    The caller should verify that the channel_id and service_id specified are
    valid.  The handle object does not check these at construction time.
    </doc>
    <argument name = "channel"  type = "amq_channel_t *"    >Parent channel</argument>
    <argument name = "command"  type = "amq_handle_open_t *">Handle open command</argument>

    /*  De-normalise from parent object, for simplicity of use               */
    self->channel     = channel;
    self->client_id   = channel->client_id;
    self->connection  = channel->connection;
    self->vhost       = channel->vhost;
    self->thread      = channel->thread;
    self->db          = channel->db;
    self->ddb         = channel->ddb;
    assert (self->db);                  /*  Database must be open            */
    assert (self->ddb);

    /*  Initialise other properties                                          */
    self->consumers    = ipr_looseref_list_new ();
    self->browser_set  = amq_browser_array_new ();
    self->state        = AMQ_HANDLE_OPEN;
    self->channel_id   = command->channel_id;
    self->service_type = command->service_type;
    ipr_shortstr_cpy (self->dest_name, command->dest_name);
    ipr_shortstr_cpy (self->mime_type, command->mime_type);
    ipr_shortstr_cpy (self->encoding,  command->encoding);

    if (self->service_type == AMQP_SERVICE_QUEUE)
        s_find_or_create_queue (&self, command->temporary);
    else {
        /*  ***TODO*** implement topics
         */
        $(selfname)_destroy (&self);
        amq_global_set_error (AMQP_ACCESS_REFUSED, "Access to requested destination is not allowed");
    }
</method>

<private name = "header">
static void
s_find_or_create_queue ($(selftype) ** p_self, Bool temporary);
</private>

<private name = "footer">
static void
s_find_or_create_queue ($(selftype) **p_self, Bool temporary)
{
    $(selftype)
        *self = *p_self;
    static qbyte
        queue_nbr = 0;                  /*  Temporary queue number           */

    /*  Find queue if named                                                  */
    if (self->dest_name && *self->dest_name) {
        self->queue = amq_queue_search (self->vhost->queue_hash, self->dest_name);
        if (self->queue == NULL
        &&  !temporary)
            $(selfname)_destroy (p_self);
    }
    else
    if (temporary)
        ipr_shortstr_fmt (self->dest_name, "tmp/%09ld", ++queue_nbr);

    /*  Create, if temporary and not already existing                        */
    if (temporary) {
        if (self->queue) {
            if (self->queue->dest->client_id != self->client_id) {
                coprintf ("$(selfname) E: temporary queue '%s' already in use", self->dest_name);
                self->queue = NULL;
                $(selfname)_destroy (p_self);
            }
        }
        else
            self->queue = amq_queue_new (
                self->dest_name,        /*  Name of queue                    */
                self->vhost,            /*  Parent virtual host              */
                self->client_id,        /*  Owning client id                 */
                TRUE,                   /*  Temporary queue?                 */
                NULL);                  /*  Configuration entry              */

        amq_server_agent_handle_created (self->thread, (dbyte) self->key, self->dest_name);
    }
}
</private>

<method name = "destroy">
    <local>
    ipr_looseref_t
        *consumer;
    </local>

    consumer = ipr_looseref_list_first (self->consumers);
    while (consumer) {
        amq_consumer_destroy ((amq_consumer_t **) &consumer->object);
        consumer = ipr_looseref_list_next (self->consumers, consumer);
    }
    ipr_looseref_list_destroy (&self->consumers);
    amq_browser_array_destroy (&self->browser_set);
</method>

<method name = "send" template = "function" >
    <doc>
    Processes a HANDLE SEND command.  Sends the message to the specified
    queue; transacted messages are held in memory until the client commits
    or rolls-back the channel.
    </doc>
    <argument name = "command" type = "amq_handle_send_t *" />
    <argument name = "message" type = "amq_smessage_t *"    />
    <local>
    amq_queue_t
        *queue;
    </local>

    /*  Look for the queue using the destination name specified              */
    queue = amq_queue_full_search (
        self->vhost->queue_hash, self->dest_name, command->dest_name);

    if (queue) {
        amq_queue_accept (queue, self->channel, message, self->channel->txn);
        if (queue->dirty)
            amq_queue_dispatch (queue);
    }
    else
        amq_global_set_error (AMQP_NOT_FOUND, "No such destination defined");
</method>

<method name = "consume" template = "function" >
    <argument name = "command" type = "amq_handle_consume_t *" />
    <local>
    amq_consumer_t
        *consumer;
    </local>

    consumer = amq_consumer_new (self, command);
    if (consumer) {
        ipr_looseref_new (self->consumers, consumer);
        amq_queue_dispatch (consumer->queue);
    }
</method>

<method name = "cancel" template = "function" >
    <argument name = "command" type = "amq_handle_cancel_t *" />
    /*  ***TODO*** implement topics
     */
    amq_global_set_error (AMQP_NOT_IMPLEMENTED, "Topics are not implemented");
</method>

<method name = "flow" template = "function" >
    <argument name = "command" type = "amq_handle_flow_t *" />
    <local>
    ipr_looseref_t
        *consumer;
    </local>

    self->paused = command->flow_pause;
    /*  If switching on, dispatch all queues for handle                      */
    if (!self->paused) {
        consumer = ipr_looseref_list_first (self->consumers);
        while (consumer) {
            amq_queue_dispatch (((amq_consumer_t *) consumer->object)->queue);
            consumer = ipr_looseref_list_next (self->consumers, consumer);
        }
    }
</method>

<method name = "unget" template = "function" >
    <argument name = "command" type = "amq_handle_unget_t *" />

    amq_dispatch_list_unget (self->channel->dispatched, command->message_nbr);
</method>

<method name = "query" template = "function" >
    <argument name = "command" type = "amq_handle_query_t *" />
    <local>
    amq_queue_t
        *queue;
    </local>

    /*  Look for the queue using the destination name specified              */
    queue = amq_queue_full_search (
        self->vhost->queue_hash, self->dest_name, command->dest_name);

    if (queue) {
        if (queue->opt_browsable) {
            amq_queue_query (queue, self->browser_set);
            amq_server_agent_handle_index (
                self->thread,
                (dbyte) self->key,
                0,
                amq_browser_array_strindex (self->browser_set, 8000));
        }
        else
            amq_global_set_error (AMQP_ACCESS_REFUSED, "Browsing not allowed on queue");
    }
    else
        amq_global_set_error (AMQP_NOT_FOUND, "No such destination defined");
</method>

<method name = "browse" template = "function" >
    <argument name = "command" type = "amq_handle_browse_t *" />
    <local>
    amq_browser_t
        *browser;
    </local>
    browser = amq_browser_array_fetch (self->browser_set, command->message_nbr);
    if (browser == NULL
    ||  amq_queue_browse (browser->queue, self, browser))
        amq_global_set_error (AMQP_MESSAGE_NOT_FOUND, "Message does not exist");
</method>

<method name = "selftest">
    <local>
    amq_vhost_table_t
        *vhosts;
    amq_vhost_t
        *vhost;

    amq_connection_open_t
        connection_open;
    amq_connection_t
        *connection;

    amq_channel_open_t
        channel_open;
    amq_channel_table_t
        *channels;
    amq_channel_t
        *channel;

    amq_handle_open_t
        handle_open;
    amq_handle_t
        *handle;
    amq_handle_table_t
        *handles;
    </local>

    /*  Initialise virtual host                                              */
    vhosts = amq_vhost_table_new (NULL);
    vhost  = amq_vhost_new (vhosts, "/test", "vh_test",
        ipr_config_new ("vh_test", AMQ_VHOST_CONFIG));
    assert (vhost);
    assert (vhost->db);
    assert (vhost->ddb);

    /*  Initialise connection                                                */
    ipr_shortstr_cpy (connection_open.virtual_path, "/test");
    ipr_shortstr_cpy (connection_open.client_name,  "selftest");
    connection = amq_connection_new (NULL);
    amq_connection_open (connection, vhosts, &connection_open);

    /*  Initialise channel                                                   */
    memset (&channel_open, 0, sizeof (channel_open));
    channel_open.channel_id = 1;
    channels   = amq_channel_table_new ();
    channel    = amq_channel_new (
        channels, channel_open.channel_id, connection, &channel_open);
    assert (channel);

    /*  Initialise handle                                                    */
    memset (&handle_open, 0, sizeof (handle_open));
    handle_open.channel_id   = 1;
    handle_open.service_type = 1;
    handle_open.handle_id    = 1;
    handles = amq_handle_table_new ();
    handle  = amq_handle_new (
        handles, handle_open.handle_id, channel, &handle_open);
    assert (handle);

    /*  Release resources                                                    */
    amq_handle_table_destroy  (&handles);
    amq_channel_table_destroy (&channels);
    amq_connection_destroy    (&connection);
    amq_vhost_destroy         (&vhost);
    amq_vhost_table_destroy   (&vhosts);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
