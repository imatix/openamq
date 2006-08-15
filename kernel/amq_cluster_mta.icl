<?xml?>
<class
    name      = "amq_cluster_mta"
    comment   = "Mesage transfer agent"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
</doc>

<option name = "links" value = "1"/>

<inherit class = "smt_object" />

<import class = "amq_server_classes" />

<context>
    amq_peering_t
        *peering;
    amq_exchange_t
        *exchange;
    Bool
        pull,
        push,
        copy;
</context>

<private>
static int
s_content_handler (
    void *vself,
    amq_peering_t *peering,
    amq_peer_method_t *peer_method)
{
    amq_cluster_mta_t
        *self = (amq_cluster_mta_t*) self;
    amq_client_method_t
        *client_method;

    if (peer_method->class_id == AMQ_PEER_BASIC &&
          peer_method->method_id == AMQ_PEER_BASIC_DELIVER) {

        /*  TODO:  Implement rejected messages returning in pull model       */
        /*  Notice two last arguments in the call bellow                     */
        client_method = amq_client_method_new_basic_publish (
            0, peer_method->payload.basic_deliver.exchange,
            peer_method->payload.basic_deliver.routing_key, FALSE, FALSE);
        client_method->content = peer_method->content;
        peer_method->content = NULL;
        amq_exchange_publish (self->exchange, NULL,
            (amq_server_method_t *) client_method);
        amq_client_method_unlink (&client_method);
    }
    else if (peer_method->class_id == AMQ_PEER_BASIC &&
          peer_method->method_id == AMQ_PEER_BASIC_RETURN) {

        assert (0); // not implemented yet
    }
    else {
        assert (0);
    }

    return (0);
}
</private>

<method name = "new">
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "virtual host" type = "char *">Virtual host</argument>
    <argument name = "login" type = "char*">Login</argument>
    <argument name = "exchange" type = "amq_exchange_t*" />
    <argument name = "pull" type = "Bool" />
    <argument name = "push" type = "Bool" />
    <argument name = "copy" type = "Bool">
        <doc>
        If true, message is published on both local and remote servers
        If false, message is published only on the remote server
        If push is false, argument has no meaning
        </doc>
    </argument>
    self->pull = pull;
    self->push = push;
    self->copy = copy;
    self->peering = amq_peering_new (host, virtual_host,
        amq_server_config_trace (amq_server_config));
    amq_peering_set_login (self->peering, login);
    amq_peering_set_content_handler (self->peering, s_content_handler,
        (void*) self);
    amq_peering_start (self->peering);
    self->exchange = amq_exchange_link (exchange);
    if (!self->copy)
        amq_exchange_discard_messages (self->exchange, TRUE);
</method>

<method name = "destroy">
    amq_peering_stop (self->peering);
    amq_exchange_unlink (&self->exchange);
    amq_peering_unlink (&self->peering);
</method>

<method name = "binding created" template = "function">
    <argument name = "routing key" type = "icl_shortstr_t" />
    <argument name = "arguments" type = "icl_longstr_t*" />
    amq_peering_bind (self->peering, self->exchange->name, routing_key,
        arguments);
</method>

<method name = "message published" template = "function">
    <argument name = "content" type = "amq_content_basic_t*" />
    <argument name = "mandatory" type = "Bool" />
    <argument name = "immediate" type = "Bool" />
    amq_peering_forward (self->peering, self->exchange->name,
        content->routing_key, content, mandatory, immediate);
</method>

<method name = "selftest" />
</class>

