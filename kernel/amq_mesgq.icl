<?xml?>
<class
    name      = "amq_mesgq"
    comment   = "Message queue base class"
    version   = "1.3"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
Implements a message queue, an abstract base.  Message queues are used to
implement queues, topics, and subscriptions.  Message queues exist per
virtual host.
</doc>

<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */

    /*  Object properties                                                    */
    int
        service_type;                   /*  AMQP service - queue or topic    */
    Bool
        temporary;                      /*  Created/deleted dynamically      */
    qbyte
        client_id;                      /*  Owner, if any                    */
    size_t
        disk_queue_size;                /*  Size of disk queue               */
    size_t
        memory_queue_size;              /*  Size of memory queue             */

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
        opt_block_size;                 /*  Size of Bblock, in record        */
</context>

<method name = "new">
    <argument name = "vhost"        type = "amq_vhost_t *">Parent virtual host</argument>
    <argument name = "service_type" type = "int"          >AMQP service type</argument>
    <argument name = "temporary"    type = "Bool"         >Temporary destination?</argument>
    <argument name = "name"         type = "char *"       >External name</argument>
    <argument name = "client_id"    type = "qbyte"        >Owner, if any</argument>

    self->vhost        = vhost;
    self->service_type = service_type;
    self->temporary    = temporary;
    self->client_id    = client_id;
    s_get_configuration (self, name);
</method>

<method name = "destroy">
</method>

<method name = "map name">
    <doc>
    Turns an external message queue name (destination name) and type
    into an internal name; the mapping function creates names that are
    globally unique and file-system safe (contains no special characters).
    A given external name is always mapped to the same internal name.
    </doc>
    <argument name = "mapped"       type = "char *">Mapped name</argument>
    <argument name = "dest name"    type = "char *">Destination name</argument>
    <argument name = "service type" type = "int"   >Service type</argument>
    <local>
    byte
        hash = 0;
    char
        *name_ptr;
    </local>

    /*  Calculate mod-255 hash for name                                      */
    for (name_ptr = dest_name; *name_ptr; name_ptr++)
        hash += *name_ptr;

    ipr_shortstr_fmt (mapped, "%s-%s-%02x",
        service_type == AMQP_SERVICE_QUEUE? "queue": "topic",
        dest_name, hash);
    for (name_ptr = mapped; *name_ptr; name_ptr++)
        if (!(isalnum (*name_ptr) || *name_ptr == '-'))
            *name_ptr = '-';
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
    ipr_shortstr_t
        fullname;                       /*  Disk filename for message queue  */

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
    /*  Tune storage parameters (for physical backing file)                  */
    self_tune (self,
        self->opt_record_size, self->opt_extent_size, self->opt_block_size);

    /*  auto-purge option means delete all messages at restart               */
    if (self->opt_auto_purge || self->temporary)
        self_purge (self);
    else {
        self_locate (self, fullname);
        if (file_exists (fullname)) {
            self->disk_queue_size = self_count (self);
            coprintf ("I: %s has %d existing messages", name, self->disk_queue_size);
        }
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

</class>
