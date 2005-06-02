<?xml?>
<class
    name      = "amq_handle"
    comment   = "Destination handle class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the AMQP HANDLE commands.
</doc>

<inherit class = "ipr_hash_int"  />

<import class = "amq_global" />

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

<private name = "header">
#include "amq_server_agent.h"           /*  Server agent methods             */
</private>

<context>
    /*  References to parent objects                                         */
    smt_thread_handle_t
        *thread;                        /*  Parent thread                    */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    amq_channel_t
        *channel;                       /*  Parent channel                   */
    qbyte
        client_id;                      /*  Parent client record             */

    /*  Object properties                                                    */
    amq_consumer_by_handle_t
        *consumers;                     /*  Consumers for this handle        */
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
    <local>
    static qbyte
        temp_count = 0;                 /*  Temporary destination number     */
    ipr_shortstr_t
        dest_name;                      /*  Temporary destination name       */
    amq_dest_t
        *dest;                          /*  New temporary destination        */
    </local>

    /*  De-normalise from parent object, for simplicity of use               */
    self->channel     = channel;
    self->client_id   = channel->client_id;
    self->vhost       = channel->vhost;
    self->thread      = channel->thread;

    /*  Initialise other properties                                          */
    self->consumers    = amq_consumer_by_handle_new ();
    self->browser_set  = amq_browser_array_new ();
    self->state        = AMQ_HANDLE_OPEN;
    self->channel_id   = command->channel_id;
    self->service_type = command->service_type;
    ipr_shortstr_cpy (self->mime_type, command->mime_type);
    ipr_shortstr_cpy (self->encoding,  command->encoding);

    /*  Create temporary queue if necessary                                  */
    if (self->service_type == AMQP_SERVICE_QUEUE && command->temporary) {
        ipr_shortstr_fmt (dest_name, "tmp-%09ld", ++temp_count);
        dest = amq_dest_new (
            self->vhost->queue_hash,
            self->vhost,
            AMQP_SERVICE_QUEUE,
            TRUE,
            dest_name,
            self->client_id);
        amq_server_agent_handle_created (self->thread, (dbyte) key, dest->key);
    }
</method>

<method name = "destroy">
    amq_consumer_by_handle_destroy (&self->consumers);
    amq_browser_array_destroy (&self->browser_set);
</method>

<method name = "send" template = "function" >
    <doc>
    Processes a HANDLE SEND command.  Sends the message to the specified
    destination queue; transacted messages are held in memory until the
    client commits or rolls-back the channel. The caller should destroy
    the message after calling this method.
    </doc>
    <argument name = "command" type = "amq_handle_send_t *" />
    <argument name = "message" type = "amq_smessage_t *"    />
    <local>
    amq_dest_t
        *dest;
    </local>
    if (self->service_type == AMQP_SERVICE_QUEUE)
        dest = amq_dest_search (self->vhost->queue_hash, command->dest_name);
    else {
        dest = self->vhost->topic_dest;
        amq_match_table_check_topic (
            self->vhost->match_topics,
            command->dest_name,
            self->vhost->subscr_list);
    }
    if (dest) {
        /*  Stamp message with destination name and save to queue            */
        ipr_shortstr_cpy   (message->dest_name, command->dest_name);
        amq_queue_accept   (dest->queue, self->channel, message, command->immediate, NULL);
        amq_vhost_dispatch (self->vhost);
    }
    else
        amq_global_set_error (AMQP_NOT_FOUND, "No such destination defined");
</method>

<method name = "consume" template = "function" >
    <doc>
    Processes a HANDLE CONSUME command.  The consumer class handles the
    actual queue and topic consumer implementation.  For queues, the command
    must refer to an existing destination unless the dynamic flag is set; for
    topics it may refer to a topic name pattern.
    </doc>
    <argument name = "command" type = "amq_handle_consume_t *" />
    amq_consumer_new (self, command);
</method>

<method name = "flow" template = "function" >
    <argument name = "command" type = "amq_handle_flow_t *" />
    <local>
    amq_consumer_t
        *consumer;
    </local>
    self->paused = command->flow_pause;
    consumer = amq_consumer_by_handle_first (self->consumers);
    while (consumer) {
        /*  Move consumer to queue active/inactive list as needed            */
        if (self->paused)
            amq_consumer_deactivate (consumer);
        else {
            amq_consumer_activate  (consumer);
            amq_queue_pre_dispatch (consumer->queue);
        }
        consumer = amq_consumer_by_handle_next (self->consumers, consumer);
    }
</method>

<method name = "unget" template = "function" >
    <argument name = "command" type = "amq_handle_unget_t *" />
    amq_dispatch_list_unget (self->channel->dispatch_list, command->message_nbr);
</method>

<method name = "query" template = "function" >
    <argument name = "command" type = "amq_handle_query_t *" />
    <local>
    amq_dest_t
        *dest;
    </local>

    if (self->service_type == AMQP_SERVICE_QUEUE) {
        /*  Look for the queue using the destination name specified         */
        dest = amq_dest_search (self->vhost->queue_hash, command->dest_name);
        if (dest) {
            if (!dest->opt_protected) {
                amq_queue_query (dest->queue, self->browser_set);
                amq_server_agent_handle_index (
                    self->thread,
                    (dbyte) self->key,
                    0,
                    amq_browser_array_strindex (self->browser_set, 8000));
            }
            else
                amq_global_set_error (AMQP_ACCESS_REFUSED, "Browsing not allowed on destination");
        }
        else
            amq_global_set_error (AMQP_NOT_FOUND, "No such destination defined");
    }
    else
        amq_global_set_error (AMQP_COMMAND_INVALID, "Cannot query topics");
</method>

<method name = "browse" template = "function" >
    <argument name = "command" type = "amq_handle_browse_t *" />
    <local>
    amq_browser_t
        *browser;
    </local>

    if (self->service_type == AMQP_SERVICE_QUEUE) {
        browser = amq_browser_array_fetch (self->browser_set, command->message_nbr);
        if (browser == NULL
        ||  amq_queue_browse (browser->queue, self, browser))
            amq_global_set_error (AMQP_MESSAGE_NOT_FOUND, "Message does not exist");
    }
    else
        amq_global_set_error (AMQP_COMMAND_INVALID, "Cannot browse topics");
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
    handle_open.service_type = AMQP_SERVICE_QUEUE;
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
