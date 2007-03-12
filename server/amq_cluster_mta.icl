<?xml?>
<!--
    Copyright (c) 1996-2007 iMatix Corporation

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
    <doc>
    Creates a new MTA instance connected to the specified remote server and
    exchange, using the specified operation mode.
    </doc>
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "virtual host" type = "char *" />
    <argument name = "login" type = "char *" />
    <argument name = "exchange" type = "amq_exchange_t *" />
    <argument name = "mode" type = "int" />
    //
    assert (AMQ_MTA_MODE_VALID (mode));
    
    self->exchange = amq_exchange_link (exchange);
    self->mode = mode;
    self->peering = amq_peering_new (host, virtual_host, amq_server_config_trace (amq_server_config),
                                     self->exchange->name, amq_exchange_type_name (self->exchange->type),
                                     self->exchange->durable, self->exchange->auto_delete);
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
    <doc>
    Called from amq_binding constructor to notify MTA of binding creation.
    </doc>
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
    amq_peering_bind (self->peering, routing_key, arguments);
    </action>
</method>

<method name = "binding destroyed" template = "async function" async = "1">
    <doc>
    Called from amq_binding destructor to notify MTA of binding destruction.
    </doc>
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
    amq_peering_unbind (self->peering, routing_key, arguments);
    </action>
</method>

<method name = "message published" template = "async function" async = "1">
    <doc>
    Called from parent exchange publish method, publishes a message to
    the remote exchange.
    </doc>
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
//  Callback handler called by our amq_peering instance when a basic.deliver is
//  received from the peering connection.
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

//  Callback handler called by our amq_peering instance when a basic.return is
//  received from the peering connection.
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

