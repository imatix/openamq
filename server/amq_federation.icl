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
    name      = "amq_federation"
    comment   = "Mesage transfer agent"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This agent implements a single federation, which is a relationship between
an exchange on this server and the same-named exchange on a parent server.
There are two families of federation, primitives that do one thing and 
compounds which combine the effects of multiple primitives.

The primitives are:

 - Subscriber federation: replicate bindings to the parent server, so that 
   messages are pulled down from it.  The subscriber federation does three
   things: send queue.bind and and queue.unbind commands to the parent 
   server (via a peering), and process messages delivered from the parent.
   Typically used on topic or header exchanges.
   
 - Publisher federation (aka 'forward all'): replicate published messages to
   the parent server.  The publisher federation does two things: forward all
   basic.publish commands to the parent server, and process undeliverable
   messages returned by the parent server. Typically used for topic or header 
   exchanges.
   
 - Locator federation (aka 'forward else'): replicate published messages to
   the parent server if they could not be delivered locally.  The locator
   federation does two things: forward basic.publish messages to the parent
   server if they could not be delivered locally, and process undeliverable
   messages returned by the parent server.  Typically used for direct 
   exchanges.
   
The compound federations are:

 - Fanout federation: combines subscriber and publisher.  Fanout federation
   does this: send queue.bind and unbind to parent server; when a local app
   publishes message, forward that to parent (and do not publish to local
   exchange); process messages delivered from the parent; process returns
   from the parent.  The result is that messages published to any server will
   be pushed up to the highest parent, and then propagated back down through
   all servers and out to applications.  This is the default federation for 
   topic and header exchanges.  The current implementation creates extra
   latency. 
 
 - Service federation: combines subscriber and locator.  Service federation
   does this: send queue.bind and unbind to parent server; when local app
   publishes message, try to deliver locally and if that fails, forward to
   parent server; process messages delivered from parent; process returns
   from the parent.  The result is that requests will go to the nearest
   consumer, and responses will come back down from parents.  This is the
   default federation for direct exchanges.
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
        type;                           //  Federation type
</context>

<public>
//  Federation types
#define AMQ_FEDERATION_NONE               0
#define AMQ_FEDERATION_SUBSCRIBER         1
#define AMQ_FEDERATION_PUBLISHER          2
#define AMQ_FEDERATION_LOCATOR            3
#define AMQ_FEDERATION_FANOUT             4
#define AMQ_FEDERATION_SERVICE            5
#define AMQ_FEDERATION_VALID(t)           (t > 0 && t < 6)
</public>

<method name = "new">
    <doc>
    Creates a new federation instance connected to the specified remote 
    server and exchange.
    </doc>
    <argument name = "exchange" type = "amq_exchange_t *">Parent exchange</argument>
    <argument name = "type" type = "unsigned int">Federation type</argument>
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "vhost" type = "char *">Virtual host identifier</argument>
    <argument name = "login" type = "char *">Peering login to use</argument>
    //
    assert (AMQ_FEDERATION_VALID (type));

    self->exchange = amq_exchange_link (exchange);
    self->type     = type;
    self->peering  = amq_peering_new (host, vhost,
        amq_server_config_trace (amq_server_config),
        self->exchange->name,
        amq_exchange_type_name (self->exchange->type));

    amq_peering_set_login           (self->peering, login);
    amq_peering_set_content_handler (self->peering, s_content_handler, self);
    amq_peering_set_return_handler  (self->peering, s_return_handler, self);
    smt_log_print (amq_broker->debug_log,
        "I: federating %s to %s (%s)", 
        self->exchange->name, host, amq_federation_type_name (self->type));

    amq_peering_start (self->peering);
</method>

<method name = "destroy">
    amq_peering_stop (self->peering);
    amq_exchange_unlink (&self->exchange);
    amq_peering_unlink (&self->peering);
</method>

<method name = "binding created" template = "async function">
    <doc>
    Called from amq_binding constructor to notify federation of binding 
    creation.
    </doc>
    <argument name = "routing key" type = "char *" />
    <argument name = "arguments"   type = "icl_longstr_t *" />
    <argument name = "exclusive"   type = "Bool" />
    <possess>
    routing_key = icl_mem_strdup (routing_key);
    arguments = icl_longstr_dup (arguments);
    </possess>
    <release>
    icl_mem_free (routing_key);
    icl_longstr_destroy (&arguments);
    </release>
    //
    <action>
    if ((self->type == AMQ_FEDERATION_SUBSCRIBER || self->type == AMQ_FEDERATION_FANOUT)
    ||  (self->type == AMQ_FEDERATION_SERVICE && exclusive)) {
        if (amq_server_config_debug_peering (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "P: bind     exchange=%s host=%s routing_key=%s", 
                self->exchange->name, self->peering->host, routing_key);
    
        amq_peering_bind (self->peering, routing_key, arguments);
    }
    </action>
</method>

<method name = "binding destroyed" template = "async function">
    <doc>
    Called from amq_binding destructor to notify federation of binding 
    destruction.
    </doc>
    <argument name = "routing key" type = "char *" />
    <argument name = "arguments"   type = "icl_longstr_t *" />
    <possess>
    routing_key = icl_mem_strdup (routing_key);
    arguments = icl_longstr_dup (arguments);
    </possess>
    <release>
    icl_mem_free (routing_key);
    icl_longstr_destroy (&arguments);
    </release>
    //
    <action>
    if (self->type == AMQ_FEDERATION_SUBSCRIBER 
    ||  self->type == AMQ_FEDERATION_FANOUT
    ||  self->type == AMQ_FEDERATION_SERVICE) {
        if (amq_server_config_debug_peering (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "P: unbind   exchange=%s host=%s routing_key=%s", 
                self->exchange->name, self->peering->host, routing_key);
    
        amq_peering_unbind (self->peering, routing_key, arguments);
    }
    </action>
</method>

<method name = "message published" template = "async function">
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

<method name = "type lookup" return = "rc">
    <doc>
    Translates a federation type name into an internal type number.  If
    the type name is not valid, returns -1, else returns one of the
    type numbers supported by this implementation.
    </doc>
    <argument name = "type name" type = "char *">Type name to lookup</argument>
    <declare name = "rc" type = "int">Type number</declare>
    //
    if (streq (type_name, "none"))
        rc = AMQ_FEDERATION_NONE;
    else
    if (streq (type_name, "subscriber"))
        rc = AMQ_FEDERATION_SUBSCRIBER;
    else
    if (streq (type_name, "publisher"))
        rc = AMQ_FEDERATION_PUBLISHER;
    else
    if (streq (type_name, "locator"))
        rc = AMQ_FEDERATION_LOCATOR;
    else
    if (streq (type_name, "fanout"))
        rc = AMQ_FEDERATION_FANOUT;
    else
    if (streq (type_name, "service"))
        rc = AMQ_FEDERATION_SERVICE;
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
    if (type == AMQ_FEDERATION_NONE)
        name = "none";
    else
    if (type == AMQ_FEDERATION_SUBSCRIBER)
        name = "subscriber";
    else
    if (type == AMQ_FEDERATION_PUBLISHER)
        name = "publisher";
    else
    if (type == AMQ_FEDERATION_LOCATOR)
        name = "locator";
    else
    if (type == AMQ_FEDERATION_FANOUT)
        name = "fanout";
    else
    if (type == AMQ_FEDERATION_SERVICE)
        name = "service";
    else
        name = "(unknown)";
</method>

<private name = "header">
static int
    s_content_handler (void *vself, amq_peering_t *peering, amq_peer_method_t *peer_method);
static int
    s_return_handler (void *vself, amq_peering_t *peering, amq_peer_method_t *peer_method);
</private>

<private name = "footer">
//  Callback handler called by our amq_peering instance when a basic.deliver 
//  is received from the peering connection.  We deliver the message to local
//  consumers, by publishing it on the exchange.
//  
static int
s_content_handler (
    void *vself,
    amq_peering_t *peering,
    amq_peer_method_t *peer_method)
{
    amq_federation_t
        *self = (amq_federation_t *) vself;
    amq_content_basic_t
        *content;

    assert (peer_method->class_id == AMQ_PEER_BASIC);
    assert (peer_method->method_id == AMQ_PEER_BASIC_DELIVER);
    assert (self->type == AMQ_FEDERATION_SUBSCRIBER 
         || self->type == AMQ_FEDERATION_FANOUT
         || self->type == AMQ_FEDERATION_SERVICE);

    if (amq_server_config_debug_peering (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "P: publish  peer exchange=%s routing_key=%s", 
            peer_method->payload.basic_deliver.exchange,
            peer_method->payload.basic_deliver.routing_key);

    content = peer_method->content;
    peer_method->content = NULL;
    amq_content_basic_set_routing_key (
        content,
        peer_method->payload.basic_deliver.exchange,
        peer_method->payload.basic_deliver.routing_key,
        0);

    amq_exchange_publish (self->exchange, NULL, content, FALSE, FALSE, AMQ_CONNECTION_GROUP_SUPER);
    amq_content_basic_unlink (&content);
    return (0);
}


//  Callback handler called by our amq_peering instance when a basic.return is
//  received from the peering connection.  This happens when the parent broker
//  fails to deliver a mandatory/immediate message.  We look for the connection
//  that originally published the message, and return the message there.
//  
static int
s_return_handler (
    void *vself,
    amq_peering_t *peering,
    amq_peer_method_t *peer_method)
{
    amq_federation_t
        *self = (amq_federation_t *) vself;
    char
        *separator;
    icl_shortstr_t
        sender_id;
    dbyte
        channel_nbr;
    amq_server_connection_t
        *connection;

    //  TODO: these assertions make it possible to crash the broker by
    //  sending it malformed data, however this would require that the
    //  parent broker is evil, we're not speaking to a client app here.
    //  
    assert (peer_method->class_id == AMQ_PEER_BASIC);
    assert (peer_method->method_id == AMQ_PEER_BASIC_RETURN);
    assert (peer_method->content);
    assert (self->type == AMQ_FEDERATION_PUBLISHER
         || self->type == AMQ_FEDERATION_LOCATOR
         || self->type == AMQ_FEDERATION_FANOUT
         || self->type == AMQ_FEDERATION_SERVICE);
    
    //  Split sender-id "connection-key|channel-nbr" into fields
    icl_shortstr_cpy (sender_id, 
        amq_content_basic_get_sender_id ((amq_content_basic_t *) peer_method->content));
    separator = strchr (sender_id, '|');

    if (separator) {
        *separator++ = 0;               //  Split sender-id into fields
        channel_nbr = atoi (separator);

        //  Find the connection that sent the message
        connection = amq_server_connection_table_search (amq_broker->connections, sender_id);
        if (connection && channel_nbr)
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
    return (0);
}
</private>

<method name = "selftest" />
</class>

