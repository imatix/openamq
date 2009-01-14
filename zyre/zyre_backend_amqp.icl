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
    name      = "zyre_backend_amqp"
    comment   = "Smart link to remote AMQP server"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
Backend module that talks to an OpenAMQ server, originally based on
the amq_peering class, but modified to work with IPR portals and to
implement the RestMS data model.

This module implementation replicates the state used by the Zyre RestMS
server.  Namely:

  * A set of feeds, pipes, and joins, implemented on the AMQP server as
    exchanges, queues, bindings, and consumers.  This peering class is the
    place where the mapping from RestMS to AMQP is defined in detail.

  * A queue of messages waiting to be published to the AMQP server.

The peering isolates the application from the AMQP server, allowing the
state to be updated asynchronously from any connection.  That is, before
a connection is made, while a connection is active, or when a connection
has died and is being brought back.

Lastly, the peering will invoke callback methods to tell you when the peer
link becomes active, and when a message content arrives.
</doc>

<inherit class = "zyre_backend_module_back" />
<inherit class = "icl_init" />

<context>
    zyre_backend_module_t
        *portal;                        //  Portal back to client
    Bool
        enabled,                        //  We're active
        connected,                      //  We're connected
        offlined;                       //  Peer has gone offline
    icl_shortstr_t
        host,                           //  Peer host name
        queue;                          //  For replies from services
    icl_longstr_t
        *auth_data;                     //  Authentication data
    int
        trace;                          //  Trace level
    smt_thread_t
        *peer_agent_thread;             //  Active agent thread if any
    dbyte
        channel_nbr;                    //  Active channel number

    zyre_pipe_table_t
        *pipe_table;                    //  Table of defined pipes
    zyre_feed_table_t
        *feed_table;                    //  Table of defined feeds
    ipr_looseref_list_t
        *messages;                      //  Messages pending
</context>

<!-- ********  The following methods form part of the peering API  ******** -->

<method name = "new">
    self->messages = ipr_looseref_list_new ();
    self->pipe_table = zyre_pipe_table_new ();
    self->feed_table = zyre_feed_table_new ();
    ipr_str_random (self->queue, "ZYRE-AAAAAAAA");
</method>

<method name = "destroy">
    <action>
    zyre_peer_method_t
        *method;                        //  Queue.Bind method

    self->portal = NULL;
    s_terminate_peering (self);
    icl_longstr_destroy (&self->auth_data);

    while ((method = (zyre_peer_method_t *) ipr_looseref_pop (self->messages)))
        zyre_peer_method_unlink (&method);
    ipr_looseref_list_destroy (&self->messages);
    zyre_pipe_table_destroy (&self->pipe_table);
    zyre_feed_table_destroy (&self->feed_table);
    </action>
</method>

<method name = "start">
    <action>
    ipr_config_t
        *config;                        //  Current server config file
    char
        *login = "peering";             //  Local user name for credentials

    config = ipr_config_dup (zyre_peer_config->config);
    ipr_config_locate (config, "/config/security", "plain");
    if (config->located)
        ipr_config_locate (config, "user", login);
    if (config->located) {
        icl_longstr_destroy (&self->auth_data);
        self->auth_data = ipr_sasl_plain_encode (login, ipr_config_get (config, "password", ""));
    }
    else {
        smt_log_print (http_server->alert_log,
            "E: login credentials for '%s' not defined - cannot connect", login);
        smt_log_print (http_server->alert_log,
            "E: please check security section in configuration, and restart");
        exit (EXIT_FAILURE);
    }
    ipr_config_destroy (&config);

    //  We  don't link to the portal since the portal wraps this object and
    //  destroys it.  The portal receives responses back from the smt thread
    //  in zyre_peer_agent.
    self->portal = portal;
    self->enabled = TRUE;

    //  Launch monitor to do actual connection
    smt_timer_request_delay (self->thread, 100 * 1000, monitor_event);
    </action>
</method>

<method name = "stop">
    <action>
    //  Tell monitor to disconnect, if connected
    self->enabled = FALSE;
    </action>
</method>

<method name = "pipe create">
    <action>
    zyre_pipe_t
        *pipe;

    pipe = zyre_pipe_table_search (self->pipe_table, pipe_name);
    if (pipe)
        assert (streq (pipe->class, pipe_class));
    else {
        pipe = zyre_pipe_new (self->pipe_table, pipe_class, pipe_name);
        if (self->connected)
            zyre_peer_agent_restms_pipe_create (
                self->peer_agent_thread,
                self->channel_nbr,
                pipe_class,
                pipe_name);
    }
    zyre_pipe_unlink (&pipe);
    </action>
</method>

<method name = "pipe delete">
    <action>
    zyre_pipe_t
        *pipe;

    pipe = zyre_pipe_table_search (self->pipe_table, pipe_name);
    assert (pipe);
    if (self->connected)
        zyre_peer_agent_restms_pipe_delete (
            self->peer_agent_thread,
            self->channel_nbr,
            pipe_name);
    zyre_pipe_destroy (&pipe);
    </action>
</method>

<method name = "feed create">
    <action>
    zyre_feed_t
        *feed;

    feed = zyre_feed_table_search (self->feed_table, feed_name);
    if (feed)
        assert (streq (feed->class, feed_class));
    else {
        feed = zyre_feed_new (self->feed_table, feed_class, feed_name);
        if (self->connected)
            zyre_peer_agent_restms_feed_create (
                self->peer_agent_thread,
                self->channel_nbr,
                feed_class,
                feed_name);
    }
    zyre_feed_unlink (&feed);
    </action>
</method>

<method name = "feed delete">
    <action>
    zyre_feed_t
        *feed;

    feed = zyre_feed_table_search (self->feed_table, feed_name);
    assert (feed);
    if (self->connected)
        zyre_peer_agent_restms_feed_delete (
            self->peer_agent_thread,
            self->channel_nbr,
            feed_name);
    zyre_feed_destroy (&feed);
    </action>
</method>

<method name = "join create">
    <action>
    zyre_pipe_t
        *pipe;
    zyre_join_t
        *join;

    pipe = zyre_pipe_table_search (self->pipe_table, pipe_name);
    assert (pipe);
    join = zyre_join_lookup (pipe, feed_name, address);
    if (join == NULL) {
        join = zyre_join_new (pipe, address, feed_name, feed_class);
        if (self->connected)
            zyre_peer_agent_restms_join_create (
                self->peer_agent_thread,
                self->channel_nbr,
                pipe->class,
                pipe->name,
                join->address,
                join->feed_name,
                join->feed_class);
    }
    zyre_join_unlink (&join);
    zyre_pipe_unlink (&pipe);
    </action>
</method>

<method name = "join delete">
    <action>
    zyre_pipe_t
        *pipe;
    zyre_join_t
        *join;

    pipe = zyre_pipe_table_search (self->pipe_table, pipe_name);
    if (pipe) {
        join = zyre_join_lookup (pipe, feed_name, address);
        if (join) {
            if (self->connected)
                zyre_peer_agent_restms_join_delete (
                    self->peer_agent_thread,
                    self->channel_nbr,
                    pipe_name,
                    address,
                    feed_name);
            zyre_join_destroy (&join);
        }
        zyre_pipe_unlink (&pipe);
    }
    </action>
</method>

<method name = "address post">
    <action>
    zyre_peer_method_t
        *method;
    zyre_feed_t
        *feed;

    feed = zyre_feed_table_search (self->feed_table, feed_name);
    assert (feed);
    if (feed->as_queue) {
        method = zyre_peer_method_new_basic_publish (
            0,                          //  Ticket
            "amq.direct",               //  Direct exchange
            feed_name,                  //  Routing key is queue name
            FALSE,                      //  Not mandatory
            FALSE);                     //  Not immediate
    }
    else
        method = zyre_peer_method_new_basic_publish (
            0,                          //  Ticket
            feed_name,                  //  Feed exchange
            address,                    //  Routing key is address
            FALSE,                      //  Not mandatory
            FALSE);                     //  Not immediate

    zyre_feed_unlink (&feed);
    method->content = amq_content_basic_link (content);

    //  We send the message if we can, else we queue it for later
    if (self->connected) {
        zyre_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
        zyre_peer_method_unlink (&method);
    }
    else
        ipr_looseref_queue (self->messages, method);
    </action>
</method>

<method name = "forward">
    <action>
    zyre_peer_method_t
        *method;

    //  Create a Basic.Publish method
    method = zyre_peer_method_new_basic_publish (
        0, exchange, routing_key, mandatory, immediate);
    method->content = amq_content_basic_link (content);

    //  We only hold forwards if the connection is currently down
    if (self->connected) {
        zyre_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
        zyre_peer_method_unlink (&method);
    }
    else
        ipr_looseref_queue (self->messages, method);
    </action>
</method>

<method name = "online partial">
    <action>
    self->channel_nbr = 1;
    zyre_peer_agent_channel_open (self->peer_agent_thread, self->channel_nbr);
    </action>
</method>

<method name = "online final">
    <action>
    zyre_peer_method_t
        *method;                        //  Method to send to peer server

    if (!self->connected) {
        self->connected = TRUE;
        self->offlined = FALSE;

        //  Create first feeds, then pipes, then joins on AMQP server
        zyre_feed_table_apply (self->feed_table, s_synchronize_feed, self);
        zyre_pipe_table_apply (self->pipe_table, s_synchronize_pipe, self);

        //  Forward all pending messages to AMQP server
        while ((method = (zyre_peer_method_t *) ipr_looseref_pop (self->messages))) {
            zyre_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
            zyre_peer_method_unlink (&method);
        }
        //  Create private queue on peer and consume off queue
        zyre_peer_agent_queue_declare (
            self->peer_agent_thread, self->channel_nbr, 0, self->queue,
            FALSE, FALSE, TRUE, TRUE, TRUE, NULL);
        zyre_peer_agent_basic_consume (
            self->peer_agent_thread, self->channel_nbr, 0, self->queue,
            NULL, TRUE, TRUE, TRUE, TRUE, NULL);
        zyre_peer_agent_queue_bind (
            self->peer_agent_thread, self->channel_nbr, 0, self->queue,
            "amq.direct", self->queue, TRUE, NULL);
    }
    zyre_backend_module_response_online (self->portal, self->queue);
    </action>
</method>

<!-- ********  The following methods are for internal use only  ******** -->

<method name = "initialise">
    <doc>
    Shadow the main config, generated peer agent uses 'zyre_peer_config'
    Then start asynchronous client connection (zyre_peer_agent)
    </doc>
    zyre_peer_config = zyre_peer_config_new ();
    zyre_peer_config_shadow (zyre_peer_config, zyre_config->config);
    zyre_peer_agent_init ();
</method>

<method name = "terminate">
    zyre_peer_agent_term ();
    zyre_peer_config_destroy (&zyre_peer_config);
</method>

<!-- Peering monitor, updates status once per second.
     Monitor the remote AMQP server. If the server was offline and has
     come online, connect to it.  If the server has gone offline, close
     the connection.
 -->
<event name = "monitor">
    <action>
    //  Terminate the peering if the application asked for it, or the
    //  peer agent thread was closed from the network side.
    if (self->peer_agent_thread && (!self->enabled || self->peer_agent_thread->zombie))
        s_terminate_peering (self);
    else
    //  Connect the peering if we're not already connected but the app
    //  has for the peering to become active.
    if (!self->peer_agent_thread && self->enabled)
        self->peer_agent_thread = zyre_peer_agent_connection_thread_new (
            self->portal,               //  Callback for incoming methods
            zyre_config_amqp_hostname (zyre_config),
            "/",                        //  Virtual host
            self->auth_data,
            "Zyre peering",             //  Instance name
            zyre_config_amqp_trace   (zyre_config),
            zyre_config_amqp_timeout (zyre_config),
            zyre_config_amqp_debug   (zyre_config));

    //  Peering monitor runs once per second
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<private name = "async header">
static void s_terminate_peering ($(selftype) *self);
static void s_synchronize_pipe  (zyre_pipe_t *pipe, void *argument);
static void s_synchronize_feed  (zyre_feed_t *feed, void *argument);
</private>

<private name = "async footer">
static void
s_terminate_peering ($(selftype) *self)
{
    //  Stop peer agent thread if it's still alive
    if (self->peer_agent_thread) {
        if (!self->peer_agent_thread->zombie)
            zyre_peer_agent_connection_close (self->peer_agent_thread);
        smt_thread_unlink (&self->peer_agent_thread);
    }
    if (self->connected) {
        self->connected = FALSE;
        self->offlined  = TRUE;
        if (self->portal)
            zyre_backend_module_response_offline (self->portal);
    }
}

static void s_synchronize_pipe (zyre_pipe_t *pipe, void *argument)
{
    zyre_join_t
        *join;                          //  Each join for pipe
    $(selftype)
        *self = ($(selftype) *) argument;

    zyre_peer_agent_restms_pipe_create (
        self->peer_agent_thread,
        self->channel_nbr,
        pipe->class,
        pipe->name);

    //  Synchronize all joins for this pipe
    join = zyre_join_list_first (pipe->join_list);
    while (join) {
        zyre_peer_agent_restms_join_create (
            self->peer_agent_thread,
            self->channel_nbr,
            pipe->class,
            pipe->name,
            join->address,
            join->feed_name,
            join->feed_class);
        join = zyre_join_list_next (&join);
    }
}

static void s_synchronize_feed (zyre_feed_t *feed, void *argument)
{
    $(selftype)
        *self = ($(selftype) *) argument;

    zyre_peer_agent_restms_feed_create (
        self->peer_agent_thread,
        self->channel_nbr,
        feed->class,
        feed->name);
}
</private>

<method name = "selftest" />

</class>
