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
    name      = "amq_peering"
    comment   = "AMQ server-to-server peering class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class defines a smart link to a remote server.  You tell the peering
what server to talk to and it will monitor the server and connect
automatically when the server comes online, and reconnect if the server
goes offline for a period.

A peering has two functions:

1. Binding propagation, in which messages published to an exchange on the
   remote server can be "pulled" to this server.  You tell the peering what
   bindings you are interested in, and it will deliver you all matching
   messages.

2. Message forwarding, in which messages originating locally are carried to
   the remote peer.  This class does not specify where those local messages
   come from.

The peer link may be arbitrarily active or inactive depending on the state
of the network and the remote server.

The peering will replay all bindings if when the peer link becomes active.
It will queue messages that are forwarded when the peer link is not active.

Lastly, the peering will invoke callback methods to tell you when the peer
link becomes active, and when a content arrives.  Currently this class
works only with Basic contents.

The selftest method demonstrates fairly sophisticated peering use.  To test
this, set the environment variable AMQ_PEERING_TEST=1 and run an amq_server.

Note that the current amq_peering implementation is limited to a signle
exchange per amq_peering instance.

This is a summary of the amq_peering API:

    peering = amq_peering_new (
        remote-host-name, virtual-host, trace-level, exchange-name)
        Create a new peering to the specified host, virtual host and
        remote exchange.

    amq_peering_set_login (peering, login-name)
        Tell the peering to login using the specified login-name, which
        must be defined in the local amq_server_base.cfg or amq_server.cfg.

    amq_peering_set_status_handler (peering, handler, calling-object)
        Set the callback handler for status updates.

    amq_peering_set_content_handler (peering, handler, calling-object)
        Set the callback handler for incoming messages.

    amq_peering_start (peering)
        Enable the peering, connect to remote server when possible.

    amq_peering_stop (peering)
        Disable the peering, disconnect from remote server if necessary.

    amq_peering_bind (peering, routing-key, arguments)
        Replicate a binding to the remote server.

    amq_peering_unbind (peering, routing-key, arguments)
        Replicate a queue.unbind to the remote server.  Also removes the
        binding from the list of bindings to replay.

    amq_peering_forward (peering, routing-key, content, madatory, immediate)
        Publish a message to the remote server.

    amq_peering_destroy (&peering)
        Destroy the peering.
</doc>

<inherit class = "smt_object" />
<inherit class = "icl_init" />

<import class = "amq_server_classes" />

<public name = "header">
//  Make a forward reference to resolve the circular dependency between the
//  function typedef and the method structure.
typedef struct _amq_peer_method_t __amq_peer_method_t;
typedef int (amq_peering_status_fn) (
    void *caller, amq_peering_t *peering, Bool connected);
typedef int (amq_peering_content_fn) (
    void *caller, amq_peering_t *peering, __amq_peer_method_t *method);
typedef int (amq_peering_return_fn) (
    void *caller, amq_peering_t *peering, __amq_peer_method_t *method);
</public>

<context>
    Bool
        enabled,                        //  We're active
        connected,                      //  We're connected
        offlined;                       //  Peer has gone offline
    icl_shortstr_t
        host,                           //  Peer host name
        virtual_host,                   //  Virtual host name
        exchange,                       //  Remote exchange name
        exchange_type;                  //  Remote exchange type
    icl_longstr_t
        *auth_data;                     //  Authentication data
    int
        trace;                          //  Trace level
    smt_thread_t
        *peer_agent_thread;             //  Active agent thread if any
    dbyte
        channel_nbr;                    //  Active channel number
    ipr_looseref_list_t
        *bindings,                      //  Bindings sent/pending
        *messages;                      //  Messages pending

    //  Callbacks into caller object
    amq_peering_status_fn
        *status_fn;                     //  Status change callback
    void
        *status_caller;                 //  Object to invoke
    amq_peering_content_fn
        *content_fn;                    //  Content delivery callback
    void
        *content_caller;                //  Object to invoke
    amq_peering_return_fn
        *return_fn;                     //  Returned message delivery callback
    void
        *return_caller;                 //  Object to invoke
</context>

<!-- ********  The following methods form part of the peering API  ******** -->

<method name = "new">
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "virtual host" type = "char *">Virtual host</argument>
    <argument name = "trace" type = "int">Trace level, 0 - 3</argument>
    <argument name = "exchange" type = "char *">Remote exchange name</argument>
    <argument name = "exchange_type" type = "char*">Remote exchange type</argument>
    //
    icl_shortstr_cpy (self->host,          host);
    icl_shortstr_cpy (self->virtual_host,  virtual_host);
    icl_shortstr_cpy (self->exchange,      exchange);
    icl_shortstr_cpy (self->exchange_type, exchange_type);
    self->trace = trace;

    //  Create binding state lists
    self->bindings = ipr_looseref_list_new ();
    self->messages = ipr_looseref_list_new ();
</method>

<method name = "destroy">
    <action>
    amq_peer_method_t
        *method;                        //  Queue.Bind method

    s_terminate_peering (self);
    icl_longstr_destroy (&self->auth_data);

    while ((method = (amq_peer_method_t *) ipr_looseref_pop (self->bindings)))
        amq_peer_method_unlink (&method);
    ipr_looseref_list_destroy (&self->bindings);

    while ((method = (amq_peer_method_t *) ipr_looseref_pop (self->messages)))
        amq_peer_method_unlink (&method);
    ipr_looseref_list_destroy (&self->messages);
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
    config = ipr_config_dup (amq_server_config_table (amq_server_config));
    ipr_config_locate (config, "/config/security", "plain");
    if (config->located)
        ipr_config_locate (config, "user", login);

    if (config->located) {
        icl_longstr_destroy (&self->auth_data);
        self->auth_data = ipr_sasl_plain_encode (login, ipr_config_get (config, "password", ""));
    }
    else {
        smt_log_print (amq_broker->alert_log,
            "E: login credentials for '%s' not defined - cannot connect", login);
        smt_log_print (amq_broker->alert_log,
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
    The caller object MUST cancel its registration when stopping.
    </doc>
    <argument name = "status_fn" type = "amq_peering_status_fn *">Callback</argument>
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
    when a basic message arrives from the remote peered server. If the
    callback function is NULL, no basic delivery callbacks will be made.
    The caller object MUST cancel its registration when stopping.
    </doc>
    <argument name = "content_fn" type = "amq_peering_content_fn *">Callback</argument>
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
    when a returned basic message arrives from the remote peered server. If the
    callback function is NULL, no basic return callbacks will be made.
    The caller object MUST cancel its registration when stopping.
    </doc>
    <argument name = "return_fn" type = "amq_peering_return_fn *">Callback</argument>
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

<method name = "start" template = "async function">
    <doc>
    Start the peering. You must call this method to start the peering.
    You can send bind methods before or after calling start; the bindings
    are queued and sent to the peer only when the connection succeeds.
    </doc>
    //
    <action>
    self->enabled = TRUE;
    smt_timer_request_delay (self->thread, 100 * 1000, monitor_event);
    </action>
</method>

<method name = "stop" template = "async function">
    <doc>
    Stop the peering. If you want to restart the peering, use the start
    method.
    </doc>
    //
    <action>
    self->enabled = FALSE;
    </action>
</method>

<method name = "bind" template = "async function">
    <doc>
    Sets up a queue binding using the specified queue.bind method. This method
    sends the queue.bind method to the remote server and exchange so that all
    messages which match will be delivered to the peering's private queue.
    </doc>
    <argument name = "routing key" type = "char *">Bind to routing key</argument>
    <argument name = "arguments" type = "icl_longstr_t *">Bind arguments</argument>
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
    int
        ticket = 0;
    char
        *queue = NULL;
    Bool
        nowait = TRUE;
    amq_peer_method_t
        *bind_method;

    //  Treat empty arguments as null
    if (arguments && arguments->cur_size == 0)
        arguments = NULL;
    //  If this is a new binding (routing_key and arguments are unique),
    //  create a Queue.Bind method and propagate it to the remote server
    //  if connected.
    if (s_binding_exists (self->bindings, routing_key, arguments) == NULL) {
        //  Queue is NULL as the only queue used by this connection is the
        //  peering's private queue
        bind_method = amq_peer_method_new_queue_bind (
            ticket, queue, self->exchange, routing_key, nowait, arguments);
        if (self->connected)
            amq_peer_agent_push (self->peer_agent_thread, self->channel_nbr, bind_method);
        //  We hold onto all outgoing bindings so we can replay them
        ipr_looseref_queue (self->bindings, bind_method);
    }
    </action>
</method>

<method name = "unbind" template = "async function">
    <doc>
    Removes a queue binding using the specified queue.unbind method. This method
    sends the queue.unbind method to the remote server.  It also removes the
    matching queue.bind method from the list of bindings to replay.
    </doc>
    <argument name = "routing key" type = "char *">Bind to routing key</argument>
    <argument name = "arguments" type = "icl_longstr_t *">Bind arguments</argument>
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
    int
        ticket = 0;
    char
        *queue = NULL;
    Bool
        nowait = TRUE;
    amq_peer_method_t
        *unbind_method,
        *bind_method;
    ipr_looseref_t
        *looseref;

    //  Treat empty arguments as null
    if (arguments && arguments->cur_size == 0)
        arguments = NULL;
    //  If connected, propagate Queue.Unbind to remote server
    if (self->connected) {
        unbind_method = amq_peer_method_new_queue_unbind (
            ticket, queue, self->exchange, routing_key, nowait, arguments);
        amq_peer_agent_push (self->peer_agent_thread, self->channel_nbr, unbind_method);
        amq_peer_method_unlink (&unbind_method);
    }
    //  Search list of bindings for the matching Queue.Bind and remove it
    looseref = s_binding_exists (self->bindings, routing_key, arguments);
    assert (looseref);
    bind_method = (amq_peer_method_t *) (looseref->object);
    ipr_looseref_list_remove (looseref);
    amq_peer_method_unlink (&bind_method);
    </action>
</method>

<method name = "forward" template = "async function">
    <doc>
    messages a basic content to the remote peer.
    </doc>
    <argument name = "routing key" type = "char *">Routing key for publish</argument>
    <argument name = "content" type = "amq_content_basic_t *">Content to publish</argument>
    <argument name = "mandatory" type = "Bool">Mandatory routing</argument>
    <argument name = "immediate" type = "Bool">Immediate delivery</argument>
    <possess>
    routing_key = icl_mem_strdup (routing_key);
    content = amq_content_basic_link (content);
    </possess>
    <release>
    icl_mem_free (routing_key);
    amq_content_basic_unlink (&content);
    </release>
    //
    <action>
    int
        ticket = 0;
    amq_peer_method_t
        *method;

    if (amq_server_config_debug_peering (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "P: forward  peer exchange=%s routing_key=%s",
            self->exchange, routing_key);

    //  Create a Basic.Publish method
    method = amq_peer_method_new_basic_publish (
        ticket, self->exchange, routing_key, mandatory, immediate);
    method->content = amq_content_basic_link (content);

    //  We only hold messages if the connection is currently down
    if (self->connected) {
        amq_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
        amq_peer_method_unlink (&method);
    }
    else
        ipr_looseref_queue (self->messages, method);
    </action>
</method>

<!-- ********  The following methods are for internal use only  ******** -->

<method name = "protocol handler" template = "async function" abstract = "1">
    <argument name = "method" type = "amq_peer_method_t *" />
    <possess>
    method = amq_peer_method_link (method);
    </possess>
    <release>
    amq_peer_method_unlink (&method);
    </release>
</method>

<method name = "peer connection start" template = "protocol handler">
    <doc>
    Handles a Connection.Start method coming from the peered server.
    </doc>
    <action>
    asl_field_list_t
        *fields;                        //  Decoded responses
    icl_shortstr_t
        peer_identifier;                //  Unique id of peer

    fields = asl_field_list_new (method->payload.connection_start.server_properties);
    assert (fields);
    asl_field_list_cpy (fields, peer_identifier, "identifier");
    asl_field_list_destroy (&fields);

    if (amq_server_config_debug_peering (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "P: connected peer=%s self=%s",
            peer_identifier, amq_broker->identifier);

    //  If peer and us have same identifier, jump overboard
    if (streq (amq_broker->identifier, peer_identifier)) {
        icl_system_panic ("amq_peering", "E: attempted to peer to self - aborting");
        smt_shut_down ();
    }
    </action>
</method>

<method name = "peer connection open ok" template = "protocol handler">
    <doc>
    Handles a Connection.Open-Ok method coming from the peered server.
    </doc>
    <action>
    self->channel_nbr = 1;              //  Single channel per connection
    amq_peer_agent_channel_open (self->peer_agent_thread, self->channel_nbr);
    </action>
</method>

<method name = "peer connection close" template = "protocol handler">
    <doc>
    Handles a Connection.Close method coming from the peered server.
    </doc>
    <action>
    //  If the connection failed due to a hard error, complain loudly and
    //  shut down the broker.  We really don't want people using servers
    //  with badly defined peerings.
    if (ASL_HARD_ERROR (method->payload.connection_close.reply_code)) {
        smt_log_print (amq_broker->alert_log,
            "E: hard error on peering - please correct and restart server");
        icl_system_panic ("amq_peering", "E: hard error on peering");
        smt_shut_down ();
    }
    </action>
</method>

<method name = "peer channel open ok" template = "protocol handler">
    <doc>
    Handles a Channel.Open-Ok method coming from the peered server.
    </doc>
    <action>
    s_initialise_peering (self);
    </action>
</method>

<method name = "peer basic deliver" template = "protocol handler">
    <doc>
    Handles a Basic.Deliver method coming from the peered server.
    </doc>
    <action>
    if (self->content_fn)
        (self->content_fn) (self->content_caller, self, method);
    </action>
</method>

<method name = "peer basic return" template = "protocol handler">
    <doc>
    Handles a Basic.Return method coming from the peered server.
    </doc>
    <action>
    if (self->return_fn)
        (self->return_fn) (self->return_caller, self, method);
    </action>
</method>

<method name = "initialise">
    <local>
    ipr_bucket_t
        *bucket;
    </local>
    //
    //  Load configuration data, if any, into the config_table
    bucket = amq_server_resource_get ("amq_server_base.cfg");
    amq_peer_config = amq_peer_config_new ();
    amq_peer_config_load_bucket  (amq_peer_config, bucket);
    amq_peer_config_load_xmlfile (amq_peer_config, "amq_peering.cfg", FALSE);
    ipr_bucket_destroy (&bucket);

    amq_peer_agent_init ();
</method>

<method name = "terminate">
    amq_peer_agent_term ();
    amq_peer_config_destroy (&amq_peer_config);
</method>

<!-- Peering monitor, updates status once per second
     Monitor the peer node status. If the node was offline and has come
     online, connect to it.  If the node has gone offline, close the
     connection.
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
        self->peer_agent_thread = amq_peer_agent_connection_thread_new (
            self,                       //  Callback for incoming methods
            self->host,
            self->virtual_host,
            self->auth_data,
            "Peering connection",       //  Instance name
            self->trace,
            amq_server_config_setup_timeout (amq_server_config),
            amq_server_config_debug_peering (amq_server_config));

    //  Peering monitor runs once per second
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<private name = "async header">
static void s_initialise_peering (amq_peering_t *self);
static void s_terminate_peering  (amq_peering_t *self);
static ipr_looseref_t *s_binding_exists (ipr_looseref_list_t *bindings,
    char *routing_key, icl_longstr_t *arguments);
</private>

<private name = "async footer">

static void
s_initialise_peering (amq_peering_t *self)
{
    int
        ticket = 0;
    char
        *queue = NULL,
        *consumer_tag = NULL;
    Bool
        passive = FALSE,
        durable = FALSE,
        exclusive = TRUE,
        auto_delete = TRUE,
        nowait = TRUE,
        no_local = FALSE,
        no_ack = FALSE;
    icl_longstr_t
        *arguments = NULL;
    ipr_looseref_t
        *looseref;                      //  Binding method
    amq_peer_method_t
        *method;                        //  Method to send to peer server
    //
    if (!self->connected) {
        self->connected = TRUE;
        self->offlined = FALSE;
        smt_log_print (amq_broker->alert_log,
            "I: exchange %s now peered to %s", self->exchange, self->host);

        //  Create private queue on peer and consume off queue
        amq_peer_agent_queue_declare (
            self->peer_agent_thread, self->channel_nbr,
            ticket, queue, passive, durable, exclusive, auto_delete, nowait, arguments);

        amq_peer_agent_basic_consume (
            self->peer_agent_thread, self->channel_nbr,
            ticket, queue, consumer_tag, no_local, no_ack, exclusive, nowait, NULL);

        //  Declare remote exchange on peer
        amq_peer_agent_exchange_declare (
            self->peer_agent_thread, self->channel_nbr,
            ticket, self->exchange, self->exchange_type, FALSE, FALSE,
            FALSE, FALSE, nowait, NULL);

        //  Replicate all bindings to remote peer
        looseref = ipr_looseref_list_first (self->bindings);
        while (looseref) {
            method = (amq_peer_method_t *) (looseref->object);
            amq_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
            looseref = ipr_looseref_list_next (&looseref);
        }
        //  Forward all pending messages to remote peer
        while ((method = (amq_peer_method_t *) ipr_looseref_pop (self->messages))) {
            amq_peer_agent_push (self->peer_agent_thread, self->channel_nbr, method);
            amq_peer_method_unlink (&method);
        }
        if (self->status_fn)
            (self->status_fn) (self->status_caller, self, TRUE);
    }
}

static void
s_terminate_peering (amq_peering_t *self)
{
    amq_broker_t
        *broker_ref;

    //  Stop peer agent thread if it's still alive
    if (self->peer_agent_thread) {
        if (!self->peer_agent_thread->zombie)
            amq_peer_agent_connection_close (self->peer_agent_thread);
        smt_thread_unlink (&self->peer_agent_thread);
    }
    if (self->connected) {
        self->connected = FALSE;
        self->offlined  = TRUE;

        if (self->status_fn)
            (self->status_fn) (self->status_caller, self, FALSE);

        //  Use broker with caution because it can be destroyed now
        broker_ref = amq_broker_link (amq_broker);
        if (broker_ref) {
            smt_log_print (broker_ref->alert_log,
                "I: exchange %s unpeered from %s", self->exchange, self->host);
            amq_broker_unlink (&broker_ref);
        }
    }
}

//  Used by peering_bind and peering_unbind to determine if binding is unique.
//  Searches provided looseref list for a queue.bind method matching the
//  provided routing_key and arguments.  Returns looseref if found, NULL if
//  no match found.
static ipr_looseref_t *
s_binding_exists (
    ipr_looseref_list_t *bindings,
    char *routing_key,
    icl_longstr_t *arguments)
{
    ipr_looseref_t
        *looseref;
    amq_peer_method_t
        *method;

    looseref = ipr_looseref_list_first (bindings);
    while (looseref) {
        method = (amq_peer_method_t *) (looseref->object);
        assert (method->class_id  == AMQ_PEER_QUEUE);
        assert (method->method_id == AMQ_PEER_QUEUE_BIND);
        //  If routing_key and arguments are equal, we have a match
        if (streq (method->payload.queue_bind.routing_key, routing_key) &&
            icl_longstr_eq (method->payload.queue_bind.arguments, arguments))
            break;
        looseref = ipr_looseref_list_next (&looseref);
    }
    return looseref;
}
</private>

<private name = "header">
//  These functions are examples of callbacks used in the peering API

static int  s_test_status_handler  (
    void *caller, amq_peering_t *peering, Bool status);
static int  s_test_content_handler (
    void *caller, amq_peering_t *peering, amq_peer_method_t *method);
</private>

<private name = "footer">
static int
s_test_status_handler (
    void *caller,
    amq_peering_t *peering,
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
    amq_peering_t *peering,
    amq_peer_method_t *method)
{
    icl_console_print ("I: (TEST) received message from peering");
    return (0);
}
</private>


<method name = "selftest">
    char
        *test_flag;
    amq_peering_t
        *peering;                       //  Peering to remote server
    amq_content_basic_t
        *content;                       //  Content for testing the peering
    int
        seconds = 1000000;              //  Microseconds per second for sleep
    amq_broker_t
        *broker_ref;

    //  This selftest is NOT run during normal builds
    //  To enable it, set the environment variable AMQ_PEERING_TEST=1
    //
    test_flag = getenv ("AMQ_PEERING_TEST");
    if (test_flag && *test_flag == '1') {
        icl_console_mode (ICL_CONSOLE_DATE, TRUE);
        amq_server_config = amq_server_config_new ();

        //  Move to some unused port so we can talk to another server
        //  on the real AMQP port (5672)
        amq_server_config_set_port (amq_server_config, "15000");

        //  These objects are needed for our test framework
        amq_console = amq_console_new ();
        amq_broker = amq_broker_new ();

        //  **************   Peering example starts here   *******************
        //  Create a new peering to local AMQP server
        peering = amq_peering_new ("localhost", "/", 1, "amq.direct", "direct");

        //  Set login credentials
        amq_peering_set_login (peering, "peering");

        //  Register handlers, with no object reference, for this test
        amq_peering_set_status_handler  (peering, s_test_status_handler, NULL);
        amq_peering_set_content_handler (peering, s_test_content_handler, NULL);

        //  Start the peering - connects to the AMQP server
        amq_peering_start (peering);

        //  Make some peering bindings, with pauses so we can see how the
        //  peering handles if we stop/restart the other server
        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Making binding rk-aaaaaa");
        amq_peering_bind (peering, "rk-aaaaaa", NULL);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Stopping the peering...");
        amq_peering_stop (peering);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Making binding rk-bbbbbb");
        amq_peering_bind (peering, "rk-bbbbbb", NULL);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Restarting the peering...");
        amq_peering_start (peering);

        apr_sleep (1 * seconds);
        icl_console_print ("I: (TEST) Making binding rk-cccccc");
        amq_peering_bind (peering, "rk-cccccc", NULL);

        //  Publish a message to each binding, check that it arrives
        content = amq_content_basic_new ();
        amq_peering_forward (peering, "rk-aaaaaa", content, FALSE, FALSE);
        amq_peering_forward (peering, "rk-bbbbbb", content, FALSE, FALSE);
        amq_peering_forward (peering, "rk-cccccc", content, FALSE, FALSE);
        amq_content_basic_unlink (&content);

        icl_console_print ("I: (TEST) Finished - press Ctrl-C when done");
        smt_wait (0);                   //  Execute SMT engine

        amq_peering_destroy (&peering);
        broker_ref = amq_broker_link (amq_broker);
        amq_broker_destroy  (&broker_ref);
        amq_console_destroy (&amq_console);
        smt_wait (0);

        amq_broker_unlink (&amq_broker);
        smt_wait (0);

        amq_server_config_destroy (&amq_server_config);
    }
</method>

</class>
