<?xml?>
<class
    name      = "amq_exchange"
    comment   = "Polymorphic exchange class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class implements the server exchange, an asynchronous
object that acts as a envelope for the separate exchange managers
for each type of exchange. This is a lock-free asynchronous class.
</doc>

<inherit class = "smt_object" />
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "hash_size" value = "65535" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "list" />
</inherit>
<inherit class = "amq_console_object" />

<!-- Console definitions for this object -->
<data type = "cml">
    <class name = "exchange" parent = "vhost">
        <field name = "name"        label = "Exchange name">
          <get>icl_shortstr_cpy (field_value, self->name);</get>
        </field>
        <field name = "type"        label = "Exchange type">
          <get>icl_shortstr_cpy (field_value, amq_exchange_type_name (self->type));</get>
        </field>
        <field name = "durable"     label = "Durable exchange?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->durable);</get>
        </field>
        <field name = "auto_delete" label = "Auto-deleted?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->auto_delete);</get>
        </field>
        <field name = "bindings"    label = "Number of bindings" type = "int">
          <get>icl_shortstr_fmt (field_value, "%ld", amq_binding_list_count (self->binding_list));</get>
        </field>
    </class>
</data>

<public name = "include">
#include "amq_server_classes.h"
</public>

<context>
    amq_vhost_t
        *vhost;                         //  Parent vhost
    int
        type;                           //  Exchange type
    icl_shortstr_t
        name;                           //  Exchange name
    Bool
        durable,                        //  Is exchange durable?
        auto_delete,                    //  Auto-delete unused exchange?
        internal;                       //  Internal exchange?
    void
        *object;                        //  Exchange implementation
    amq_binding_list_t
        *binding_list;                  //  Bindings as a list
    ipr_index_t
        *binding_index;                 //  Gives us binding indices

    //  Exchange access functions
    int
        (*publish) (
            void                 *self,
            amq_server_channel_t *channel,
            int                   type_id,
            void                 *content,
            Bool                  mandatory,
            Bool                  immediate);
    int
        (*compile) (
            void                 *self,
            amq_binding_t        *binding,
            amq_server_channel_t *channel);
</context>

<public name = "header">
//  Exchange types we implement

#define AMQ_EXCHANGE_SYSTEM         1
#define AMQ_EXCHANGE_FANOUT         2
#define AMQ_EXCHANGE_DIRECT         3
#define AMQ_EXCHANGE_TOPIC          4
#define AMQ_EXCHANGE_HEADERS        5
</public>

<method name = "new">
    <argument name = "vhost" type = "amq_vhost_t *">Parent vhost</argument>
    <argument name = "type" type = "int">Exchange type</argument>
    <argument name = "name" type = "char *">Exchange name</argument>
    <argument name = "durable" type = "Bool">Is exchange durable?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused exchange?</argument>
    <argument name = "internal" type = "Bool">Internal exchange?</argument>
    <dismiss argument = "key" value = "name">Key is exchange name</dismiss>
    //
    self->vhost         = vhost;
    self->type          = type;
    self->durable       = durable;
    self->auto_delete   = auto_delete;
    self->internal      = internal;
    self->binding_list  = amq_binding_list_new ();
    self->binding_index = ipr_index_new ();
    icl_shortstr_cpy (self->name, name);

    if (self->type == AMQ_EXCHANGE_SYSTEM) {
        self->object  = amq_exchange_system_new (self);
        self->publish = amq_exchange_system_publish;
        self->compile = amq_exchange_system_compile;
    }
    else
    if (self->type == AMQ_EXCHANGE_FANOUT) {
        self->object  = amq_exchange_fanout_new (self);
        self->publish = amq_exchange_fanout_publish;
        self->compile = amq_exchange_fanout_compile;
    }
    else
    if (self->type == AMQ_EXCHANGE_DIRECT) {
        self->object  = amq_exchange_direct_new (self);
        self->publish = amq_exchange_direct_publish;
        self->compile = amq_exchange_direct_compile;
    }
    else
    if (self->type == AMQ_EXCHANGE_TOPIC) {
        self->object  = amq_exchange_topic_new (self);
        self->publish = amq_exchange_topic_publish;
        self->compile = amq_exchange_topic_compile;
    }
    else
    if (self->type == AMQ_EXCHANGE_HEADERS) {
        self->object  = amq_exchange_headers_new (self);
        self->publish = amq_exchange_headers_publish;
        self->compile = amq_exchange_headers_compile;
    }
    else
        icl_console_print ("E: invalid type '%d' in exchange_new", self->type);

    amq_exchange_list_queue (self->vhost->exchange_list, self);
    if (amq_server_config_trace_route (amq_server_config))
        icl_console_print ("X: create   exchange=%s", self->name);
</method>

<method name = "destroy">
    <action>
    if (amq_server_config_trace_route (amq_server_config))
        icl_console_print ("X: destroy  exchange=%s", self->name);

    amq_binding_list_destroy (&self->binding_list);
    ipr_index_destroy (&self->binding_index);
    if (self->type == AMQ_EXCHANGE_SYSTEM)
        amq_exchange_system_destroy ((amq_exchange_system_t **) &self->object);
    else
    if (self->type == AMQ_EXCHANGE_FANOUT)
        amq_exchange_fanout_destroy ((amq_exchange_fanout_t **) &self->object);
    else
    if (self->type == AMQ_EXCHANGE_DIRECT)
        amq_exchange_direct_destroy ((amq_exchange_direct_t **) &self->object);
    else
    if (self->type == AMQ_EXCHANGE_TOPIC)
        amq_exchange_topic_destroy ((amq_exchange_topic_t **) &self->object);
    else
    if (self->type == AMQ_EXCHANGE_HEADERS)
        amq_exchange_headers_destroy ((amq_exchange_headers_t **) &self->object);
    </action>
</method>

<method name = "search" return = "self">
    <argument name = "table" type = "$(selfname)_table_t *">Exchange table</argument>
    <argument name = "name"  type = "char *"               >Exchange name</argument>
    <declare name = "self" type = "$(selftype) *">The found object</declare>
    //
    self = $(selfname)_table_search (table, name);
</method>

<method name = "type lookup" return = "rc">
    <doc>
    Translates an exchange type name into an internal type number.  If
    the type name is not valid, returns zero, else returns one of the
    type numbers supported by this implementation.
    </doc>
    <argument name = "type name" type = "char *">Type name to lookup</argument>
    <declare name = "rc" type = "int">Type number</declare>
    //
    if (streq (type_name, "system"))
        rc = AMQ_EXCHANGE_SYSTEM;
    else
    if (streq (type_name, "fanout"))
        rc = AMQ_EXCHANGE_FANOUT;
    else
    if (streq (type_name, "direct"))
        rc = AMQ_EXCHANGE_DIRECT;
    else
    if (streq (type_name, "topic"))
        rc = AMQ_EXCHANGE_TOPIC;
    else
    if (streq (type_name, "headers"))
        rc = AMQ_EXCHANGE_HEADERS;
    else
        rc = 0;
</method>

<method name = "type name" return = "name">
    <doc>
    Translates an exchange type index into an external name.
    </doc>
    <argument name = "type" type = "int">Type index to translate</argument>
    <declare name = "name" type = "char *">Type name</declare>
    //
    if (type == AMQ_EXCHANGE_SYSTEM)
        name = "system";
    else
    if (type == AMQ_EXCHANGE_FANOUT)
        name = "fanout";
    else
    if (type == AMQ_EXCHANGE_DIRECT)
        name = "direct";
    else
    if (type == AMQ_EXCHANGE_TOPIC)
        name = "topic";
    else
    if (type == AMQ_EXCHANGE_HEADERS)
        name = "headers";
    else
        name = "(unknown)";
</method>

<method name = "bind queue" template = "async function" async = "1">
    <doc>
    Bind a queue to the exchange.  The logic is the same for all exchange
    types - we compare all existing bindings and if we find one that
    matches our arguments (has identical arguments) we attach the queue
    to the binding.  Otherwise we create a new binding and compile it
    into the exchange, this operation being exchange type-specific.
    </doc>
    <argument name = "channel"     type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "queue"       type = "amq_queue_t *">The queue to bind</argument>
    <argument name = "routing key" type = "char *">Bind to routing key</argument>
    <argument name = "arguments"   type = "icl_longstr_t *">Bind arguments</argument>
    //
    <possess>
    channel = amq_server_channel_link (channel);
    queue = amq_queue_link (queue);
    arguments = icl_longstr_dup (arguments);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    amq_queue_unlink (&queue);
    icl_longstr_destroy (&arguments);
    </release>
    //
    <action>
    amq_binding_t
        *binding;                       //  We examine each binding
    ipr_looseref_t
        *looseref;                      //  We check the queues per binding

    if (amq_server_config_trace_route (amq_server_config))
        icl_console_print ("X: bind     queue=%s onto=%s", queue->key, self->name);

    //  Check existing bindings to see if we have one that matches
    binding = amq_binding_list_first (self->binding_list);
    while (binding) {
        if (streq (binding->routing_key, routing_key)
        &&  icl_longstr_eq (binding->arguments, arguments))
            break;
        binding = amq_binding_list_next (&binding);
    }
    //  If no binding matched, create a new one
    if (binding == NULL) {
        //  Compile the binding to the exchange
        binding = amq_binding_new (self, routing_key, arguments);
        if (binding) {
            if (self->compile (self->object, binding, channel) == 0)
                amq_binding_list_queue (self->binding_list, binding);
            else
                amq_binding_destroy (&binding);
        }
        else
            amq_server_channel_close (
                channel, ASL_RESOURCE_ERROR, "Cannot make new binding");
    }
    if (binding) {
        //  Add queue to binding if it's not already there
        looseref = ipr_looseref_list_first (binding->queue_list);
        while (looseref && looseref->object != queue)
            looseref = ipr_looseref_list_next (&looseref);

        if (looseref)                   //  Ignore duplicates
            ipr_looseref_unlink (&looseref);
        else
            amq_binding_bind_queue (binding, queue);

        amq_binding_unlink (&binding);
        if (amq_server_channel_alive (channel))
            amq_server_agent_queue_bind_ok (
                channel->connection->thread, (dbyte) channel->key);
    }
    </action>
</method>

<method name = "publish" template = "async function" async = "1">
    <doc>
    Publishes the message to the exchange.  The actual routing mechanism
    is defined in the exchange implementations.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "class id"  type = "int">The content class</argument>
    <argument name = "content"   type = "void *">The message content</argument>
    <argument name = "mandatory" type = "Bool">Warn if unroutable</argument>
    <argument name = "immediate" type = "Bool">Warn if no consumers</argument>
    //
    <possess>
    if (class_id == AMQ_SERVER_JMS)
        amq_content_jms_possess (content);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_content_basic_possess (content);
    </possess>
    <release>
    if (class_id == AMQ_SERVER_JMS)
        amq_content_jms_destroy ((amq_content_jms_t **) &content);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_content_basic_destroy ((amq_content_basic_t **) &content);
    </release>
    <action>
    self->publish (self->object, channel, class_id, content, mandatory, immediate);
    amq_broker_messages++;
    </action>
</method>

<method name = "selftest" />

</class>
