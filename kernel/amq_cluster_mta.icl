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
        *peering;                       //  The peering we work with
    amq_exchange_t
        *exchange;                      //  The exchange we work with
    int
        mode;                           //  MTA operation mode
</context>

<public>
//  MTA operation modes
#define AMQ_MTA_MODE_DISABLED           0
#define AMQ_MTA_MODE_SUBSCRIBER         1
#define AMQ_MTA_MODE_FORWARD_ALL        2
#define AMQ_MTA_MODE_FORWARD_ELSE       3
#define AMQ_MTA_MODE_BOTH               4
#define AMQ_MTA_MODE_VALID(m) (m > 0 && m <= 4)
</public>

<method name = "new">
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "virtual host" type = "char *" />
    <argument name = "login" type = "char *" />
    <argument name = "exchange" type = "amq_exchange_t *" />
    <argument name = "mode" type = "int" />
    //
    assert (AMQ_MTA_MODE_VALID (mode));
    
    self->exchange = amq_exchange_link (exchange);
    self->mode = mode;
    self->peering = amq_peering_new (host, virtual_host, amq_server_config_trace (amq_server_config));
    amq_peering_set_login           (self->peering, login);
    amq_peering_set_content_handler (self->peering, s_content_handler, self);
    amq_peering_set_return_handler  (self->peering, s_return_handler, self);
    amq_peering_start (self->peering);
</method>

<method name = "destroy">
    amq_peering_stop (self->peering);
    amq_exchange_unlink (&self->exchange);
    amq_peering_unlink (&self->peering);
</method>

<method name = "binding created" template = "async function" async = "1">
    <argument name = "routing key" type = "char*" />
    <argument name = "arguments" type = "icl_longstr_t *" />
    <possess>
    arguments = icl_longstr_dup (arguments);
    routing_key = icl_mem_strdup (routing_key);
    </possess>
    <release>
    icl_longstr_destroy (&arguments);
    icl_mem_free (routing_key);
    </release>
    //
    <action>
    amq_peering_bind (self->peering, self->exchange->name,
        amq_exchange_type_name (self->exchange->type), self->exchange->durable,
        self->exchange->auto_delete, routing_key, arguments);
    </action>
</method>

<method name = "message published" template = "async function" async = "1">
    <argument name = "content" type = "amq_content_basic_t *" />
    <argument name = "mandatory" type = "Bool" />
    <argument name = "immediate" type = "Bool" />
    <possess>
    content = amq_content_basic_link (content);
    </possess>
    <release>
    amq_content_basic_unlink (&content);
    </release>
    //
    <action>
    amq_peering_forward (
        self->peering,
        self->exchange->name,
        content->routing_key,
        content,
        mandatory,
        immediate);
    </action>
</method>

<private name = "header">
static int
    s_content_handler (void *vself, amq_peering_t *peering, amq_peer_method_t *peer_method);
static int
    s_return_handler (void *vself, amq_peering_t *peering, amq_peer_method_t *peer_method);
</private>

<private name = "footer">
static int
s_content_handler (
    void *vself,
    amq_peering_t *peering,
    amq_peer_method_t *peer_method)
{
    amq_cluster_mta_t
        *self = (amq_cluster_mta_t *) vself;
    amq_client_method_t
        *client_method;

    if (self->mode == AMQ_MTA_MODE_SUBSCRIBER ||
          self->mode == AMQ_MTA_MODE_BOTH) {

        assert (peer_method->class_id == AMQ_PEER_BASIC);
        assert (peer_method->method_id == AMQ_PEER_BASIC_DELIVER);

        //  TODO: Implement rejected messages returning in pull model
        //  Notice two last arguments in the call bellow
        client_method = amq_client_method_new_basic_publish (
            0,                          //  Access ticket
            peer_method->payload.basic_deliver.exchange,
            peer_method->payload.basic_deliver.routing_key,
            FALSE,                      //  Mandatory
            FALSE);                     //  Immediate

        client_method->content = peer_method->content;
        peer_method->content = NULL;

        amq_exchange_publish (self->exchange, NULL, (amq_server_method_t *) client_method);
        amq_client_method_unlink (&client_method);
    }
    return (0);
}

static int
s_return_handler (
    void *vself,
    amq_peering_t *peering,
    amq_peer_method_t *peer_method)
{
    amq_cluster_mta_t
        *self = (amq_cluster_mta_t *) vself;
    char
        *separator;
    icl_shortstr_t
        sender_id;
    dbyte
        channel_nbr;
    amq_server_connection_t
        *connection;

    assert (peer_method->class_id == AMQ_PEER_BASIC);
    assert (peer_method->method_id == AMQ_PEER_BASIC_RETURN);
    assert (peer_method->content);

    if (self->mode == AMQ_MTA_MODE_FORWARD_ALL
    ||  self->mode == AMQ_MTA_MODE_FORWARD_ELSE
    ||  self->mode == AMQ_MTA_MODE_BOTH) {
        //  Split sender-id "connection-key|channel-nbr" into fields
        icl_shortstr_cpy (sender_id, 
            amq_content_basic_get_sender_id ((amq_content_basic_t *)peer_method->content));
        separator = strchr (sender_id, '|');

        //  Does this assertion mean we can crash the server by sending it junk?
        assert (separator);
        *separator++ = 0;               //  Split sender-id into fields
        channel_nbr = atoi (separator);
        assert (channel_nbr);

        //  Find the connection that sent the message
        connection = amq_server_connection_table_search (amq_broker->connections, sender_id);

        if (connection) {
            amq_server_agent_basic_return (
                connection->thread,
                channel_nbr,
                peer_method->content,
                peer_method->payload.basic_return.reply_code,
                peer_method->payload.basic_return.reply_text,
                peer_method->payload.basic_return.exchange,
                peer_method->payload.basic_return.routing_key,
                NULL);

            amq_server_connection_unlink (&connection);
        }
    }
    return (0);
}
</private>

<method name = "selftest" />
</class>

