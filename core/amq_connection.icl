<?xml?>
<class
    name      = "amq_connection"
    comment   = "Connection class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "icl_alloc_cache" />
<option name = "nullify" value = "1" />

<import class = "smt_thread_handle" />
<import class = "amq_vhost"   />
<import class = "amq_channel" />
<import class = "amq_handle"  />
<import class = "ipr_classes" />

<public name = "header">
#include "amq_core.h"
</public>

<context>
    /*  References to parent objects                                         */
    smt_thread_handle_t
        *thread;                        /*  Parent thread                    */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    qbyte
        client_id;                      /*  Parent client record             */

    /*  Object properties                                                    */
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_db_t
        *ddb;                           /*  Deprecated database handle       */
    amq_channel_table_t
        *channels;                      /*  Active channels                  */
    amq_handle_table_t
        *handles;                       /*  Active handles                   */
    size_t
        frame_max;                      /*  Negotiated frame limit           */
    dbyte
        channel_max;                    /*  Negotiated channel limit         */
    dbyte
        handle_max;                     /*  Negotiated handle limit          */
    dbyte
        heartbeat;                      /*  Connection heartbeat             */
    ipr_shortstr_t
        client_name;                    /*  Client identifier                */
</context>

<method name = "new">
    <argument name = "thread"  type = "smt_thread_t *">Parent thread</argument>
    if (thread)
        self->thread  = smt_thread_handle_new (thread);
    self->frame_max   = AMQ_BUCKET_SIZE;
    self->channel_max = AMQ_CHANNEL_MAX;
    self->handle_max  = AMQ_HANDLE_MAX;
    self->channels    = amq_channel_table_new ();
    self->handles     = amq_handle_table_new ();
</method>

<method name = "destroy">
    uint
        table_idx;

    /*  Destroy all handles for this connection                              */
    for (table_idx = 0; table_idx &lt; AMQ_HANDLE_TABLE_MAXSIZE; table_idx++)
        if (self->handles->item_table [table_idx])
            amq_handle_destroy (&self->handles->item_table [table_idx]);

    /*  Destroy all channels for this connection                             */
    for (table_idx = 0; table_idx &lt; AMQ_CHANNEL_TABLE_MAXSIZE; table_idx++)
        if (self->channels->item_table [table_idx])
            amq_channel_destroy (&self->channels->item_table [table_idx]);

    amq_channel_table_destroy (&self->channels);
    amq_handle_table_destroy  (&self->handles);
    smt_thread_handle_destroy (&self->thread);

    /*  If not closed, close now                                             */
    $(selfname)_close (self);
</method>

<method name = "tune" template = "function" >
    <argument name = "command" type = "amq_connection_tune_t *" />
    /*  Lower limits if client asks for that                                 */
    if (self->frame_max   > command->frame_max)
        self->frame_max   = command->frame_max;
    if (self->channel_max > command->channel_max)
        self->channel_max = command->channel_max;
    if (self->handle_max  > command->handle_max)
        self->handle_max  = command->handle_max;

    self->heartbeat = command->heartbeat;
</method>

<method name = "open" template = "function" >
    <argument name = "vhosts"   type = "amq_vhost_table_t *" />
    <argument name = "command"  type = "amq_connection_open_t *" />
    <argument name = "reply_text" type = "char **" />
    <local>
    amq_db_client_t
        *client;
    </local>

    self->vhost = amq_vhost_search (vhosts, command->virtual_path);
    if (self->vhost) {
        self->db  = self->vhost->db;
        self->ddb = self->vhost->ddb;
        ipr_shortstr_cpy (self->client_name, command->client_name);

        /*  Create or update client instance                                 */
        client = amq_db_client_new ();
        strcpy (client->name, self->client_name);
        if (amq_db_client_fetch_byname (self->ddb, client, AMQ_DB_FETCH_EQ) == 0) {
            if (client->connected) {
                *reply_text = "Client already has an active session";
                rc = AMQP_CLIENT_ACTIVE;
            }
            else {
                client->connected = TRUE;
                amq_db_client_update (self->ddb, client);
                self->client_id = client->id;
            }
        }
        else {
            client->connected = TRUE;
            amq_db_client_insert (self->ddb, client);
            self->client_id = client->id;
        }
        amq_db_client_destroy (&client);
    }
    else {
        *reply_text = "Virtual path does not exist";
        rc = AMQP_INVALID_PATH;
    }
</method>

<method name = "close" template = "function" >
    <local>
    amq_db_client_t
        *client;
    </local>
    if (self->client_id) {
        client = amq_db_client_new ();
        client->id = self->client_id;
        amq_db_client_fetch (self->ddb, client, AMQ_DB_FETCH_EQ);
        client->connected = FALSE;
        amq_db_client_update  (self->ddb, client);
        amq_db_client_destroy (&client);
        self->client_id = 0;
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

    char
        *reply_text;
    </local>

    /*  Initialise virtual host                                              */
    vhosts = amq_vhost_table_new (NULL);
    vhost  = amq_vhost_new (vhosts, "/test", "vh_test",
        ipr_config_new ("vh_test", AMQ_VHOST_CONFIG));
    ASSERT (vhost);

    /*  Initialise connection                                                */
    ipr_shortstr_cpy (connection_open.virtual_path, "/test");
    ipr_shortstr_cpy (connection_open.client_name,  "selftest");
    connection = amq_connection_new (NULL);
    amq_connection_open (connection, vhosts, &connection_open, &reply_text);

    /*  Release resources                                                    */
    amq_connection_destroy  (&connection);
    amq_vhost_destroy       (&vhost);
    amq_vhost_table_destroy (&vhosts);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
