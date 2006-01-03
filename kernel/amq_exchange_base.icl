<?xml?>
<class
    name      = "amq_exchange_base"
    comment   = "Exchange abstract base class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
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
    assert (self);
    </header>
</method>

<method name = "publish" return = "rc">
    <doc>
    Routes one message through the exchange.
    </doc>
    <argument name = "self_v"  type = "void *">The exchange cast as a void *</argument>
    <argument name = "channel" type = "amq_server_channel_t *">Channel for reply</argument>
    <argument name = "method"  type = "amq_server_method_t *">Publish method</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self = self_v;
    amq_server_basic_publish_t
        *basic_method;
    amq_content_basic_t
        *basic_content;
    char
        *routing_key = "",
        *message_id = NULL;
    Bool
        mandatory,                      //  Mandatory option from method
        delivered = FALSE,              //  Set to TRUE if message processed
        returned = FALSE;
    </local>
    //
    <header>
    assert (self);
    if (method->class_id == AMQ_SERVER_BASIC) {
        basic_method  = &method->payload.basic_publish;
        basic_content = (amq_content_basic_t *) (method->content);
        mandatory     = basic_method->mandatory;
        routing_key   = basic_method->routing_key;
        message_id    = basic_content->message_id;
    }
    else
        icl_console_print ("E: $(selfname) - bad class_id - %d", method->class_id);
    </header>
    <footer>
    if (delivered == FALSE && mandatory) {
        if (method->class_id == AMQ_SERVER_BASIC) {
            if (amq_server_channel_alive (channel) && !basic_content->returned) {
                amq_server_agent_basic_return (
                    channel->connection->thread,
                    (dbyte) channel->key,
                    basic_content,
                    ASL_NOT_DELIVERED,
                    "Message cannot be processed - no route is defined",
                    basic_method->exchange,
                    routing_key);
                basic_content->returned = TRUE;
                returned = TRUE;
            }
        }
    }
    if (amq_server_config_trace_route (amq_server_config)) {
        if (returned)
            icl_console_print ("X: return   message=%s reason=unroutable_mandatory",
                message_id);
        else
        if (!delivered)
            icl_console_print ("X: discard  message=%s reason=unroutable_optional",
                message_id);
    }
    </footer>
</method>

<method name = "selftest" />

</class>
