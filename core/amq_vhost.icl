<?xml?>
<class
    name      = "amq_vhost"
    comment   = "Virtual host class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

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
    amq_dest_table_t
        *dest_hash;                     /*  Destinations for this vhost      */
    ipr_looseref_list_t
        *dest_refs;                    /*  Destinations, as list            */
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
    self->dest_hash = amq_dest_table_new ();
    self->dest_refs = ipr_looseref_list_new ();
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
    amq_dest_table_destroy    (&self->dest_hash);
    ipr_looseref_list_destroy (&self->dest_refs);
    ipr_config_destroy        (&self->config);
    ipr_db_destroy            (&self->db);
    amq_db_destroy            (&self->ddb);
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatches all queues that have received new messages, i.e. have the
    'dirty' property.  All dirty queues are at the start of the vhost
    dest list - see amq_dest_accept ().
    </doc>
    <local>
    ipr_looseref_t
        *dest_ref;                      /*  Entry into dest list     */
    amq_dest_t
        *dest;                          /*  Destination object       */
    </local>

    /*  Dispatch all dirty queues until we hit the last one          */
    dest_ref = ipr_looseref_list_first (self->dest_refs);
    while (dest_ref) {
        dest = (amq_dest_t *) dest_ref->object;
        if (dest->dirty) {
            amq_dest_dispatch (dest);
            dest_ref = ipr_looseref_list_next (self->dest_refs, dest_ref);
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
}


/*  Insert or find configured queues                                         */

static void
s_config_queues ($(selftype) *self)
{
    char
        *external_name;
    ipr_shortstr_t
        internal_name;
        
    coprintf ("I: - configuring and checking persistent queues...");
    ipr_config_locate (self->config, "/config/queues/queue", NULL);
    while (self->config->located) {
        external_name = ipr_config_attr (self->config, "name", "unnamed");
        amq_dest_map_name (internal_name, external_name, AMQ_DEST_TYPE_QUEUE);
        amq_dest_new (
            internal_name,              /*  Mapped key/filename              */
            self,                       /*  Parent virtual host              */
            AMQ_DEST_TYPE_QUEUE,        /*  Destination type                 */
            external_name,              /*  External dest name               */
            0);                         /*  Owning client id, if any         */
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
