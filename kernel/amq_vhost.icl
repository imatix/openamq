<?xml?>
<class
    name      = "amq_vhost"
    comment   = "Virtual host class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
Defines a virtual host.  Virtual hosts are held in a hash table and accessed
by name.  Each virtual host has its own working directory for persistent
data, and holds the queues, topics, and subscriptions defined for that
virtual host.    
</doc>

<inherit class = "ipr_hash_str" />

<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<private>
#include "amq_server_agent.h"
</private>

<context>
    ipr_db_t
        *db;                            /*  Database for virtual host        */
    amq_db_t
        *ddb;                           /*  Deprecated database handle       */
    ipr_config_t
        *config;                        /*  Virtual host configuration       */
    ipr_shortstr_t
        directory;                      /*  Location for virtual host        */
    ipr_shortstr_t
        spooldir;                       /*  Message spool directory          */
    ipr_shortstr_t
        storedir;                       /*  Message store directory          */
    amq_dest_list_t
        *dest_list;                     /*  Destinations for dispatching     */
    amq_dest_table_t
        *queue_hash;                    /*  Queues for vhost, hash table     */
    amq_dest_table_t
        *topic_hash;                    /*  Topics for vhost, hash table     */
    amq_dest_table_t
        *subsc_hash;                    /*  Subscriptions for vhost          */
    //TODO: these will be moved to a hashed bit array for matching
    amq_subsc_list_t
        *subsc_list;                    /*  Subscriptions for vhost          */
</context>

<method name = "new">
    <argument name = "directory" type = "char *"/>
    <argument name = "config"    type = "ipr_config_t *"/>

    self->config     = config;
    self->dest_list  = amq_dest_list_new ();
    self->queue_hash = amq_dest_table_new ();
    self->topic_hash = amq_dest_table_new ();
    self->subsc_hash = amq_dest_table_new ();
    ipr_shortstr_cpy (self->directory, directory);

    coprintf ("I: configuring virtual host '%s'", self->key);
    s_configure_workdir  (self);
    s_configure_database (self);
    s_configure_queues   (self);
    s_configure_topics   (self);
    s_clean_destinations (self);
</method>

<method name = "destroy">
    /*  If we try to close a cursor during interrupt handling, BDB
        deadlocks.  So we will attempt to leave the cursor open and
        shutdown anyhow...
     */
    if (smt_signal_raised && self->db->db_cursor) {
        coprintf ("$(selfname) E: database cursor still open, attempting recovery");
        self->db->db_cursor = NULL;
    }
    amq_dest_table_destroy (&self->queue_hash);
    amq_dest_table_destroy (&self->topic_hash);
    amq_dest_table_destroy (&self->subsc_hash);
    amq_dest_list_destroy  (&self->dest_list);
    ipr_config_destroy     (&self->config);
    ipr_db_destroy         (&self->db);
    amq_db_destroy         (&self->ddb);
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatches all destination queues that have received new messages,
    i.e. have the 'dirty' property.  All dirty destination queues are at
    the start of the vhost destination list - see amq_queue_accept ().
    </doc>
    <local>
    amq_dest_t
        *dest;                           /*  Destination object              */
    </local>

    /*  Dispatch all dirty message queues, which come at start of list       */
    dest = amq_dest_list_first (self->dest_list);
    while (dest) {
        if (dest->queue->dirty) {
            amq_queue_dispatch (dest->queue);
            dest = amq_dest_list_next (self->dest_list, dest);
        }
        else
            break;
    }
</method>

<private name = "header">
static void s_configure_workdir  ($(selftype) *self);
static void s_configure_database ($(selftype) *self);
static void s_configure_queues   ($(selftype) *self);
static void s_configure_topics   ($(selftype) *self);
static void s_clean_destinations ($(selftype) *self);
</private>

<private name = "footer">
/*  Prepare vhost workdir and working directories                            */

static void
s_configure_workdir ($(selftype) *self)
{
    /*  Spool directory is for non-persistent or incomplete messages         */
    ipr_shortstr_fmt (self->spooldir, "%s/%s", self->directory,
        ipr_config_locattr (self->config, "/config/directories", "spool", "spool"));

    if (strlast (self->spooldir) == '/')
        strlast (self->spooldir) = 0;
    if (!file_is_directory (self->spooldir)) {
        coprintf ("I: - creating working directory '%s'", self->spooldir);
        make_dir (self->spooldir);
    }

    /*  Store directory is for messages saved to persistent storage          */
    ipr_shortstr_fmt (self->storedir, "%s/%s", self->directory,
        ipr_config_locattr (self->config, "/config/directories", "store", "store"));
    if (strlast (self->storedir) == '/')
        strlast (self->storedir) = 0;
    if (!file_is_directory (self->storedir)) {
        coprintf ("I: - creating working directory '%s'", self->storedir);
        make_dir (self->storedir);
    }
}

static void
s_configure_database ($(selftype) *self)
{
    ipr_shortstr_t
        database_dir;
    amq_db_client_t
        *client;
    amq_db_dest_t
        *dest;                          /*  Destination record               */

    /*  Prepare database working directory                                   */
    ipr_shortstr_fmt (database_dir, "%s/%s", self->directory,
        ipr_config_locattr (self->config, "/config/directories", "data", "data"));

    /*  Connection to database                                               */
    self->db = ipr_db_new (database_dir);

    //TODO: rewrite BDB access using new model
    ipr_shortstr_fmt (database_dir, "%s/%s", self->directory, "ddata");
    make_dir (database_dir);
    self->ddb = amq_db_new (database_dir);

    /*  Reset all client records to non-connected                            */
    client = amq_db_client_new ();
    while (amq_db_client_fetch (self->ddb, client, AMQ_DB_FETCH_GT) == 0) {
        client->connected = FALSE;
        amq_db_client_update (self->ddb, client);
    }
    amq_db_client_destroy (&client);

    /*  Disactivate all destinations                                         */
    dest = amq_db_dest_new ();
    while (amq_db_dest_fetch (self->ddb, dest, AMQ_DB_FETCH_GT) == 0) {
        dest->active = FALSE;
        amq_db_dest_update (self->ddb, dest);
    }
    amq_db_dest_destroy (&dest);
}


/*  Insert or find configured queues                                         */

static void
s_configure_queues ($(selftype) *self)
{
    char
        *dest_name;

    coprintf ("I: - configuring and checking configured queues...");
    ipr_config_locate (self->config, "/config/queues/queue", NULL);
    while (self->config->located) {
        dest_name = ipr_config_attr (self->config, "name", NULL);
        if (dest_name) {
            amq_dest_new (
                self->queue_hash,           /*  Queue table                      */
                self,                       /*  Parent virtual host              */
                AMQP_SERVICE_QUEUE,         /*  Message queue type               */
                FALSE,                      /*  Temporary destination?           */
                dest_name,                  /*  Destination name                 */
                0);                         /*  Owning client id, if any         */
        }
        else
            coprintf ("W: error in queue definitions - 'name' not defined");

        ipr_config_next (self->config);
    }
}


/*  Insert or find topics                                                    */

static void
s_configure_topics ($(selftype) *self)
{
    char
        *dest_name;

    coprintf ("I: - configuring and checking configured topics...");
    ipr_config_locate (self->config, "/config/topics/topic", NULL);
    while (self->config->located) {
        dest_name = ipr_config_attr (self->config, "name", NULL);
        if (dest_name) {
            amq_dest_new (
                self->topic_hash,           /*  Topic table                      */
                self,                       /*  Parent virtual host              */
                AMQP_SERVICE_TOPIC,         /*  Message topic type               */
                FALSE,                      /*  Temporary destination?           */
                dest_name,                  /*  Destination name                 */
                0);                         /*  Owning client id, if any         */
        }
        else
            coprintf ("W: error in topic definitions - 'name' not defined");

        ipr_config_next (self->config);
    }
}


/*  Remove all unused destinations                                           */

static void
s_clean_destinations ($(selftype) *self)
{
    amq_db_dest_t
        *dest;                          /*  Destination record               */

    /*  Delete all inactive destinations                                     */
    dest = amq_db_dest_new ();
    while (amq_db_dest_fetch (self->ddb, dest, AMQ_DB_FETCH_GT) == 0) {
        if (!dest->active) {
            //TODO: delete persistent file associated with destination
            amq_db_dest_delete (self->ddb, dest);
        }
    }
    amq_db_dest_destroy (&dest);
}
</private>

<method name = "selftest">
    <local>
    ipr_config_t
        *config;
    amq_vhost_table_t
        *vhosts;
    amq_vhost_t
        *vhost;
    </local>

    amq_vhost_animate (TRUE);
    amq_vhost_table_animate (TRUE);

    config = ipr_config_new (".", AMQ_SERVER_CONFIG);
    vhosts = amq_vhost_table_new (config);

    vhost = amq_vhost_search (vhosts, "/");
    assert (vhost);
    vhost = amq_vhost_search (vhosts, "/test");
    assert (vhost);

    amq_vhost_destroy       (&vhost);
    amq_vhost_table_destroy (&vhosts);
    ipr_config_destroy      (&config);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
