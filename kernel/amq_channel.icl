<?xml?>
<class
    name      = "amq_channel"
    comment   = "Channel class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the AMQP CHANNEL commands.
</doc>

<inherit class = "ipr_hash_int" />

<import class = "amq_global" />

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
    int
        state;                          /*  Channel state                    */
    Bool
        transacted;                     /*  Transaction mode                 */
    Bool
        restartable;                    /*  Restartable mode                 */
    amq_dispatch_list_t
        *dispatch_list;                 /*  Messages dispatched & pending    */
    amq_smessage_list_t
        *transact_list;                 /*  Messages pending commit          */
    size_t
        transact_count;                 /*  Number of messages pending       */
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
    self->dispatch_list = amq_dispatch_list_new (self);
    self->transact_list = amq_smessage_list_new ();
    self->state         = AMQ_CHANNEL_OPEN;
    self->transacted    = command->transacted;
    self->restartable   = command->restartable;
</method>

<method name = "destroy">
    <local>
    amq_handle_t
        *handle;
    </local>
    /*  Rollback any open transaction                                        */
    if (self->transacted)
        self_rollback (self);

    /*  Restore pending messages if the client disconnected but not if the
        server is shutting down (BDB should not be called from a signal
        handler, which is where we are if shutting down).
     */
    if (!smt_signal_raised) {
        amq_dispatch_list_restore (self->dispatch_list);
        ipr_db_cursor_close (self->db);
    }
    amq_dispatch_list_destroy (&self->dispatch_list);
    amq_smessage_list_destroy (&self->transact_list);
    amq_smessage_destroy      (&self->message_in);

    /*  Destroy all handles for this channel                                 */
    for (table_idx = 0; table_idx &lt; AMQ_HANDLE_TABLE_MAXSIZE; table_idx++) {
        handle = self->connection->handles->item_table [table_idx];
        if (handle && handle != AMQ_HANDLE_DELETED && handle->channel == self)
            amq_handle_destroy (&handle);
    }
</method>

<method name = "ack" template = "function" >
    <argument name = "command" type = "amq_channel_ack_t *" />
    amq_dispatch_list_ack (self->dispatch_list, command->message_nbr);
</method>

<method name = "commit" template = "function" >
    <local>
    ipr_db_txn_t
        *txn;                           /*  Transaction for queue i/o        */
    </local>
    if (self->transacted) {
        txn = ipr_db_txn_new (self->db);
        amq_smessage_list_commit (self->transact_list, txn);
        amq_dispatch_list_commit (self->dispatch_list, txn);
        ipr_db_txn_commit  (txn);
        ipr_db_txn_destroy (&txn);
        ipr_db_log_flush   (self->db);
        self->transact_count = 0;

        //TODO: only purge if transaction finished successfully
        amq_dispatch_list_purge (self->dispatch_list);

        /*  Now dispatch the resulting messages, if any                      */
        amq_vhost_dispatch (self->vhost);
    }
    else
        amq_global_set_error (AMQP_COMMAND_INVALID, "Channel is not transacted - commit is not allowed");
</method>

<method name = "rollback" template = "function" >
    <local>
    ipr_db_txn_t
        *txn;                           /*  Transaction for queue i/o        */
    </local>
    if (self->transacted) {
        txn = ipr_db_txn_new (self->db);
        amq_smessage_list_rollback (self->transact_list);
        amq_dispatch_list_rollback (self->dispatch_list, txn);
        ipr_db_txn_commit  (txn);
        ipr_db_txn_destroy (&txn);
        ipr_db_log_flush   (self->db);
        self->transact_count = 0;
    }
    else
        amq_global_set_error (AMQP_COMMAND_INVALID, "Channel is not transacted - rollback is not allowed");
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
    </local>

    /*  Initialise virtual host                                              */
    vhosts = amq_vhost_table_new (NULL);
    vhost  = amq_vhost_new (vhosts, "/test", "vh_test",
        ipr_config_new ("vh_test", AMQ_VHOST_CONFIG));
    assert (vhost);

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

    /*  Release resources                                                    */
    amq_channel_table_destroy (&channels);
    amq_connection_destroy    (&connection);
    amq_vhost_destroy         (&vhost);
    amq_vhost_table_destroy   (&vhosts);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
