<?xml?>
<class
    name      = "amq_vhost"
    comment   = "Virtual host class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

<inherit class = "ipr_hash_str" />

<import class = "ipr_classes"  />
<import class = "amq_global"   />

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
    amq_queue_table_t
        *queue_hash;                    /*  Queues for this vhost            */
    ipr_looseref_list_t
        *queue_refs;                    /*  Queues, as list                  */
    ipr_shortstr_t
        directory;                      /*  Location for virtual host        */
    ipr_shortstr_t
        spooldir;                       /*  Message spool directory          */
    ipr_shortstr_t
        storedir;                       /*  Message store directory          */
</context>

<method name = "new">
    <argument name = "directory" type = "char *"/>
    <argument name = "config"    type = "ipr_config_t *"/>

    self->config = config;
    self->queue_hash = amq_queue_table_new ();
    self->queue_refs = ipr_looseref_list_new ();
    ipr_shortstr_cpy (self->directory, directory);

    coprintf ("I: configuring virtual host '%s'", self->key);
    s_config_workdir  (self);
    s_config_database (self);
    s_config_queues   (self);

    /*  ***TODO*** implement topics
    s_config_topics   (self);
     */
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
    amq_queue_table_destroy   (&self->queue_hash);
    ipr_looseref_list_destroy (&self->queue_refs);
    ipr_config_destroy        (&self->config);
    ipr_db_destroy            (&self->db);
    amq_db_destroy            (&self->ddb);
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatches all queues that have received new messages, i.e. have the
    'dirty' property.  All dirty queues are at the start of the vhost
    queue list - see amq_queue_accept ().
    </doc>
    <local>
    ipr_looseref_t
        *queue_ref;                     /*  Entry into queue list    */
    amq_queue_t
        *queue;                         /*  Queue itself             */
    </local>

    /*  Dispatch all dirty queues until we hit the last one          */
    queue_ref = ipr_looseref_list_first (self->queue_refs);
    while (queue_ref) {
        queue = (amq_queue_t *) queue_ref->object;
        if (queue->dirty) {
            amq_queue_dispatch (queue);
            queue_ref = ipr_looseref_list_next (self->queue_refs, queue_ref);
        }
        else
            break;
    }
</method>

<private name = "header">
static void s_config_workdir   ($(selftype) *self);
static void s_config_database  ($(selftype) *self);
static void s_config_queues    ($(selftype) *self);
</private>

<private name = "footer">
/*  Prepare vhost workdir and working directories                            */

static void
s_config_workdir ($(selftype) *self)
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
        ipr_config_locattr (self->config, "/config/directories", "data", "data"));

    /*  Connection to database                                               */
    self->db = ipr_db_new (database_dir);

    /*  ***TODO*** (iPR) rewrite BDB database access to use new model
    */
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

    /*  Wipe all temporary destinations                                      */
    dest = amq_db_dest_new ();
    while (amq_db_dest_fetch (self->ddb, dest, AMQ_DB_FETCH_GT) == 0) {
        if (dest->temporary)
            amq_db_dest_delete (self->ddb, dest);
    }
    amq_db_dest_destroy (&dest);
}


/*  Insert or find configured queues                                         */

static void
s_config_queues ($(selftype) *self)
{
    coprintf ("I: - configuring and checking persistent queues...");
    ipr_config_locate (self->config, "/config/queues/queue", NULL);
    while (self->config->located) {
        amq_queue_new (
            ipr_config_attr (self->config, "name", "unnamed"),
            self,                       /*  Parent virtual host              */
            0,                          /*  Owning client id, if any         */
            FALSE,                      /*  Temporary queue?                 */
            self->config);              /*  Configuration entry              */
        ipr_config_next (self->config);
    }
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
