<?xml?>
<class
    name      = "amq_channel"
    comment   = "Channel class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "ipr_hash_int" />
<option name = "nullify" value = "1" />

<import class = "amq_connection" />
<import class = "amq_smessage"   />
<import class = "amq_dispatch"   />
<import class = "ipr_classes"    />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"

/*  Max. number of channels we accept (limited by hash-table size)           */
#define AMQ_CHANNEL_MAX         255

/*  Channel states                                                           */
#define AMQ_CHANNEL_CLOSED      0
#define AMQ_CHANNEL_OPEN        1
#define AMQ_CHANNEL_CLOSING     2
</public>

<context>
    /*  References to parent objects                                         */
    smt_thread_handle_t
        *thread;                        /*  Parent thread                    */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    amq_connection_t
        *connection;                    /*  Parent connection                */
    qbyte
        client_id;                      /*  Parent client record             */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_db_t
        *ddb;                           /*  Deprecated database handle       */
    ipr_db_txn_t
        *txn;                           /*  Transaction object               */
    int
        state;                          /*  Channel state                    */
    Bool
        transacted;                     /*  Transaction mode                 */
    Bool
        restartable;                    /*  Restartable mode                 */
    amq_dispatch_list_t
        *dispatched;                    /*  Messages dispatched & pending    */
    qbyte
        message_nbr;                    /*  Message numbering                */
    amq_smessage_t
        *message_in;                    /*  Incoming message                 */
    amq_smessage_t
        *message_out;                   /*  Outgoing message                 */
</context>

<method name = "new">
    <argument name = "connection" type = "amq_connection_t *">Parent thread</argument>
    <argument name = "command"    type = "amq_channel_open_t *" />
    /*  De-normalise from parent object, for simplicity of use               */
    self->connection  = connection;
    self->client_id   = connection->client_id;
    self->vhost       = connection->vhost;
    self->thread      = connection->thread;
    self->db          = connection->db;
    self->ddb         = connection->ddb;

    /*  Initialise other properties                                          */
    self->dispatched  = amq_dispatch_list_new (self);
    self->state       = AMQ_CHANNEL_OPEN;
    self->transacted  = command->transacted;
    self->restartable = command->restartable;

    if (self->transacted)
        self->txn = ipr_db_txn_new (self->db);
</method>

<method name = "destroy">
    if (self->transacted)
        ipr_db_txn_destroy (&self->txn);

    amq_dispatch_list_destroy (&self->dispatched);
    /*  Destroy all handles for this channel                                 */
    for (table_idx = 0; table_idx &lt; AMQ_HANDLE_TABLE_MAXSIZE; table_idx++) {
        if (self->connection->handles->item_table [table_idx]
        &&  self->connection->handles->item_table [table_idx]->channel == self)
            amq_handle_destroy (&self->connection->handles->item_table [table_idx]);
    }
</method>

<method name = "ack" template = "function" >
    <argument name = "command"  type = "amq_channel_ack_t *" />
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    amq_dispatch_list_ack (self->dispatched, command->message_nbr);
</method>

<method name = "commit" template = "function" >
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    if (self->transacted) {
        amq_dispatch_list_commit (self->dispatched);
        ipr_db_log_flush  (self->db);
        ipr_db_txn_commit (self->txn);
    }
    else {
        if (reply_text)
            *reply_text = "Channel is not transacted - commit is not allowed";
        rc = AMQP_COMMAND_INVALID;
    }
</method>

<method name = "rollback" template = "function" >
    <argument name = "reply_text" type = "char **">Returned error message, if any</argument>
    if (self->transacted) {
        amq_dispatch_list_rollback (self->dispatched);
        ipr_db_txn_rollback (self->txn);
    }
    else {
        if (reply_text)
            *reply_text = "Channel is not transacted - rollback is not allowed";
        rc = AMQP_COMMAND_INVALID;
    }
</method>

<method name = "selftest">
    <local>
    amq_vhost_table_t
        *vhosts;
    amq_vhost_t
        *vhost;

    amq_connection_t
        *connection;
    amq_connection_open_t
        connection_open;

    amq_channel_open_t
        channel_open;
    amq_channel_table_t
        *channels;
    amq_channel_t
        *channel;
    char
        *reply_text;
    </local>

    /*  Initialise virtual host                                              */
    vhosts = amq_vhost_table_new (NULL);
    vhost  = amq_vhost_new (vhosts, "/test", "vh_test",
        ipr_config_table_new ("vh_test", AMQ_VHOST_CONFIG));
    ASSERT (vhost);

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

    /*  Release resources                                                    */
    amq_channel_table_destroy (&channels);
    amq_connection_destroy    (&connection);
    amq_vhost_destroy         (&vhost);
    amq_vhost_table_destroy   (&vhosts);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>




