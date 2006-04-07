<?xml?>
<class
    name      = "amq_exchange_base"
    comment   = "Exchange abstract base class"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
This is an abstract base class for all exchange implementations.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_server_classes" />

<context>
    amq_exchange_t
        *exchange;                      //  Parent exchange
</context>

<method name = "new">
    <argument name = "exchange" type = "amq_exchange_t *">Parent exchange</argument>
    //
    self->exchange = exchange;
</method>

<method name = "compile" return = "rc">
    <doc>
    Compiles the binding for the exchange, returns 1 if the binding was
    invalid.
    </doc>
    <argument name = "self_v"  type = "void *">The exchange cast as a void *</argument>
    <argument name = "binding" type = "amq_binding_t *">Binding to compile</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for errors</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
</method>

<method name = "publish" return = "rc">
    <doc>
    Routes one message through the exchange.
    </doc>
    <argument name = "self_v" type = "void *">The exchange cast as a void *</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "method" type = "amq_server_method_t *">Publish method</argument>
    <argument name = "from_cluster" type = "Bool">Intra-cluster publish?</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    amq_server_basic_publish_t
        *basic_method = NULL;
    amq_content_basic_t
        *basic_content = NULL;
    char
        *routing_key = "",
        *message_id = NULL;
    Bool
        mandatory = FALSE,              //  Mandatory option from method
        returned = FALSE;
    int
        delivered = 0;                  //  Number of message deliveries
    amq_server_connection_t
        *connection;
    </local>
    //
    <header>
    $(selfname:upper)_ASSERT_SANE (self);

    if (method->class_id == AMQ_SERVER_BASIC) {
        basic_method  = &method->payload.basic_publish;
        basic_content = (amq_content_basic_t *) (method->content);
        mandatory     = basic_method->mandatory;
        routing_key   = basic_method->routing_key;
        message_id    = basic_content->message_id;
    }
    else
        asl_log_print (amq_broker->alert_log,
            "E: $(selfname) - bad class_id - %d", method->class_id);
    </header>
    <footer>
    if (!delivered && mandatory) {
        if (method->class_id == AMQ_SERVER_BASIC) {
            if (!basic_content->returned) {
                connection = channel?
                    amq_server_connection_link (channel->connection): NULL;
                if (connection) {
                    amq_server_agent_basic_return (
                        connection->thread,
                        channel->number,
                        basic_content,
                        ASL_NOT_DELIVERED,
                        "Message cannot be processed - no route is defined",
                        basic_method->exchange,
                        routing_key,
                        NULL);
                    amq_server_connection_unlink (&connection);
                    basic_content->returned = TRUE;
                }
                returned = TRUE;
            }
        }
    }
    if (amq_server_config_debug_route (amq_server_config)) {
        if (returned)
            asl_log_print (amq_broker->debug_log,
                "X: return   %s: message=%s reason=unroutable_mandatory",
                    self->exchange->name, message_id);
        else
        if (!delivered)
            asl_log_print (amq_broker->debug_log,
                "X: discard  %s: message=%s reason=unroutable_optional",
                    self->exchange->name, message_id);
    }
    rc = delivered;                     //  Return number of deliveries
    </footer>
</method>

<method name = "unbind" return = "rc">
    <doc>
    Unbinds the binding for the exchange.
    </doc>
    <argument name = "self_v"  type = "void *">The exchange cast as a void *</argument>
    <argument name = "binding" type = "amq_binding_t *">Binding to compile</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
</method>

<method name = "selftest" />

</class>
