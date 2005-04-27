<?xml?>
<class
    name      = "amq_dest"
    comment   = "Destination class"
    version   = "1.2"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class defines a destination, which can be a queue, a topic, or a
subscription (an internal destination).  Every destination is mapped to
an amq_queue object which acts as a container for the destination's
messages.  Temporary destinations are created dynamically as the server
runs, and deleted when the server restarts.  Persistent destinations are
created from the configuration file.

Destinations are named and contained in hash tables.  Destinations are
parameterised from the virtual host configuration file.  Each destination
is recorded in the db_dest database table so that we can administer these
at restart time.  Note that all destination message handling is done by
amq_queue. Note that 'queue' is used to cover two concepts: the
point-to-point service type, and the amq_queue class used to store and
forward messages.
</doc>

<inherit class = "ipr_list_item" />
<inherit class = "ipr_hash_str" />
<option name = "bigtable" value = "1" />

<import class = "amq_global" />
<import class = "amq_db" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<private>
#include "amq_server_agent.h"           /*  Server agent methods             */
</private>

<context>
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */

    /*  Object properties                                                    */
    amq_queue_t
        *queue;                         /*  Message queue for destination    */
    amq_db_dest_t
        *db_dest;                       /*  Destination record               */
    int
        service_type;                   /*  AMQP service - queue or topic    */
    Bool
        temporary;                      /*  Created/deleted dynamically      */
    qbyte
        client_id;                      /*  Owner, if any                    */

    /*  Mesgq options, loaded from dest configuration in amq_vhost.cfg       */
    size_t
        opt_min_consumers;              /*  Minimum required consumers       */
    size_t
        opt_max_consumers;              /*  Maximum allowed consumers        */
    size_t
        opt_memory_queue_max;           /*  Max. size of memory queue        */
    size_t
        opt_max_messages;               /*  Max. allowed messages            */
    size_t
        opt_max_message_size;           /*  Max. allowed message size        */
    size_t
        opt_priority_levels;            /*  Number of priority levels        */
    Bool
        opt_protected;                  /*  Browsing disallowed?             */
    Bool
        opt_persistent;                 /*  Force persistent messages?       */
    Bool
        opt_auto_purge;                 /*  Purge when creating?             */
    size_t
        opt_record_size;                /*  Physical record size             */
    size_t
        opt_extent_size;                /*  Records per file extent          */
    size_t
        opt_block_size;                 /*  Size of block, in record         */
</context>

<method name = "new">
    <argument name = "vhost"        type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "service type" type = "int"          >AMQP service type</argument>
    <argument name = "temporary"    type = "Bool"         >Temporary destination?</argument>
    <argument name = "name"         type = "char *"       >Destination name</argument>
    <argument name = "client id"    type = "qbyte"        >Owner, if any</argument>
    <dismiss argument = "key"   value = "name" />
    <local>
    ipr_shortstr_t
        filename;
    </local>
    self->vhost        = vhost;
    self->service_type = service_type;
    self->temporary    = temporary;
    self->client_id    = client_id;

    s_get_configuration (self, name);
    ipr_shortstr_fmt (filename, "%s-%s",
        service_type == AMQP_SERVICE_QUEUE? "queue":
        service_type == AMQP_SERVICE_TOPIC? "topic": "subscr", name);

    amq_dest_list_queue (vhost->dest_list, self);
    self->queue   = amq_queue_new   (filename, vhost, self);
    self->db_dest = amq_db_dest_new ();

    /*  Fetch existing dest if caller specified a name                       */
    ipr_shortstr_cpy (self->db_dest->name, name);
    self->db_dest->type = service_type;
    amq_db_dest_fetch_byname (vhost->ddb, self->db_dest, AMQ_DB_FETCH_EQ);

    /*  If new destination, create it now                                    */
    self->db_dest->active    = TRUE;
    self->db_dest->client_id = client_id;
    self->db_dest->temporary = temporary;
    if (self->db_dest->id == 0)
        amq_db_dest_insert (vhost->ddb, self->db_dest);
    else
        amq_db_dest_update (vhost->ddb, self->db_dest);
</method>

<method name = "destroy">
    amq_db_dest_destroy (&self->db_dest);
    amq_queue_destroy   (&self->queue);
</method>

<method name = "search">
    <argument name = "prefix" type = "char *">Destination name prefix</argument>
    <argument name = "suffix" type = "char *">Destination name suffix</argument>
    <dismiss argument = "key" value = "dest_name" />
    <local>
    ipr_shortstr_t
        dest_name;
    </local>
    <header>
    ipr_shortstr_fmt (dest_name, "%s%s", prefix, suffix);
    </header>
</method>

<method name = "resolve" return = "self">
    <doc>
    Locates the destination, creates it if it's a temporary destination.  If
    the specified destination can't be found or created, returns null.  Else
    returns the reference to the destination object.
    </doc>
    <argument name = "handle"    type = "amq_handle_t *">Parent handle</argument>
    <argument name = "temporary" type = "Bool"          >Temporary destination?</argument>
    <declare name = "self" type = "$(selftype) *" default = "NULL">Object reference</declare>
    <local>
    static qbyte
        temp_count = 0;                 /*  Temporary destination number     */
    amq_dest_table_t
        *table;                         /*  Hash table for destination       */
    </local>

    if (handle->service_type == AMQP_SERVICE_QUEUE)
        table = handle->vhost->queue_hash;
    else
        table = handle->vhost->topic_hash;

    /*  Look for destination as specified                                    */
    if (strused (handle->dest_name))
        self = self_search (table, handle->dest_name, "");

    /*  The 'temporary' flag asks to create & own the destination            */
    if (temporary) {
        if (self) {
            if (self->client_id != handle->client_id) {
                amq_global_set_error (AMQP_NOT_FOUND, "Temporary destination already in use");
                self = NULL;            /*  Failed...                        */
            }
        }
        else {
            if (strnull (handle->dest_name))
                ipr_shortstr_fmt (handle->dest_name, "tmp-%09ld", ++temp_count);
            amq_dest_new (
                table,
                handle->vhost,
                handle->service_type,
                temporary,
                handle->dest_name,
                handle->client_id);
            amq_server_agent_handle_created (
                handle->thread, (dbyte) handle->key, handle->dest_name);
        }
    }
</method>

<private name = "header">
static void
    s_get_configuration ($(selftype) *self, char *name);
static void
    s_load_dest_properties ($(selftype) *self,
                            char *dest_path, char *dest_name,
                            char *template_path, char *template_name);
static void
    s_load_property_set ($(selftype) *self, ipr_config_t *config);
</private>

<private name = "footer">
/*  Load dest options from virtual host configuration values                 */

static void
s_get_configuration ($(selftype) *self, char *name)
{
    if (self->service_type == AMQP_SERVICE_QUEUE) {
        if (self->temporary)
            s_load_dest_properties (self,
                NULL, NULL,
                "/config/template/queue", "temporary");
        else
            s_load_dest_properties (self,
                "/config/queues/queue",    name,
                "/config/template/queue", "default");
    }
    else
    if (self->service_type == AMQP_SERVICE_TOPIC) {
        if (self->temporary)
            s_load_dest_properties (self,
                NULL, NULL,
                "/config/template/topic", "temporary");
        else
            s_load_dest_properties (self,
                "/config/topics/topic",    name,
                "/config/template/topic", "default");
    }
    else
    if (self->service_type == AMQP_SERVICE_SUBSCR) {
        s_load_dest_properties (self,
            NULL, NULL,
            "/config/template/subscription", "default");
    }
}


/*  -----------------------------------------------------------------------
    Loads the properties for a destination, looking in various places:
    - first, the template, if any
        - specified by the "template" attribute of the destination
        - or the template name specified
    - second, the destination itself
        - specified by a primary path and a name
 */

static void
s_load_dest_properties (
    $(selftype) *self,                  /*  Destination to configure         */
    char *dest_path,                    /*  Primary config path, or NULL     */
    char *dest_name,                    /*  Destination name, or NULL        */
    char *template_path,                /*  Template path                    */
    char *template_name)                /*  Default template name            */
{
    ipr_config_t
        *config = self->vhost->config;
    char
        *template = NULL;

    /*  Look for named configuration entry, and its template definition      */
    if (dest_path) {
        ipr_config_locate (config, dest_path, dest_name);
        if (self->vhost->config->located)
            template = ipr_config_attr (config, "template", template_name);
    }
    /*  Load template definition                                             */
    ipr_config_locate (config, template_path, template? template: template_name);
    s_load_property_set (self, config);

    /*  Load primary definition if possible                                  */
    if (dest_path) {
        ipr_config_locate (config, dest_path, dest_name);
        s_load_property_set (self, config);
    }
    /*  Verify/adjust destination properties                                 */
    if (self->opt_priority_levels > AMQP_MAX_PRIORITIES)
        self->opt_priority_levels = AMQP_MAX_PRIORITIES;
    else
    if (self->opt_priority_levels == 0)
        self->opt_priority_levels = 1;
}

static void
s_load_property_set ($(selftype) *self, ipr_config_t *config)
{
    char
        *value;

    if (config->located) {
        IPR_CONFIG_GETATTR_LONG (config, self->opt_min_consumers,    "min-consumers");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_max_consumers,    "max-consumers");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_memory_queue_max, "memory-queue-max");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_max_messages,     "max-messages");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_max_message_size, "max-message-size");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_priority_levels,  "priority-levels");
        IPR_CONFIG_GETATTR_BOOL (config, self->opt_protected,        "protected");
        IPR_CONFIG_GETATTR_BOOL (config, self->opt_persistent,       "persistent");
        IPR_CONFIG_GETATTR_BOOL (config, self->opt_auto_purge,       "auto-purge");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_record_size,      "record-size");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_extent_size,      "extent-size");
        IPR_CONFIG_GETATTR_LONG (config, self->opt_block_size,       "block-size");
    }
}
</private>

<method name = "selftest" />

</class>
