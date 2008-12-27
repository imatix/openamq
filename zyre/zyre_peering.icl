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
    name      = "zyre_peering"
    comment   = "Smart link to remote AMQP server"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class is based on the amq_peering class from OpenAMQ and should be
maintained in parallel.  A peering is a smart link to a remote AMQP server
that monitors the remote server, logs in when the server becomes available,
and replicates some state to that remote server.

This peering implementation replicates the state used by the Zyre RestMS
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

The selftest method demonstrates fairly sophisticated peering use.  To test
this, set the environment variable ZYRE_PEERING_TEST=1 and run an amq_server.

This is a summary of the zyre_peering API.  The order of arguments follows
the order of URI components for the resource being accessed:

    peering = zyre_peering_new (
        remote-host-name, trace-level)
        Create a new peering to the specified host.

    zyre_peering_set_login (peering, login-name)
        Tell the peering to login using the specified login-name, which
        must be defined in the local zyre_base.cfg or zyre.cfg.

    zyre_peering_set_status_handler (peering, handler, calling-object)
        Set the callback handler for status updates.

    zyre_peering_set_content_handler (peering, handler, calling-object)
        Set the callback handler for incoming messages.

    zyre_peering_start (peering)
        Enable the peering, connect to remote server when possible.

    zyre_peering_stop (peering)
        Disable the peering, disconnect from remote server if necessary.

    zyre_peering_pipe_create (peering, pipe-class, pipe-name)
        Add a pipe to the set of defined popes and create the pipe resources
        on the remote server.

    zyre_peering_pipe_delete (peering, pipe-name)
        Remove a pipe from the set of defined pipes and delete all pipe
        resources on the remote server.

    zyre_peering_feed_create (peering, feed-class, feed-name)
        Add a feed to the set of defined feeds and create the feed on the
        remote server.

    zyre_peering_feed_delete (peering, feed-name)
        Remove a feed from the set of defined feeds and delete the feed
        on the remote server.

    zyre_peering_join_create (peering, pipe-name, address, feed-name, feed-class)
        Add a join to the set of defined joins and create the join on the
        remote server.  The pipe and feed must already exist.

    zyre_peering_join_delete (peering, pipe-name, address, feed-name)
        Remove a join from the set of defined joins and delete all resources
        for the join on the remote server.

    zyre_peering_address_post (peering, address, feed-name, content)
        Publish a message to the remote server.  The feed must already exist.

    zyre_peering_destroy (&peering)
        Destroy the peering.
</doc>

<inherit class = "smt_object" />
<inherit class = "icl_init" />

<import class = "zyre_classes" />

<public name = "header">
//  Make a forward reference to resolve the circular dependency between the
//  function typedef and the method structure.
typedef struct _zyre_peer_method_t __zyre_peer_method_t;
typedef int (zyre_peering_status_fn) (
    void *caller, zyre_peering_t *peering, Bool connected);
typedef int (zyre_peering_content_fn) (
    void *caller, zyre_peering_t *peering, __zyre_peer_method_t *method);
typedef int (zyre_peering_return_fn) (
    void *caller, zyre_peering_t *peering, __zyre_peer_method_t *method);
</public>

<context>
    Bool
        enabled,                        //  We're active
        connected,                      //  We're connected
        offlined;                       //  Peer has gone offline
    icl_shortstr_t
        host;                           //  Peer host name
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

    //  Callbacks into caller object
    zyre_peering_status_fn
        *status_fn;                     //  Status change callback
    void
        *status_caller;                 //  Object to invoke
    zyre_peering_content_fn
        *content_fn;                    //  Content delivery callback
    void
        *content_caller;                //  Object to invoke
    zyre_peering_return_fn
        *return_fn;                     //  Returned message delivery callback
    void
        *return_caller;                 //  Object to invoke
</context>

<!-- ********  The following methods form part of the peering API  ******** -->

<method name = "new">
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "trace" type = "int">Trace level, 0 - 3</argument>
    //
    icl_shortstr_cpy (self->host, host);
    self->trace = trace;
    self->messages = ipr_looseref_list_new ();
    self->pipe_table = zyre_pipe_table_new ();
    self->feed_table = zyre_feed_table_new ();
</method>

<method name = "destroy">
    <action>
    zyre_peer_method_t
        *method;                        //  Queue.Bind method

    s_terminate_peering (self);
    icl_longstr_destroy (&self->auth_data);

    while ((method = (zyre_peer_method_t *) ipr_looseref_pop (self->messages)))
        zyre_peer_method_unlink (&method);
    ipr_looseref_list_destroy (&self->messages);
    zyre_pipe_table_destroy (&self->pipe_table);
    zyre_feed_table_destroy (&self->feed_table);
    </action>
</method>

<method name = "set login" template = "function">
    <doc>
    Sets the login credentials using the specified login name; the password
    is taken from the peering configuration file.
    </doc>
    <argument name = "login" type = "char *">Login name to use</argument>
    <local>
    ipr_config_t
        *config;                        //  Current server config file
    </local>
    //
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
        smt_log_print (http_server->error_log,
            "E: please check security section in configuration, and restart");
        exit (EXIT_FAILURE);
    }
    ipr_config_destroy (&config);
</method>

<method name = "set status handler" template = "function">
    <doc>
    Register a status callback handler. The callback is invoked when
    the peering connection changes status (connected or disconnected).
    If the callback function is NULL, no status callbacks will be made.
    </doc>
    <argument name = "status_fn" type = "zyre_peering_status_fn *">Callback</argument>
    <argument name = "caller" type = "void *">Reference of caller object</argument>
    //
    self->status_fn = status_fn;
    self->status_caller = caller;
</method>

<method name = "cancel status handler" template = "function">
    <doc>
    Cancel a status callback handler.
    </doc>
    <argument name = "caller" type = "void *">Reference of caller object</argument>
    //
    self->status_fn = NULL;
    self->status_caller = NULL;
</method>

<method name = "set content handler" template = "function">
    <doc>
    Register a basic_deliver callback handler.  The callback in invoked
    when a basic message arrives from the AMQP server. If the callback
    function is NULL, no basic delivery callbacks will be made.
    </doc>
    <argument name = "content_fn" type = "zyre_peering_content_fn *">Callback</argument>
    <argument name = "caller" type = "void *">Reference of caller object</argument>
    //
    self->content_fn = content_fn;
    self->content_caller = caller;
</method>

<method name = "cancel content handler" template = "function">
    <doc>
    Cancel a basic_deliver callback handler.
    </doc>
    <argument name = "caller" type = "void *">Reference of caller object</argument>
    //
    self->content_fn = NULL;
    self->content_caller = NULL;
</method>

<method name = "set return handler" template = "function">
    <doc>
    Register a basic_return callback handler.  The callback in invoked
    when a returned basic message arrives from the AMQP server. If the
    callback function is NULL, no basic return callbacks will be made.
    </doc>
    <argument name = "return_fn" type = "zyre_peering_return_fn *">Callback</argument>
    <argument name = "caller" type = "void *">Reference of caller object</argument>
    //
    self->return_fn = return_fn;
    self->return_caller = caller;
</method>

<method name = "cancel return handler" template = "function">
    <doc>
    Cancel a basic_return callback handler.
    </doc>
    <argument name = "caller" type = "void *">Reference of caller object</argument>
    //
    self->return_fn = NULL;
    self->return_caller = NULL;
</method>

<method name = "pipe create" template = "async function" async = "1">
    <doc>
    Add a pipe to the set of defined pipes and create the pipe on the
    remote server.
    </doc>
    <argument name = "pipe class" type = "char *" />
    <argument name = "pipe name" type = "char *" />
    //
    <possess>
    pipe_class = icl_mem_strdup (pipe_class);
    pipe_name = icl_mem_strdup (pipe_name);
    </possess>
    <release>
    icl_mem_free (pipe_class);
    icl_mem_free (pipe_name);
    </release>
    //
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

<method name = "pipe delete" template = "async function" async = "1">
    <doc>
    Remove a pipe from the set of defined pipes and delete the pipe on the
    remote server.
    </doc>
    <argument name = "pipe name" type = "char *" />
    //
    <possess>
    pipe_name = icl_mem_strdup (pipe_name);
    </possess>
    <release>
    icl_mem_free (pipe_name);
    </release>
    //
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

<method name = "feed create" template = "async function" async = "1">
    <doc>
    Add a feed to the set of defined feeds and create the feed on the
    remote server.
    </doc>
    <argument name = "feed class" type = "char *" />
    <argument name = "feed name" type = "char *" />
    //
    <possess>
    feed_class = icl_mem_strdup (feed_class);
    feed_name = icl_mem_strdup (feed_name);
    </possess>
    <release>
    icl_mem_free (feed_class);
    icl_mem_free (feed_name);
    </release>
    //
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

<method name = "feed delete" template = "async function" async = "1">
    <doc>
    Remove a feed from the set of defined feeds and delete the feed
    on the remote server.
    </doc>
    <argument name = "feed name" type = "char *" />
    //
    <possess>
    feed_name = icl_mem_strdup (feed_name);
    </possess>
    <release>
    icl_mem_free (feed_name);
    </release>
    //
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

<method name = "join create" template = "async function" async = "1">
    <doc>
    Add a join to the set of defined joins and create the join on the remote
    server.  The pipe and feed must already exist.  Does not validate the
    pipe class, this must be done by the caller.
    </doc>
    <argument name = "pipe name" type = "char *" />
    <argument name = "address" type = "char *">Address to join</argument>
    <argument name = "feed name" type = "char *" />
    <argument name = "feed class" type = "char *" />
    //
    <possess>
    pipe_name = icl_mem_strdup (pipe_name);
    address = icl_mem_strdup (address);
    feed_name = icl_mem_strdup (feed_name);
    feed_class = icl_mem_strdup (feed_class);
    </possess>
    <release>
    icl_mem_free (pipe_name);
    icl_mem_free (address);
    icl_mem_free (feed_name);
    icl_mem_free (feed_class);
    </release>
    //
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

<method name = "join delete" template = "async function" async = "1">
    <doc>
    Remove a join from the set of defined joins and delete all resources for
    the join on the remote server.  Neither the pipe nor feed need exist.
    </doc>
    <argument name = "pipe name" type = "char *" />
    <argument name = "feed name" type = "char *" />
    <argument name = "address" type = "char *">Address to join</argument>
    //
    <possess>
    pipe_name = icl_mem_strdup (pipe_name);
    feed_name = icl_mem_strdup (feed_name);
    address = icl_mem_strdup (address);
    </possess>
    <release>
    icl_mem_free (pipe_name);
    icl_mem_free (feed_name);
    icl_mem_free (address);
    </release>
    //
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

<method name = "address post" template = "async function" async = "1">
    <doc>
    Publishes a message to the specified address@feed on the AMQP server.
    If the feed is an exchange, the address is used as the message routing
    key and the feed is the exchange for the publish method.  If the feed
    is a shared queue, the address is used as the message id and the default
    exchange is used as the exchange for the publish method.  Does not
    validate the feed class, this must be done by the caller.
    </doc>
    <argument name = "address" type = "char *" />
    <argument name = "feed name" type = "char *" />
    <argument name = "content" type = "amq_content_basic_t *">Content to publish</argument>
    //
    <possess>
    address = icl_mem_strdup (address);
    feed_name = icl_mem_strdup (feed_name);
    content = amq_content_basic_link (content);
    </possess>
    <release>
    icl_mem_free (address);
    icl_mem_free (feed_name);
    amq_content_basic_unlink (&content);
    </release>
    //
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

<method name = "start" template = "async function" async = "1">
    <doc>
    Start the peering. You must call this method to start the peering.
    </doc>
    //
    <action>
    self->enabled = TRUE;
    smt_timer_request_delay (self->thread, 100 * 1000, monitor_event);
    </action>
</method>

<method name = "stop" template = "async function" async = "1">
    <doc>
    Stop the peering. If you want to restart the peering, use the start
    method.
    </doc>
    //
    <action>
    self->enabled = FALSE;
    </action>
</method>

<!-- ********  The following methods are for internal use only  ******** -->

<method name = "initialise">
    //  Load peering configuration data from zyre.cfg if present
    //  Then start asynchronous client connection (zyre_peer_agent)
    zyre_peer_config = zyre_peer_config_new ("zyre_base.cfg", "zyre.cfg", FALSE);
    zyre_peer_agent_init ();
</method>

<method name = "terminate">
    zyre_peer_agent_term ();
    zyre_peer_config_destroy (&zyre_peer_config);
</method>

<!-- The following async methods handle specific protocol replies from
     the AMQP server -->

<method name = "protocol handler" template = "async function" async = "1" abstract = "1">
    <argument name = "method" type = "zyre_peer_method_t *" />
    <possess>
    method = zyre_peer_method_link (method);
    </possess>
    <release>
    zyre_peer_method_unlink (&method);
    </release>
</method>

<method name = "connection open ok" template = "protocol handler">
    <action>
    self->channel_nbr = 1;              //  Single channel per connection
    zyre_peer_agent_channel_open (self->peer_agent_thread, self->channel_nbr);
    </action>
</method>

<method name = "connection close" template = "protocol handler">
    <action>
    //  If the connection failed due to a hard error, complain loudly and
    //  shut down the broker.  We really don't want people using servers
    //  with badly defined peerings.
    if (ASL_HARD_ERROR (method->payload.connection_close.reply_code)) {
        smt_log_print (http_server->alert_log,
            "E: hard error on peering - fix and restart server");
        smt_shut_down ();
    }
    </action>
</method>

<method name = "channel open ok" template = "protocol handler">
    <action>
    s_initialise_peering (self);
    </action>
</method>

<method name = "basic deliver" template = "protocol handler">
    <action>
    if (self->content_fn)
        (self->content_fn) (self->content_caller, self, method);
    </action>
</method>

<method name = "basic return" template = "protocol handler">
    <action>
    if (self->return_fn)
        (self->return_fn) (self->return_caller, self, method);
    </action>
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
            self,                       //  Callback for incoming methods
            self->host,
            "/",
            self->auth_data,
            "Zyre peering",             //  Instance name
            self->trace,
            zyre_config_amqp_timeout (zyre_config),
            zyre_config_amqp_debug (zyre_config));

    //  Peering monitor runs once per second
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<private name = "async header">
static void s_initialise_peering (zyre_peering_t *self);
static void s_terminate_peering  (zyre_peering_t *self);
static void s_synchronize_pipe   (zyre_pipe_t *pipe, void *argument);
static void s_synchronize_feed   (zyre_feed_t *feed, void *argument);
</private>

<private name = "async footer">
static void
s_initialise_peering (zyre_peering_t *self)
{
    zyre_peer_method_t
        *method;                        //  Method to send to peer server
    //
    if (!self->connected) {
        self->connected = TRUE;
        self->offlined = FALSE;
        smt_log_print (http_server->alert_log,
            "I: Zyre is now peered to OpenAMQ server on %s", self->host);

        //  Create first feeds, then pipes, then joins on AMQP server
        zyre_feed_table_apply (self->feed_table, s_synchronize_feed, self);
        zyre_pipe_table_apply (self->pipe_table, s_synchronize_pipe, self);

        //  Forward all pending messages to AMQP server
        while ((method = (zyre_peer_method_t *) ipr_looseref_pop (self->messages))) {
            zyre_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
            zyre_peer_method_unlink (&method);
        }
        if (self->status_fn)
            (self->status_fn) (self->status_caller, self, TRUE);
    }
}

static void
s_terminate_peering (zyre_peering_t *self)
{
    http_server_t
        *server_ref;

    //  Stop peer agent thread if it's still alive
    if (self->peer_agent_thread) {
        if (!self->peer_agent_thread->zombie)
            zyre_peer_agent_connection_close (self->peer_agent_thread);
        smt_thread_unlink (&self->peer_agent_thread);
    }
    if (self->connected) {
        self->connected = FALSE;
        self->offlined  = TRUE;

        if (self->status_fn)
            (self->status_fn) (self->status_caller, self, FALSE);

        //  Use server with caution because it can be destroyed now
        server_ref = http_server_link (http_server);
        if (server_ref) {
            smt_log_print (server_ref->alert_log,
                "I: Zyre unpeered from OpenAMQ server at %s", self->host);
            http_server_unlink (&server_ref);
        }
    }
}

static void s_synchronize_pipe (zyre_pipe_t *pipe, void *argument)
{
    zyre_join_t
        *join;                          //  Each join for pipe
    zyre_peering_t
        *self = (zyre_peering_t *) argument;

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
    zyre_peering_t
        *self = (zyre_peering_t *) argument;

    zyre_peer_agent_restms_feed_create (
        self->peer_agent_thread,
        self->channel_nbr,
        feed->class,
        feed->name);
}
</private>

<private name = "header">
//  These functions are examples of callbacks used in the peering API

static int s_test_status_handler  (
    void *caller, zyre_peering_t *peering, Bool status);
static int s_test_content_handler (
    void *caller, zyre_peering_t *peering, zyre_peer_method_t *method);
</private>

<private name = "footer">
static int
s_test_status_handler (
    void *caller,
    zyre_peering_t *peering,
    Bool status)
{
    if (status)
        icl_console_print ("I: (TEST) peering now connected to %s", peering->host);
    else
        icl_console_print ("W: (TEST) peering disconnected from %s", peering->host);

    return (0);
}

static int
s_test_content_handler (
    void *caller,
    zyre_peering_t *peering,
    zyre_peer_method_t *method)
{
    icl_console_print ("I: (TEST) received message from peering");
    return (0);
}
</private>

<method name = "selftest">
    char
        *test_flag;
    zyre_peering_t
        *peering;                       //  Peering to remote server
    int
        seconds = 1000000;              //  Microseconds per second for sleep
    amq_content_basic_t
        *content;

    //  This selftest is NOT run during normal builds
    //  To enable it, set the environment variable ZYRE_PEERING_TEST=1
    //
    test_flag = getenv ("ZYRE_PEERING_TEST");
    if (test_flag && *test_flag == '1') {
        icl_console_mode (ICL_CONSOLE_DATE, TRUE);

        //  These objects are needed for our test framework
        zyre_config = zyre_config_new ("zyre_base.cfg", NULL, FALSE);
        http_config = http_config_new ("http_base.cfg", NULL, FALSE);
        http_config_set_port (http_config, "8080");
        http_server = http_server_new ();

        //  **************   Peering example starts here   *******************
        //  Create a new peering to local AMQP server
        peering = zyre_peering_new ("localhost", 1);

        //  Set login credentials
        zyre_peering_set_login (peering, "peering");

        //  Register handlers, with no object reference, for this test
        zyre_peering_set_status_handler  (peering, s_test_status_handler, NULL);
        zyre_peering_set_content_handler (peering, s_test_content_handler, NULL);

        //  Start the peering - connects to the AMQP server
        zyre_peering_start (peering);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Creating resources");
        zyre_peering_pipe_create (peering, "pipe", "my.pipe");
        zyre_peering_feed_create (peering, "service", "my.feed.queue");
        zyre_peering_feed_create (peering, "fanout", "my.feed.exchange");
        zyre_peering_join_create (peering, "my.pipe", "*", "my.feed.queue", "service");
        zyre_peering_join_create (peering, "my.pipe", "*", "my.feed.exchange", "fanout");

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Stopping the peering...");
        zyre_peering_stop (peering);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Creating more resources");
        zyre_peering_pipe_create (peering, "pipe", "my.pipe.2");

        //  Publish a message to each feeds, check that they arrives
        content = amq_content_basic_new ();
        zyre_peering_address_post (peering, "address-1", "my.feed.queue", content);
        zyre_peering_address_post (peering, "address-2", "my.feed.exchange", content);
        amq_content_basic_unlink (&content);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Restarting the peering...");
        zyre_peering_start (peering);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Deleting resources");

        zyre_peering_join_delete (peering, "my.pipe", "*", "my.feed.exchange");
        zyre_peering_feed_delete (peering, "my.feed.queue");
        zyre_peering_feed_delete (peering, "my.feed.exchange");
        zyre_peering_pipe_delete (peering, "my.pipe.2");
        zyre_peering_pipe_delete (peering, "my.pipe");

        icl_console_print ("I: (TEST) Finished - press Ctrl-C when done");
        smt_wait (0);                   //  Execute SMT engine

        zyre_peering_destroy (&peering);
        http_server_destroy (&http_server);
        http_config_destroy (&http_config);
        zyre_config_destroy (&zyre_config);
    }
</method>

</class>
