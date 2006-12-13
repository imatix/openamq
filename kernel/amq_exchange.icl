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

<option name = "links" value = "1"/>

<inherit class = "smt_object" />
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "initial_size" value = "15" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "by_vhost" />
</inherit>
<inherit class = "amq_console_object" />
<inherit class = "smt_object_tracker" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "exchange" parent = "broker" label = "Exchange">
        <field name = "name">
          <get>icl_shortstr_cpy (field_value, self->name);</get>
        </field>
        <field name = "type">
          <get>icl_shortstr_cpy (field_value, amq_exchange_type_name (self->type));</get>
        </field>
        <field name = "durable" label = "Durable exchange?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->durable);</get>
        </field>
        <field name = "auto_delete" label = "Auto-deleted?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->auto_delete);</get>
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
    </class>
</data>

<import class = "amq_server_classes" />
<import class = "amq_queue_bindings_list_table" />

<context>
    amq_broker_t
        *broker;                        //  Parent broker
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
    amq_queue_bindings_list_table_t
        *queue_bindings;                //  Bindings sorted by queues
    ipr_hash_table_t
        *binding_hash;                  //  Bindings hashed by routing_key
    ipr_index_t
        *binding_index;                 //  Gives us binding indices

    amq_cluster_mta_t
        *mta;                           //  MTA for this exchange, if any
    int
        mta_mode;                       //  MTA mode, if we're using an MTA

    //  Exchange access functions
    int
        (*publish) (
            void                 *self,
            amq_server_channel_t *channel,
            amq_server_method_t  *method);
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
</public>

<method name = "new">
    <argument name = "vhost" type = "amq_vhost_t *">Parent vhost</argument>
    <argument name = "type" type = "int">Exchange type</argument>
    <argument name = "name" type = "char *">Exchange name</argument>
    <argument name = "durable" type = "Bool">Is exchange durable?</argument>
    <argument name = "auto delete" type = "Bool">Auto-delete unused exchange?</argument>
    <argument name = "internal" type = "Bool">Internal exchange?</argument>
    <dismiss argument = "key" value = "name">Key is exchange name</dismiss>
    <local>
    amq_broker_t
        *broker = amq_broker;
    ipr_config_t
        *config;
    char
        *mta_host,                      //  Host used for MTA
        *mta_vhost,                     //  Virtual host for MTA
        *mta_login;                     //  Login name for MTA
    int
        mta_mode;                       //  MTA mode
    </local>
    //
    self->broker        = broker;
    self->vhost         = vhost;
    self->type          = type;
    self->durable       = durable;
    self->auto_delete   = auto_delete;
    self->internal      = internal;
    self->binding_list  = amq_binding_list_new ();
    self->binding_hash  = ipr_hash_table_new ();
    self->binding_index = ipr_index_new ();
    self->queue_bindings
                        = amq_queue_bindings_list_table_new ();
    icl_shortstr_cpy (self->name, name);

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
        smt_log_print (amq_broker->alert_log,
            "E: invalid type '%d' in exchange_new", self->type);

    amq_exchange_by_vhost_queue (self->vhost->exchange_list, self);

    //  If exchange is configured for message transfer, create MTA agent
    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/cluster-mta", name);
    if (config->located) {
        mta_host  = ipr_config_get (config, "host", NULL);
        mta_vhost = ipr_config_get (config, "vhost", "/");
        mta_login = ipr_config_get (config, "login", "peering");
        mta_mode  = atoi (ipr_config_get (config, "mode", "0"));
        if (AMQ_MTA_MODE_VALID (mta_mode)) {
            self->mta = amq_cluster_mta_new (mta_host, mta_vhost, mta_login, self, mta_mode);
            self->mta_mode = mta_mode;
        }
        else
        if (mta_mode > 0)
            icl_console_print ("W: invalid mode for MTA '%s' - ignoring", name);
    }
    ipr_config_destroy (&config);
</method>

<method name = "destroy">
    <action>
    ipr_hash_table_destroy (&self->binding_hash);
    amq_binding_list_destroy (&self->binding_list);
    ipr_index_destroy (&self->binding_index);
    amq_queue_bindings_list_table_destroy (&self->queue_bindings);
    amq_cluster_mta_destroy (&self->mta);

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

    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: bind     %s: queue=%s", self->name, queue->name);

    //  Treat empty arguments as null to simplify comparisons
    if (arguments && arguments->cur_size == 0)
        arguments = NULL;

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
        binding = amq_binding_new (self, routing_key, arguments);
        assert (binding);
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
    if (queue) {
        bindings_list = amq_queue_bindings_list_table_search (
            self->queue_bindings, queue->name);
        if (!bindings_list)
            bindings_list = amq_queue_bindings_list_new (
                self->queue_bindings, queue->name);
        amq_queue_bindings_list_push_back (bindings_list, binding);
        amq_queue_bindings_list_unlink (&bindings_list);
    }

    //  Notify MTA about new binding
    if (self->mta)
        amq_cluster_mta_binding_created (self->mta, routing_key, arguments);

    amq_binding_bind_queue (binding, queue);
    amq_binding_unlink (&binding);
    ipr_hash_unlink (&hash);
    </action>
</method>

<method name = "publish" template = "async function" async = "1">
    <doc>
    Publishes the message to the exchange.  The actual routing mechanism
    is defined in the exchange implementations.
    </doc>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "method"  type = "amq_server_method_t *">Publish method</argument>
    //
    <possess>
    channel = amq_server_channel_link (channel);
    method = amq_server_method_link (method);
    </possess>
    <release>
    amq_server_channel_unlink (&channel);
    amq_server_method_unlink (&method);
    </release>
    //
    <action>
    int
        delivered = 0;                      //  Number of message deliveries
    int64_t
        content_size;
    amq_server_connection_t
        *connection;
    Bool
        returned = FALSE;

    if (self->mta_mode == AMQ_MTA_MODE_BOTH && channel)
        delivered = 1;
    else
        delivered = self->publish (self->object, channel, method);

    content_size = ((amq_content_basic_t *) method->content)->body_size;

    //  Publish message to MTA if required
    if (self->mta && (self->mta_mode == AMQ_MTA_MODE_FORWARD_ALL ||
          (self->mta_mode == AMQ_MTA_MODE_BOTH && channel) ||
          (self->mta_mode == AMQ_MTA_MODE_FORWARD_ELSE && !delivered))) {

        amq_cluster_mta_message_published (
            self->mta,
            channel,
            method->content,
            method->payload.basic_publish.mandatory,
            method->payload.basic_publish.immediate);
        delivered++;
    }

    if (!delivered && method->payload.basic_publish.mandatory) {
        if (method->class_id == AMQ_SERVER_BASIC) {
            if (!((amq_content_basic_t *) method->content)->returned) {
                connection = channel?
                    amq_server_connection_link (channel->connection): NULL;
                if (connection) {
                    icl_console_print ("I: Returning message to sender.");
                    amq_server_agent_basic_return (
                        connection->thread,
                        channel->number,
                        (amq_content_basic_t *) method->content,
                        ASL_NOT_DELIVERED,
                        "Message cannot be processed - no route is defined",
                        method->payload.basic_publish.exchange,
                        method->payload.basic_publish.routing_key,
                        ((amq_content_basic_t *) method->content)->sender_id,
                        NULL);
                    ((amq_content_basic_t *) method->content)->returned = TRUE;
                }
                returned = TRUE;
                amq_server_connection_unlink (&connection);
            }
        }
    }
    if (amq_server_config_debug_route (amq_server_config)) {
        if (returned)
            smt_log_print (amq_broker->debug_log,
                "X: return   %s: message=%s reason=unroutable_mandatory",
                    self->name,
                    ((amq_content_basic_t *) method->content)->message_id);
        else
        if (!delivered)
            smt_log_print (amq_broker->debug_log,
                "X: discard  %s: message=%s reason=unroutable_optional",
                    self->name,
                    ((amq_content_basic_t *) method->content)->message_id);
    }

    //  Track exchange statistics
    self->contents_in  += 1;
    self->contents_out += delivered;
    self->traffic_in   += content_size;
    self->traffic_out  += (delivered * content_size);
    </action>
</method>

<method name = "unbind queue" template = "async function" async = "1">
    <doc>
    Unbind a queue from the exchange.
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
        *queue_bindings;
    amq_binding_t
        *binding;

    queue_bindings =
        amq_queue_bindings_list_table_search (self->queue_bindings, queue->name);
    if (queue_bindings) {
        while (1) {
            binding = amq_queue_bindings_list_pop (queue_bindings);
            if (!binding)
                break;
            if (amq_binding_unbind_queue (binding, queue))
                //  Allow the exchange implementation the chance to cleanup the
                //  binding, but be careful to get the next binding first...
                self->unbind (self->object, binding);
            amq_binding_unlink (&binding);
        }
        amq_queue_bindings_list_destroy (&queue_bindings);
    }
    </action>
</method>

<method name = "selftest" />

</class>
