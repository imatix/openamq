<?xml?>
<class
    name      = "amq_handle"
    comment   = "Destination handle class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "ipr_hash_int"  />
<option name = "nullify" value = "1" />

<import class = "amq_channel"  />
<import class = "amq_consumer" />
<import class = "amq_smessage" />
<import class = "ipr_classes"  />

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
    amq_looseref_list_t
        *consumers;                     /*  List of consumers per handle     */
    amq_queue_t
        *queue;                         /*  If refers to actual queue        */
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
    ASSERT (self->db);                  /*  Database must be open            */
    ASSERT (self->ddb);

    /*  Initialise other properties                                          */
    self->consumers    = amq_looseref_list_new ();
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
        coprintf ("amq_handle: invalid service type '%d' specified",
            self->service_type);
        $(selfname)_destroy (&self);
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

    if (temporary) {
        if (self->dest_name == NULL || *self->dest_name == 0)
            ipr_shortstr_fmt (self->dest_name, "tmp/%09ld", ++queue_nbr);
        self->queue = amq_queue_new (self->dest_name, self->vhost, self->client_id, TRUE);
        if (self->queue)
            amq_server_agent_handle_created
                (self->thread, (dbyte) self->key, self->dest_name);
        else
            $(selfname)_destroy (p_self);
    }
    else
        /*  Find queue if fully named                                        */
        self->queue = amq_queue_search (self->vhost->queues, self->dest_name);
}
</private>

<method name = "destroy">
    <local>
    amq_looseref_t
        *consumer;
    </local>

    consumer = amq_looseref_list_first (self->consumers);
    while (consumer) {
        amq_consumer_destroy ((amq_consumer_t **) &consumer->object);
        consumer = amq_looseref_list_next (self->consumers, consumer);
    }
    amq_looseref_list_destroy (&self->consumers);
</method>

<method name = "send" template = "function" >
    <argument name = "command"  type = "amq_handle_send_t *" />
    <argument name = "message"  type = "amq_smessage_t *"    />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    <local>
    amq_queue_t
        *queue;
    </local>

    queue = amq_queue_full_search (
        self->vhost->queues, self->dest_name, command->dest_name);
    if (queue)
        amq_queue_dispatch (queue, message);
    else {
        *reply_text = "No such destination defined";
        rc = AMQP_NOT_FOUND;            /*  Destination not found            */
    }
</method>

<method name = "consume" template = "function" >
    <argument name = "command"  type = "amq_handle_consume_t *" />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    <local>
    amq_consumer_t
        *consumer;
    </local>

    consumer = amq_consumer_new (self, command);
    if (consumer) {
        amq_looseref_new (self->consumers, consumer);
        amq_queue_dispatch (consumer->queue, NULL);
    }
    else {
        *reply_text = "No such destination defined";
        rc = AMQP_NOT_FOUND;            /*  Destination not found            */
    }
</method>

<method name = "cancel" template = "function" >
    <argument name = "command"  type = "amq_handle_cancel_t *" />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    /*  ***TODO*** implement topics
     */
    *reply_text = "Topics are not implemented";
    rc = AMQP_NOT_IMPLEMENTED;
</method>

<method name = "flow" template = "function" >
    <argument name = "command"  type = "amq_handle_flow_t *" />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    self->paused = command->flow_pause;
</method>

<method name = "unget" template = "function" >
    <argument name = "command"  type = "amq_handle_unget_t *" />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    rc = amq_dispatch_list_unget (self->channel->dispatched, command->message_nbr);
    if (rc)
        *reply_text = "No such message held";
</method>

<method name = "query" template = "function" >
    <argument name = "command"  type = "amq_handle_query_t *" />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    /*  ***TODO*** implement HANDLE QUERY
     */
    *reply_text = "Function is not implemented";
    rc = AMQP_NOT_IMPLEMENTED;
</method>

<method name = "browse" template = "function" >
    <argument name = "command"  type = "amq_handle_browse_t *" />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    /*  ***TODO*** implement HANDLE BROWSE
     */
    *reply_text = "Function is not implemented";
    rc = AMQP_NOT_IMPLEMENTED;
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

    char
        *reply_text;
    </local>

    /*  Initialise virtual host                                              */
    vhosts = amq_vhost_table_new (NULL);
    vhost  = amq_vhost_new (vhosts, "/test", "vh_test",
        ipr_config_table_new ("vh_test", AMQ_VHOST_CONFIG));
    ASSERT (vhost);
    ASSERT (vhost->db);
    ASSERT (vhost->ddb);

    /*  Initialise connection                                                */
    ipr_shortstr_cpy (connection_open.virtual_path, "/test");
    ipr_shortstr_cpy (connection_open.client_name,  "selftest");
    connection = amq_connection_new (NULL);
    amq_connection_open (connection, vhosts, &connection_open, &reply_text);

    /*  Initialise channel                                                   */
    memset (&channel_open, 0, sizeof (channel_open));
    channel_open.channel_id = 1;
    channels   = amq_channel_table_new ();
    channel    = amq_channel_new (
        channels, channel_open.channel_id, connection, &channel_open);
    ASSERT (channel);

    /*  Initialise handle                                                    */
    memset (&handle_open, 0, sizeof (handle_open));
    handle_open.channel_id   = 1;
    handle_open.service_type = 1;
    handle_open.handle_id    = 1;
    handles = amq_handle_table_new ();
    handle  = amq_handle_new (
        handles, handle_open.handle_id, channel, &handle_open);
    ASSERT (handle);

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
