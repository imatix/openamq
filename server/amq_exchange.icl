<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
-->

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

<option name = "links" value = "1"/>

<inherit class = "smt_object" />
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "initial_size" value = "15" />
</inherit>
<inherit class = "icl_list_item" />
<inherit class = "amq_console_object" />
<inherit class = "smt_object_tracker" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "exchange" parent = "amq_broker" label = "Exchange">
        <field name = "name">
          <get>icl_shortstr_cpy (field_value, self->name);</get>
        </field>
        <field name = "type">
          <get>icl_shortstr_cpy (field_value, amq_exchange_type_name (self->type));</get>
        </field>
        <field name = "bindings" label = "Number of bindings" type = "int">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", amq_binding_list_count (self->binding_list));</get>
        </field>
        <field name = "messages_in" type = "int" label = "Messages published">
          <get>icl_shortstr_fmt (field_value, "%d", self->contents_in);</get>
        </field>
        <field name = "messages_out" type = "int" label = "Messages routed">
          <get>icl_shortstr_fmt (field_value, "%d", self->contents_out);</get>
        </field>
        <field name = "megabytes_in" type = "int" label = "Megabytes published">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->traffic_in / (1024 * 1024)));</get>
        </field>
        <field name = "megabytes_out" type = "int" label = "Megabytes routed">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", (int) (self->traffic_out / (1024 * 1024)));</get>
        </field>

        <class name = "binding" label = "Bindings" repeat = "1" source = "amq_binding_mgt" >
          <local>
            amq_binding_t
                *binding;
          </local>
          <get>
            binding = amq_binding_list_first (self->binding_list);
            if (binding)
                icl_shortstr_fmt (field_value, "%d", binding->binding_mgt->object_id);
          </get>
          <next>
            binding = amq_binding_list_next (&binding);
            if (binding)
                icl_shortstr_fmt (field_value, "%d", binding->binding_mgt->object_id);
          </next>
        </class>
    </class>
</data>

<import class = "amq_server_classes" />
<import class = "amq_queue_bindings_list_table" />

<context>
    int
        type;                           //  Exchange type
    icl_shortstr_t
        name;                           //  Exchange name
    Bool
        internal;                       //  Internal exchange?
    void
        *object;                        //  Exchange implementation
    amq_binding_list_t
        *binding_list;                  //  Bindings as a list
    amq_queue_bindings_list_table_t
        *queue_bindings;                //  Bindings sorted by queues
    ipr_hash_table_t
        *binding_hash;                  //  Bindings hashed by routing_key
    ipr_index_t
        *binding_index;                 //  Gives us binding indices
    amq_queue_t
        **queue_set;                    //  Queue publish set
    amq_federation_t
        *federation;                    //  Federation for this exchange, if any
    int
        federation_type;                //  Federation type, or 0 means none

    //  Exchange access functions
    int
        (*publish) (
            void                 *self,
            amq_server_channel_t *channel,
            amq_content_basic_t  *content,
            int                   group,
            Bool                  immediate);
    int
        (*compile) (
            void                 *self,
            amq_binding_t        *binding,
            amq_server_channel_t *channel);
    int
        (*unbind) (
            void                 *self,
            amq_binding_t        *binding);

    //  Statistics
    int64_t
        traffic_in,                     //  Traffic in, in octets
        traffic_out,                    //  Traffic out, in octets
        contents_in,                    //  Contents in, in octets
        contents_out;                   //  Contents out, in octets
</context>

<public name = "header">
//  Exchange types we implement
#define AMQ_EXCHANGE_SYSTEM         1
#define AMQ_EXCHANGE_FANOUT         2
#define AMQ_EXCHANGE_DIRECT         3
#define AMQ_EXCHANGE_TOPIC          4
#define AMQ_EXCHANGE_HEADERS        5
#define AMQ_EXCHANGE_REGEXP         6

//  Max number of queues we can publish one message to.
//  Used for static table of void * per exchange instance.
#define AMQ_QUEUE_SET_MAX           16000
</public>

<method name = "new">
    <argument name = "type" type = "int">Exchange type</argument>
    <argument name = "name" type = "char *">Exchange name</argument>
    <argument name = "internal" type = "Bool">Internal exchange?</argument>
    <argument name = "auto federate" type = "Bool">Auto-federate exchange?</argument>
    <dismiss argument = "table" value = "amq_broker->exchange_table" />
    <dismiss argument = "key" value = "name" />
    <local>
    ipr_config_t
        *config;
    char
        *federation_attach,             //  Host to attach to
        *federation_vhost,              //  Virtual host
        *federation_login;              //  Login name
    int
        federation_type;                //  Federation type
    char
        *type_name;                     //  Federation type text
    Bool
        federation_valid;               //  Is federation configured?
    </local>
    //
    self->type           = type;
    self->internal       = internal;
    self->binding_list   = amq_binding_list_new ();
    self->binding_hash   = ipr_hash_table_new ();
    self->binding_index  = ipr_index_new ();
    self->queue_bindings = amq_queue_bindings_list_table_new ();
    self->queue_set      = icl_mem_alloc (AMQ_QUEUE_SET_MAX * sizeof (amq_queue_t *));
    icl_shortstr_cpy (self->name, name);

    if (!self->internal && amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: create   exchange=%s type=%s", self->name, amq_exchange_type_name (self->type));

    if (self->type == AMQ_EXCHANGE_SYSTEM) {
        self->object  = amq_exchange_system_new (self);
        self->publish = amq_exchange_system_publish;
        self->compile = amq_exchange_system_compile;
        self->unbind  = amq_exchange_system_unbind;
    }
    else
    if (self->type == AMQ_EXCHANGE_FANOUT) {
        self->object  = amq_exchange_fanout_new (self);
        self->publish = amq_exchange_fanout_publish;
        self->compile = amq_exchange_fanout_compile;
        self->unbind  = amq_exchange_fanout_unbind;
    }
    else
    if (self->type == AMQ_EXCHANGE_DIRECT) {
        self->object  = amq_exchange_direct_new (self);
        self->publish = amq_exchange_direct_publish;
        self->compile = amq_exchange_direct_compile;
        self->unbind  = amq_exchange_direct_unbind;
    }
    else
    if (self->type == AMQ_EXCHANGE_TOPIC) {
        self->object  = amq_exchange_topic_new (self);
        self->publish = amq_exchange_topic_publish;
        self->compile = amq_exchange_topic_compile;
        self->unbind  = amq_exchange_topic_unbind;
    }
    else
    if (self->type == AMQ_EXCHANGE_HEADERS) {
        self->object  = amq_exchange_headers_new (self);
        self->publish = amq_exchange_headers_publish;
        self->compile = amq_exchange_headers_compile;
        self->unbind  = amq_exchange_headers_unbind;
    }
    else
    if (self->type == AMQ_EXCHANGE_REGEXP) {
        self->object  = amq_exchange_regexp_new (self);
        self->publish = amq_exchange_regexp_publish;
        self->compile = amq_exchange_regexp_compile;
        self->unbind  = amq_exchange_regexp_unbind;
    }
    else
        smt_log_print (amq_broker->alert_log,
            "E: invalid type '%d' in exchange_new", self->type);

    amq_exchange_list_queue (amq_broker->exchange_list, self);

    //  EXCHANGE FEDERATION ===================================================
    //
    //  Option 1: auto-federation using --attach, federates ESB exchanges
    //  (amq.service, amq.data, amq.dataex).  Fine tune using --attach-vhost
    //  and --attach-login.  Expand to more exchanges using --attach-all.
    //
    //  Option 2: manual federation using federate profiles. Command
    //  line values act as defaults for these profiles.
    //
    //  federate
    //      exchange = "pattern"    Name or wildcard using *
    //      attach = "hostname"     Default is --attach if specified
    //    [ vhost = "path" ]        Default is --attach-path ("/")
    //    [ login = "userid" ]      Default is --attach-login ("peering")
    //    [ type = "service | data | subscriber | publisher | locator" ]
    //                              Default is 'service' for direct exchanges
    //                              and 'fanout' for all others.
    //
    //  Internal, default, and system exchanges cannot be federated.
    //  =======================================================================

    //  Take default settings from federation section (or command line)
    federation_valid  = FALSE;
    federation_attach = amq_server_config_attach       (amq_server_config);
    federation_vhost  = amq_server_config_attach_vhost (amq_server_config);
    federation_login  = amq_server_config_attach_login (amq_server_config);
    federation_type   = (self->type == AMQ_EXCHANGE_HEADERS)?
                        AMQ_FEDERATION_SERVICE: AMQ_FEDERATION_FANOUT;

    //  First take settings from configuration file if possible
    config = ipr_config_dup (amq_server_config_table (amq_server_config));
    ipr_config_locate (config, "/config/federate", "");
    while (config->located) {
        char
            *pattern = ipr_config_get (config, "exchange", NULL);
            if (pattern) {
                if (ipr_str_matches (name, pattern))
                    break;          //  We have a match
            }
            else
                icl_console_print ("E: missing 'exchange' name in &lt;federate&gt;");
        ipr_config_next (config);
    }
    if (config->located) {
        //  We take the attach host, vhost, and login from the specific
        //  federate entry.  If not set there, we default to whatever
        //  was used on the command line, which can override the values
        //  set in the federation section.  This lets users work with
        //  only the command line, or only the configured entries, or a
        //  mix of the two.
        //
        federation_attach = ipr_config_get (config, "attach", federation_attach);
        federation_vhost  = ipr_config_get (config, "vhost",  federation_vhost);
        federation_login  = ipr_config_get (config, "login",  federation_login);

        //  Override default type if explicit federation type set
        type_name = ipr_config_get (config, "type", NULL);
        if (type_name) {
            federation_type = amq_federation_type_lookup (type_name);
            if (federation_type > 0)
                federation_valid = TRUE;
            else
            if (federation_type &lt; 0)
                icl_console_print ("E: invalid type '%s' in &lt;federate&gt;", type_name);
        }
        else
        if (*federation_attach)
            federation_valid = TRUE;    //  Using default federation type
    }
    else
    if (*federation_attach) {
        //  Federate if exchange created with auto-federation
        //  or if the name matches the --attach-all pattern
        if (auto_federate
        ||  ipr_str_matches (name, amq_server_config_attach_all (amq_server_config)))
            federation_valid = TRUE;
    }
    //  We don't federate system, internal, or the default exchange
    if (self->type == AMQ_EXCHANGE_SYSTEM || internal || streq (name, "$default$"))
        federation_valid = FALSE;

    if (federation_valid) {
        self->federation = amq_federation_new (
            self,
            federation_type,
            federation_attach,
            federation_vhost,
            federation_login);
        self->federation_type = federation_type;
    }
    ipr_config_destroy (&config);
</method>

<method name = "destroy">
    <action>
    ipr_hash_table_destroy (&self->binding_hash);
    amq_binding_list_destroy (&self->binding_list);
    ipr_index_destroy (&self->binding_index);
    amq_queue_bindings_list_table_destroy (&self->queue_bindings);
    icl_mem_free (self->queue_set);
    amq_federation_destroy (&self->federation);

    if (!self->internal && amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: destroy  exchange=%s type=%s", self->name, amq_exchange_type_name (self->type));

    if (self->type == AMQ_EXCHANGE_SYSTEM) {
        amq_exchange_system_t
            *exchange = (amq_exchange_system_t *) self->object;
        amq_exchange_system_destroy (&exchange);
    }
    else
    if (self->type == AMQ_EXCHANGE_FANOUT) {
        amq_exchange_fanout_t
            *exchange = (amq_exchange_fanout_t *) self->object;
        amq_exchange_fanout_destroy (&exchange);
    }
    else
    if (self->type == AMQ_EXCHANGE_DIRECT) {
        amq_exchange_direct_t
            *exchange = (amq_exchange_direct_t *) self->object;
        amq_exchange_direct_destroy (&exchange);
    }
    else
    if (self->type == AMQ_EXCHANGE_TOPIC) {
        amq_exchange_topic_t
            *exchange = (amq_exchange_topic_t *) self->object;
        amq_exchange_topic_destroy (&exchange);
    }
    else
    if (self->type == AMQ_EXCHANGE_HEADERS) {
        amq_exchange_headers_t
            *exchange = (amq_exchange_headers_t *) self->object;
        amq_exchange_headers_destroy (&exchange);
    }
    else
    if (self->type == AMQ_EXCHANGE_REGEXP) {
        amq_exchange_regexp_t
            *exchange = (amq_exchange_regexp_t *) self->object;
        amq_exchange_regexp_destroy (&exchange);
    }
    </action>
</method>

<method name = "type lookup" return = "rc">
    <doc>
    Translates an exchange type name into an internal type number.  If
    the type name is not valid, returns -1, else returns one of the
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
    if (streq (type_name, "regexp"))
        rc = AMQ_EXCHANGE_REGEXP;
    else
        rc = -1;
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
    if (type == AMQ_EXCHANGE_REGEXP)
        name = "regexp";
    else
        name = "(unknown)";
</method>

<method name = "bind queue" template = "async function">
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
    routing_key = icl_mem_strdup (routing_key);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    amq_queue_unlink (&queue);
    icl_longstr_destroy (&arguments);
    icl_mem_free (routing_key);
    </release>
    //
    <action>
    amq_binding_t
        *binding = NULL;                //  New binding created
    ipr_hash_t
        *hash;                          //  Entry into hash table
    amq_queue_bindings_list_t
        *bindings_list;                 //  List of bindings for the queue
    amq_queue_bindings_list_iter_t *
        iterator;

    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: bind     %s: queue=%s", self->name, queue->name);

    //  Treat empty arguments as null to simplify comparisons
    if (arguments && arguments->cur_size == 0)
        arguments = NULL;

    //  Force all fanout bindings to a single instance
    if (self->type == AMQ_EXCHANGE_FANOUT)
        routing_key = "";

    //  We need to know if this is a new binding or not
    //  First, we`ll check on the routing key
    hash = ipr_hash_table_search (self->binding_hash, routing_key);
    if (hash) {
        //  We found the same routing key, now we need to check
        //  all bindings to check for an exact match
        binding = amq_binding_list_first (self->binding_list);
        while (binding) {
            if (streq (binding->routing_key, routing_key)
            && icl_longstr_eq (binding->arguments, arguments))
                break;
            binding = amq_binding_list_next (&binding);
        }
    }
    if (!binding) {
        //  If no binding matched, create a new one and compile it
        binding = amq_binding_new (self, routing_key, arguments, queue->exclusive);
        assert (binding);               //  If that failed, don't continue
        if (!hash)                      //  Hash routing key if needed
            hash = ipr_hash_new (self->binding_hash, routing_key, binding);

        //  Compile binding and put all 'wildcard' bindings at the front
        //  of the list. The meaning of this flag depends on the exchange.
        if (self->compile (self->object, binding, channel) == 0) {
            if (binding->is_wildcard)
                amq_binding_list_push (self->binding_list, binding);
            else
                amq_binding_list_queue (self->binding_list, binding);
        }
    }
    //  In a service federation we forward exclusive bindings only and we
    //  prohibit a mix of exclusive/non-exclusive queues on the same binding.
    else
    if (self->federation_type == AMQ_FEDERATION_SERVICE) {
        if (channel && (binding->exclusive != queue->exclusive))
            amq_server_channel_error (channel, ASL_NOT_ALLOWED,
                "Invalid bind mix for service federation",
                AMQ_SERVER_QUEUE, AMQ_SERVER_QUEUE_BIND);
    }
    //  Add queue to binding structures if not already present
    if (queue) {
        bindings_list = amq_queue_bindings_list_table_search (
            self->queue_bindings, queue->name);
        if (!bindings_list)
            bindings_list = amq_queue_bindings_list_new (
                self->queue_bindings, queue->name);
        //  Search per-queue bindings_list for a matching binding
        for (iterator = amq_queue_bindings_list_first (bindings_list);
             iterator != NULL;
             iterator = amq_queue_bindings_list_next (&iterator)) {
            if (iterator->item == binding)
                break;
        }
        //  And only add binding to per-queue bindings_list once
        if (!iterator)
            amq_queue_bindings_list_queue (bindings_list, binding);
        amq_queue_bindings_list_unlink (&bindings_list);
    }
    amq_queue_binding_register (queue, self, binding);
    amq_binding_bind_queue (binding, queue);
    amq_binding_unlink (&binding);
    ipr_hash_unlink (&hash);
    </action>
</method>

<method name = "unbind queue" template = "async function">
    <doc>
    Unbind a queue from the exchange.  This method implements the queue.unbind
    protocol command.  We search for the specific binding to unbind, as opposed
    to the "broker unbind queue" method which destroys all the bindings from a
    specified queue.
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
    routing_key = icl_mem_strdup (routing_key);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    amq_queue_unlink (&queue);
    icl_longstr_destroy (&arguments);
    icl_mem_free (routing_key);
    </release>
    //
    <action>
    amq_queue_bindings_list_t
        *queue_bindings;                //  List of bindings for queue
    amq_queue_bindings_list_iter_t
        *iterator;

    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: unbind     %s: queue=%s", self->name, queue->name);

    //  Treat empty arguments as null, to simplify comparisons
    if (arguments && arguments->cur_size == 0)
        arguments = NULL;
    queue_bindings =
        amq_queue_bindings_list_table_search (self->queue_bindings, queue->name);
    if (queue_bindings) {
        //  Search queue_bindings list for the matching binding
        for (iterator = amq_queue_bindings_list_first (queue_bindings);
              iterator != NULL;
              iterator = amq_queue_bindings_list_next (&iterator)) {
            if (streq (iterator->item->routing_key, routing_key) &&
                icl_longstr_eq (iterator->item->arguments, arguments)) {
                amq_queue_binding_cancel (queue, self, iterator->item);
                if (amq_binding_unbind_queue (iterator->item, queue))
                    //  If binding is now empty, destroy it
                    self->unbind (self->object, iterator->item);
                amq_queue_bindings_list_iter_destroy (&iterator);
                break;
            }
        }
        //  If per-queue binding list is now empty, destroy it
        if (amq_queue_bindings_list_count (queue_bindings) == 0)
            amq_queue_bindings_list_destroy (&queue_bindings);
        else
            amq_queue_bindings_list_unlink (&queue_bindings);
    }
    </action>
</method>

<method name = "publish" template = "async function">
    <doc>
    Publishes the message to the exchange.  The actual routing mechanism
    is defined in the exchange implementations.
    </doc>
    <argument name = "channel"   type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "content"   type = "amq_content_basic_t *">Content to publish</argument>
    <argument name = "mandatory" type = "Bool">Route to queue or return?</argument>
    <argument name = "immediate" type = "Bool">Send immediately or return?</argument>
    <argument name = "group"     type = "int">User group, from connection</argument>
    //
    <possess>
    channel = amq_server_channel_link (channel);
    content = amq_content_basic_link (content);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    amq_content_basic_unlink (&content);
    </release>
    //
    <action>
    int
        delivered = 0;                      //  Number of message deliveries
    amq_server_connection_t
        *connection;
    Bool
        returned = FALSE;

    //  If channel is set, we got message from local application; if we have
    //  fanout federation, send to parent broker but don't publish locally.
    //  This stops message being delivered twice to local apps (once now and
    //  once when received back from parent).
    //
    if (self->federation_type == AMQ_FEDERATION_FANOUT && channel)
        delivered = 1;                  //  Just fake it
    else
        //  Publish message locally
        delivered = self->publish (self->object, channel, content, group, immediate);

    //  Publish message to federation if necessary. We don't have the problem
    //  of message loops with fanout federations because we only push back to
    //  the parent messages that came from a client app (channel != 0).
    //
    if (self->federation_type == AMQ_FEDERATION_PUBLISHER ||
       (self->federation_type == AMQ_FEDERATION_FANOUT && channel) ||
       (self->federation_type == AMQ_FEDERATION_LOCATOR && !delivered) ||
       (self->federation_type == AMQ_FEDERATION_SERVICE && !delivered)) {
        amq_federation_message_published (self->federation, content, mandatory, immediate);
        delivered++;
    }
    if (!delivered && mandatory) {
        if (!content->returned) {
            connection = channel?
                amq_server_connection_link (channel->connection): NULL;
            if (connection) {
                amq_server_agent_basic_return (
                    connection->thread,
                    channel->number,
                    content,
                    ASL_NOT_DELIVERED,
                    "Message cannot be processed - no route is defined",
                    self->name,
                    content->routing_key,
                    NULL);
                content->returned = TRUE;
            }
            returned = TRUE;
            amq_server_connection_unlink (&connection);
        }
    }
    if (amq_server_config_debug_route (amq_server_config)) {
        if (returned)
            smt_log_print (amq_broker->debug_log,
                "X: return   %s: message=%s reason=unroutable_mandatory",
                    self->name, content->message_id);
        else
        if (!delivered)
            smt_log_print (amq_broker->debug_log,
                "X: discard  %s: message=%s reason=unroutable_optional",
                    self->name, content->message_id);
    }
    //  Track exchange statistics
    self->contents_in  += 1;
    self->contents_out += delivered;
    self->traffic_in   += content->body_size;
    self->traffic_out  += delivered * content->body_size;
    </action>
</method>

<method name = "broker unbind queue" template = "async function">
    <doc>
    Unbind a queue from the exchange. Called when queue is being destroyed.
    All the bindings to specific queue are destroyed.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">The queue to unbind</argument>
    //
    <possess>
    queue = amq_queue_link (queue);
    </possess>
    <release>
    amq_queue_unlink (&queue);
    </release>
    //
    <action>
    amq_queue_bindings_list_t
        *queue_bindings;                //  List of bindings for queue
    amq_binding_t
        *binding;

    queue_bindings =
        amq_queue_bindings_list_table_search (self->queue_bindings, queue->name);
    if (queue_bindings) {
        //  Iterate over queue_bindings list, removing each binding
        while (1) {
            binding = amq_queue_bindings_list_pop (queue_bindings);
            if (!binding)
                break;
            if (amq_binding_unbind_queue (binding, queue))
                //  If binding is now empty, destroy it
                self->unbind (self->object, binding);
            amq_binding_unlink (&binding);
        }
        //  Per-queue bindings list is now empty, destroy it
        amq_queue_bindings_list_destroy (&queue_bindings);
    }
    </action>
</method>

<method name = "selftest" />

</class>
