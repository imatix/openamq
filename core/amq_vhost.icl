<?xml?>
<class
    name      = "amq_vhost"
    comment   = "Virtual host class"
    version   = "1.0"
    copyright = "Copyright (c) 2004 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "ipr_hash_str" />
<option name = "nullify" value = "1" />

<import class = "ipr_classes"  />
<import class = "amq_db"       />
<import class = "amq_db_hack"  />
<import class = "amq_queue"    />
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
    amq_db_t
        *db;                            /*  Database for virtual host        */
    ipr_config_table_t
        *config;                        /*  Virtual host configuration       */
    amq_queue_table_t
        *queues;                        /*  Queues for this vhost            */
    ipr_shortstr_t
        directory;                      /*  Location for virtual host        */
    ipr_shortstr_t
        spooldir;                       /*  Message spool directory          */
    ipr_shortstr_t
        storedir;                       /*  Message store directory          */
</context>

<method name = "new">
    <argument name = "directory" type = "char *"               />
    <argument name = "config"    type = "ipr_config_table_t *" />

    self->config = config;
    self->queues = amq_queue_table_new ();
    ipr_shortstr_cpy (self->directory, directory);

    s_config_workdir  (self);
    s_config_database (self);
    s_config_queues   (self, "queue/name");
    /*  ***TODO*** implement topics
    s_config_topics   (self, "topic/name");
     */
</method>

<method name = "destroy">
    amq_queue_table_destroy  (&self->queues);
    ipr_config_table_destroy (&self->config);
    amq_db_destroy           (&self->db);
</method>

<private name = "header">
static void s_config_workdir   ($(selftype) *self);
static void s_config_database  ($(selftype) *self);
static void s_config_queues    ($(selftype) *self, char *key);
</private>

<private name = "footer">
/*  Prepare vhost workdir and working directories                            */

static void
s_config_workdir ($(selftype) *self)
{
    /*  Spool directory is for non-persistent or incomplete messages         */
    ipr_shortstr_fmt (self->spooldir, "%s/%s", self->directory,
        ipr_config_table_lookup (self->config, "workdir/spool", "spool"));
    if (strlast (self->spooldir) == '/')
        strlast (self->spooldir) = 0;
    if (!file_is_directory (self->spooldir)) {
        coprintf ("Creating working directory '%s'", self->spooldir);
        make_dir (self->spooldir);
    }

    /*  Store directory is for messages saved to persistent storage          */
    ipr_shortstr_fmt (self->storedir, "%s/%s", self->directory,
        ipr_config_table_lookup (self->config, "workdir/store", "store"));
    if (strlast (self->storedir) == '/')
        strlast (self->storedir) = 0;
    if (!file_is_directory (self->storedir)) {
        coprintf ("Creating working directory '%s'", self->storedir);
        make_dir (self->storedir);
    }
}

static void
s_config_database ($(selftype) *self)
{
    ipr_shortstr_t
        database_dir;
    amq_db_client_t
        *client;
    amq_db_dest_t
        *dest;

    /*  Prepare database working directory                                   */
    ipr_shortstr_fmt (database_dir, "%s/%s", self->directory,
        ipr_config_table_lookup (self->config, "workdir/data", "data"));
    if (!file_is_directory (database_dir)) {
        coprintf ("Creating working directory '%s'", database_dir);
        make_dir (database_dir);
    }
    /*  Connection to database                                               */
    self->db = amq_db_new (database_dir);

    /*  Reset all client records to non-connected                            */
    client = amq_db_client_new ();
    while (amq_db_client_fetch (self->db, client, AMQ_DB_FETCH_GT) == 0) {
        client->connected = FALSE;
        amq_db_client_update (self->db, client);
    }
    amq_db_client_destroy (&client);

    /*  Wipe all temporary destinations                                      */
    dest = amq_db_dest_new ();
    while (amq_db_dest_fetch (self->db, dest, AMQ_DB_FETCH_GT) == 0) {
        if (dest->temporary)
            amq_db_dest_delete (self->db, dest);
    }
    amq_db_dest_destroy (&dest);
}

/*  Insert or find configured queues                                         */

static void
s_config_queues ($(selftype) *self, char *key)
{
    ipr_config_t
        *config_entry;

    config_entry = ipr_config_search (self->config, key);
    while (config_entry) {
        amq_queue_new (self->queues, config_entry->value, self, 0, FALSE);
        config_entry = ipr_config_next (config_entry);
    }
}
</private>

<method name = "selftest">
    <local>
    ipr_config_table_t
        *config;
    amq_vhost_table_t
        *vhosts;
    amq_vhost_t
        *vhost;
    </local>

    amq_vhost_animate (TRUE);
    amq_vhost_table_animate (TRUE);

    config = ipr_config_table_new (".", AMQ_SERVER_CONFIG);
    vhosts = amq_vhost_table_new  (config);

    vhost = amq_vhost_search (vhosts, "/");
    ASSERT (vhost);
    vhost = amq_vhost_search (vhosts, "/test");
    ASSERT (vhost);

    amq_vhost_destroy        (&vhost);
    amq_vhost_table_destroy  (&vhosts);
    ipr_config_table_destroy (&config);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
