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

<todo>
  Get feed updates from OpenAMQ, if feeds are new here then create via
  main restms thread.
</todo>

<inherit class = "zyre_backend_back" />
<inherit class = "icl_init" />

<context>
    zyre_backend_t
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
    ipr_hash_table_t
        *pipe_table,                    //  Known pipes
        *feed_table;                    //  Known feeds
    ipr_looseref_list_t
        *messages;                      //  Messages pending
</context>

<!-- ********  The following methods form part of the peering API  ******** -->

<method name = "new">
    self->messages = ipr_looseref_list_new ();
    self->pipe_table = ipr_hash_table_new ();
    self->feed_table = ipr_hash_table_new ();
    ipr_str_random (self->queue, "ZYRE-AAAAAAAA");
</method>

<method name = "destroy">
    <action>
    {
    zyre_peer_method_t
        *method;                        //  Queue.Bind method

    self->portal = NULL;
    s_terminate_peering (self);
    icl_longstr_destroy (&self->auth_data);

    while ((method = (zyre_peer_method_t *) ipr_looseref_pop (self->messages)))
        zyre_peer_method_unlink (&method);
    ipr_looseref_list_destroy (&self->messages);
    ipr_hash_table_apply   (self->feed_table, s_destroy_feed, NULL);
    ipr_hash_table_apply   (self->pipe_table, s_destroy_pipe, NULL);
    ipr_hash_table_destroy (&self->pipe_table);
    ipr_hash_table_destroy (&self->feed_table);
    }
    </action>
</method>

<method name = "start">
    <action>
    ipr_config_t
        *config;                        //  Current server config file
    char
        *login = "peering";             //  Local user name for credentials

    config = ipr_config_dup (zyre_peer_config_table (zyre_peer_config));
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
    zyre_amqp_pipe_t
        *pipe;

    assert (ipr_hash_lookup (self->pipe_table, pipe_name) == NULL);
    pipe = zyre_amqp_pipe_new (pipe_type, pipe_name);
    ipr_hash_insert (self->pipe_table, pipe_name, pipe);
    if (self->connected)
        zyre_peer_agent_restms_pipe_create (
            self->peer_agent_thread,
            self->channel_nbr,
            pipe_type, pipe_name);
    </action>
</method>

<method name = "pipe delete">
    <action>
    zyre_amqp_pipe_t
        *pipe;

    pipe = ipr_hash_lookup (self->pipe_table, pipe_name);
    assert (pipe);
    if (self->connected)
        zyre_peer_agent_restms_pipe_delete (
            self->peer_agent_thread,
            self->channel_nbr,
            pipe_name);
    ipr_hash_delete (self->pipe_table, pipe_name);
    zyre_amqp_pipe_destroy (&pipe);
    </action>
</method>

<method name = "feed create">
    <action>
    zyre_amqp_feed_t
        *feed;

    assert (ipr_hash_lookup (self->feed_table, feed_name) == NULL);
    feed = zyre_amqp_feed_new (feed_type, feed_name);
    ipr_hash_insert (self->feed_table, feed_name, feed);
    if (self->connected)
        zyre_peer_agent_restms_feed_create (
            self->peer_agent_thread,
            self->channel_nbr,
            feed_type, feed_name);
    </action>
</method>

<method name = "feed delete">
    <action>
    zyre_amqp_feed_t
        *feed;

    feed = ipr_hash_lookup (self->feed_table, feed_name);
    assert (feed);
    if (self->connected)
        zyre_peer_agent_restms_feed_delete (
            self->peer_agent_thread,
            self->channel_nbr,
            feed_name);
    ipr_hash_delete (self->feed_table, feed_name);
    zyre_amqp_feed_destroy (&feed);
    </action>
</method>

<method name = "join create">
    <action>
    zyre_amqp_pipe_t
        *pipe;
    zyre_amqp_join_t
        *join;

    pipe = ipr_hash_lookup (self->pipe_table, pipe_name);
    assert (pipe);
    assert (zyre_amqp_pipe_join_lookup (pipe, feed_name, address) == NULL);
    join = zyre_amqp_join_new (pipe_name, feed_name, address);
    join->pipe_ref = ipr_looseref_queue (pipe->joins, join);
    if (self->connected)
        zyre_peer_agent_restms_join_create (
            self->peer_agent_thread,
            self->channel_nbr,
            pipe_name, feed_name, address);
    </action>
</method>

<method name = "join delete">
    <action>
    zyre_amqp_pipe_t
        *pipe;
    zyre_amqp_join_t
        *join;

    pipe = ipr_hash_lookup (self->pipe_table, pipe_name);
    assert (pipe);
    join = zyre_amqp_pipe_join_lookup (pipe, feed_name, address);
    assert (join);
    if (self->connected)
        zyre_peer_agent_restms_join_delete (
            self->peer_agent_thread,
            self->channel_nbr,
            pipe_name, feed_name, address);

    //  Pipe must still exist
    ipr_looseref_destroy (&join->pipe_ref);
    zyre_amqp_join_destroy (&join);
    </action>
</method>

<method name = "address post">
    <action>
    zyre_peer_method_t
        *method;
    zyre_amqp_feed_t
        *feed;

    feed = ipr_hash_lookup (self->feed_table, feed_name);
    if (*feed_name)
        assert (feed);                  //  Default feed is not defined

    if (*feed_name && feed->as_queue) {
        method = zyre_peer_method_new_basic_publish (
            0,                          //  Ticket
            NULL,                       //  Default exchange
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
        ipr_hash_table_apply (self->feed_table, s_synchronize_feed, self);
        ipr_hash_table_apply (self->pipe_table, s_synchronize_pipe, self);

        //  Forward all pending messages to AMQP server
        while ((method = (zyre_peer_method_t *) ipr_looseref_pop (self->messages))) {
            zyre_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
            zyre_peer_method_unlink (&method);
        }
        //  Create private queue for internal request-response
        zyre_peer_agent_queue_declare (
            self->peer_agent_thread, self->channel_nbr, 0, self->queue,
            FALSE, FALSE, TRUE, TRUE, TRUE, NULL);
        zyre_peer_agent_basic_consume (
            self->peer_agent_thread, self->channel_nbr, 0, self->queue,
            NULL, TRUE, TRUE, TRUE, TRUE, NULL);
    }
    zyre_backend_response_online (self->portal, self->queue);
    </action>
</method>

<!-- ********  The following methods are for internal use only  ******** -->

<method name = "initialise">
    <doc>
    Shadow the main config, generated peer agent uses 'zyre_peer_config'
    Then start asynchronous client connection (zyre_peer_agent)
    </doc>
    zyre_peer_config = zyre_peer_config_new ();
    zyre_peer_config_shadow (zyre_peer_config, zyre_config_table (zyre_config));
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
static void s_terminate_peering (zyre_backend_amqp_t *self);
static void s_synchronize_pipe  (ipr_hash_t *hash, void *argument);
static void s_synchronize_feed  (ipr_hash_t *hash, void *argument);
static void s_destroy_pipe      (ipr_hash_t *hash, void *argument);
static void s_destroy_feed      (ipr_hash_t *hash, void *argument);
</private>

<private name = "async footer">
static void
s_terminate_peering (zyre_backend_amqp_t *self)
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
            zyre_backend_response_offline (self->portal);
    }
}

static void s_synchronize_pipe (ipr_hash_t *hash, void *argument)
{
    ipr_looseref_t
        *looseref;
    zyre_amqp_join_t
        *join;                          //  Each join for pipe
    zyre_backend_amqp_t
        *self = (zyre_backend_amqp_t *) argument;
    zyre_amqp_pipe_t
        *pipe = (zyre_amqp_pipe_t *) hash->data;

    zyre_peer_agent_restms_pipe_create (
        self->peer_agent_thread,
        self->channel_nbr,
        pipe->type, pipe->name);

    //  Synchronize all joins for this pipe
    looseref = ipr_looseref_list_first (pipe->joins);
    while (looseref) {
        join = ((zyre_amqp_join_t *) looseref->object);
        zyre_peer_agent_restms_join_create (
            self->peer_agent_thread,
            self->channel_nbr,
            join->pipe_name, join->feed_name, join->address);
        looseref = ipr_looseref_list_next (&looseref);
    }
}

static void s_synchronize_feed (ipr_hash_t *hash, void *argument)
{
    zyre_backend_amqp_t
        *self = (zyre_backend_amqp_t *) argument;
    zyre_amqp_feed_t
        *feed = (zyre_amqp_feed_t *) hash->data;

    zyre_peer_agent_restms_feed_create (
        self->peer_agent_thread,
        self->channel_nbr,
        feed->type, feed->name);
}

static void s_destroy_pipe (ipr_hash_t *hash, void *argument)
{
    zyre_amqp_pipe_t
        *pipe = (zyre_amqp_pipe_t *) hash->data;
    zyre_amqp_join_t
        *join;                          //  Each join for pipe

    //  Destroy all joins for this pipe
    while ((join = (zyre_amqp_join_t *) ipr_looseref_pop (pipe->joins)))
        zyre_amqp_join_destroy (&join);

    zyre_amqp_pipe_destroy (&pipe);
}

static void s_destroy_feed (ipr_hash_t *hash, void *argument)
{
    zyre_amqp_feed_t
        *feed = (zyre_amqp_feed_t *) hash->data;
    zyre_amqp_feed_destroy (&feed);
}
</private>

<method name = "selftest" />

</class>
