<?xml?>
<class
    name      = "amq_exchange"
    comment   = "Polymorphic exchange class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class implements the server exchange class, an asynchronous
object that acts as a envelope for the separate exchange managers
for each class of exchange. This is a lock-free asynchronous class.
</doc>

<inherit class = "smt_object" />
<inherit class = "ipr_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "hash_size" value = "65535" />
</inherit>

<public name = "include">
#include "amq_server_classes.h"
</public>

<context>
    int
        class;                          //  Destination class
    ipr_shortstr_t
        name;                           //  Destination name
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
            int                   class_id, 
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
//  Exchange classes we implement

#define AMQ_EXCHANGE_SYSTEM         1
#define AMQ_EXCHANGE_DEST_NAME      2
#define AMQ_EXCHANGE_FANOUT         3
</public>

<method name = "new">
    <argument name = "class"       type = "int">Exchange class</argument>
    <argument name = "name"        type = "char *">Exchange name</argument>
    <argument name = "durable"     type = "Bool">Is exchange durable?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused exchange?</argument>
    <argument name = "internal"    type = "Bool">Internal exchange?</argument>
    <dismiss argument = "key"   value = "name">Key is exchange name</dismiss>
    //
    ipr_shortstr_cpy (self->name, name);
    self->class        = class;
    self->durable      = durable;
    self->auto_delete  = auto_delete;
    self->internal     = internal;
    self->binding_list = amq_binding_list_new ();

    if (self->class == AMQ_EXCHANGE_SYSTEM) {
        self->object  = amq_exchange_system_new (self);
        self->publish = amq_exchange_system_publish;
        self->compile = amq_exchange_system_compile;
    }
    else
    if (self->class == AMQ_EXCHANGE_FANOUT) {
        self->object  = amq_exchange_fanout_new (self);
        self->publish = amq_exchange_fanout_publish;
        self->compile = amq_exchange_fanout_compile;
    }
    else
    if (self->class == AMQ_EXCHANGE_DEST_NAME) {
        self->object  = amq_exchange_dest_name_new (self);
        self->publish = amq_exchange_dest_name_publish;
        self->compile = amq_exchange_dest_name_compile;
    }
    else
        icl_console_print ("E: invalid class '%d' in exchange_new", self->class);
</method>

<method name = "destroy">
    <action>
    amq_binding_list_destroy (&self->binding_list);
    if (self->class == AMQ_EXCHANGE_SYSTEM)
        amq_exchange_system_destroy ((amq_exchange_system_t **) &self->object);
    else
    if (self->class == AMQ_EXCHANGE_FANOUT)
        amq_exchange_fanout_destroy ((amq_exchange_fanout_t **) &self->object);
    else
    if (self->class == AMQ_EXCHANGE_DEST_NAME)
        amq_exchange_dest_name_destroy ((amq_exchange_dest_name_t **) &self->object);
    </action>
</method>

<method name = "search" return = "self">
    <argument name = "table" type = "$(selfname)_table_t *">Exchange table</argument>
    <argument name = "name"  type = "char *"               >Exchange name</argument>
    <declare name = "self" type = "$(selftype) *">The found object</declare>
    //
    self = $(selfname)_table_search (table, name);
</method>

<method name = "class lookup" return = "rc">
    <doc>
    Translates an exchange class name into an internal class number.  If
    the class name is not valid, returns zero, else returns one of the
    class numbers supported by this implementation.
    </doc>
    <argument name = "class name" type = "char *">Class name to lookup</argument>
    <declare name = "rc" type = "int">Class number</declare>
    if (streq (class_name, "system"))
        rc = AMQ_EXCHANGE_SYSTEM;
    else
    if (streq (class_name, "fanout"))
        rc = AMQ_EXCHANGE_FANOUT;
    else
    if (streq (class_name, "dest-name"))
        rc = AMQ_EXCHANGE_DEST_NAME;
    else
        rc = 0;
</method>

<method name = "bind queue" template = "async function" async = "1">
    <doc>
    Bind a queue to the exchange.  The logic is the same for all exchange
    classes - we compare all existing bindings and if we find one that
    matches our arguments (has identical arguments) we attach the queue
    to the binding.  Otherwise we create a new binding and compile it
    into the exchange, this operation being exchange class-specific.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "queue"     type = "amq_queue_t *">The queue to bind</argument>
    <argument name = "m_arguments" type = "ipr_longstr_t *">Bind arguments</argument>
    //
    <action>
    amq_binding_t
        *binding;                       //  We examine each binding
    ipr_looseref_t  
        *looseref;                      //  We check the queues per binding
        
    //  Check existing bindings to see if we have one that matches
    binding = amq_binding_list_first (self->binding_list);
    while (binding) {
        if (ipr_longstr_eq (binding->arguments, m_arguments))
            break;
        binding = amq_binding_list_next (&binding);
    }
    //  If no binding matched, create a new one
    if (binding == NULL) {
        //  Compile the binding to the exchange
        binding = amq_binding_new (self, m_arguments);
        if (self->compile (self->object, binding, channel) == 0)
            amq_binding_list_queue (self->binding_list, binding);
        else
            amq_binding_destroy (&binding);
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
        amq_server_agent_queue_bind_ok (
            channel->connection->thread, (dbyte) channel->key);
    }
    </action>
</method>

<method name = "bind exchange" template = "async function" async = "1">
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "exchange"  type = "amq_exchange_t *">The exchange to bind</argument>
    <argument name = "m_arguments" type = "ipr_longstr_t *">Bind arguments</argument>
    //
    <action>
    amq_binding_t
        *binding;                       //  We examine each binding
    ipr_looseref_t  
        *looseref;                      //  We check exchanges per binding
    int
        rc = 0;                         //  >0 means binding failed
        
    //  Check existing bindings to see if we have one that matches
    binding = amq_binding_list_first (self->binding_list);
    while (binding) {
        if (ipr_longstr_eq (binding->arguments, m_arguments))
            break;
        binding = amq_binding_list_next (&binding);
    }
    //  If no binding matched, create a new one
    if (binding == NULL) {
        //  Compile the binding to the exchange
        binding = amq_binding_new (self, m_arguments);
        if (self->compile (self->object, binding, channel) == 0)
            amq_binding_list_queue (self->binding_list, binding);
        else
            amq_binding_destroy (&binding);
    }
    if (binding) {
        //  Add exchange to binding if it's not already there
        looseref = ipr_looseref_list_first (binding->exchange_list);
        while (looseref && looseref->object != exchange)
            looseref = ipr_looseref_list_next (&looseref);

        if (looseref)                   //  Ignore duplicates
            ipr_looseref_unlink (&looseref);
        else
            amq_binding_bind_exchange (binding, exchange);

        amq_binding_unlink (&binding);
        amq_server_agent_exchange_bind_ok (
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
    if (class_id == AMQ_SERVER_JMS)
        amq_content_jms_possess (content);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_content_basic_possess (content);
    //
    <action>
    self->publish (self->object, channel, class_id, content, mandatory, immediate);
    if (class_id == AMQ_SERVER_JMS)
        amq_content_jms_destroy ((amq_content_jms_t **) &content);
    else
    if (class_id == AMQ_SERVER_BASIC)
        amq_content_basic_destroy ((amq_content_basic_t **) &content);
    </action>
</method>

<method name = "selftest" inherit = "none"/>

</class>
