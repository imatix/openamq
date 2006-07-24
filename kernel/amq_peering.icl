<?xml?>
<class
    name      = "amq_peering"
    comment   = "AMQ server-to-server peering class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
    - peering is a smart link to remote server
    - automatically connects and logs in
        - queues outgoing requests if connection is dead
    - reconnects if remote link goes down
        - tells you if there is a problem
    - creates private queue and consumes from queue
    - lets you publish messages to peer
    - provides you messages delivered from peer
    - works with unlimited number of remote exchanges


    todo:
    - invoke status callback when peering connected/disconnected
</doc>

<inherit class = "smt_object" />
<inherit class = "icl_init" />

<import class = "amq_server_classes" />

<public name = "header">
//  Make a forward reference to resolve the circular dependency between the
//  function typedef and the method structure.
typedef struct _amq_peer_basic_deliver_t amq_peering_basic_deliver_t;
typedef int (amq_peering_status_fn) (void *self, Bool connected);
typedef int (amq_peering_basic_deliver_fn) (void *self, amq_peering_basic_deliver_t *basic_deliver);
</public>

<context>
    void
        *parent;                        //  Parent object
    Bool
        enabled,                        //  We're active
        connected,                      //  We're connected
        offlined;                       //  Peer has gone offline
    icl_shortstr_t
        host,                           //  Peer host name
        virtual_host;                   //  Virtual host name
    icl_longstr_t
        *auth_data;                     //  Authentication data
    int
        trace;                          //  Trace level
    Bool
        nowait;                         //  Use nowait binding option?
    smt_thread_t
        *peer_agent_thread;             //  Active agent thread if any
    dbyte
        channel_nbr;                    //  Active channel number
    ipr_looseref_list_t
        *bind_list;                     //  List of bindings for peering

    //  Callbacks into parent object
    amq_peering_status_fn
        *status_fn;                     //  Status change callback
    amq_peering_basic_deliver_fn
        *basic_deliver_fn;              //  Basic deliver callback
</context>

<method name = "new">
    <argument name = "parent" type = "void *">Parent object</argument>
    <argument name = "host" type = "char *">Host to connect to</argument>
    <argument name = "virtual host" type = "char *">Virtual host</argument>
    <argument name = "trace" type = "int">Trace level, 0 - 3</argument>
    //
    self->parent = parent;
    icl_shortstr_cpy (self->host, host);
    icl_shortstr_cpy (self->virtual_host, virtual_host);
    self->trace = trace;
    self->bind_list = ipr_looseref_list_new ();
    self->nowait = TRUE;
</method>

<method name = "destroy">
    <action>
    s_disconnect_peer (self);
    icl_longstr_destroy (&self->auth_data);
    ipr_looseref_list_destroy (&self->bind_list);
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
    config = ipr_config_dup (amq_peer_config->config);
    ipr_config_locate (config, "/config/security", "plain");
    if (config->located)
        ipr_config_locate (config, "user", login);

    if (config->located) {
        icl_longstr_destroy (&self->auth_data);
        self->auth_data = ipr_sasl_plain_encode (login, ipr_config_get (config, "password", ""));
    }
    else
        icl_console_print ("W: peering credentials for '%s' not defined - cannot connect", login);

    ipr_config_destroy (&config);
</method>

<method name = "start" template = "async function" async = "1">
    <doc>
    Start the peering. You must call this method to start the peering.
    </doc>
    //
    <action>
    self->enabled = TRUE;
    s_connect_to_peer (self);
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</method>

<method name = "stop" template = "async function" async = "1">
    <doc>
    Stop the peering. Disconnects the peering. If you want to restart the
    peering, use the start method.
    </doc>
    //
    <action>
    self->enabled = FALSE;
    s_disconnect_peer (self);
    </action>
</method>

<!-- Peering monitor, updates status once per second
     Monitor the peer node status. If the node was offline and has come
     online, connect to it.  If the node has gone offline, close the
     connection.
 -->
<event name = "monitor">
    <action>
    //
    if (self->peer_agent_thread && self->peer_agent_thread->zombie)
        s_disconnect_peer (self);
    else
    if (self->enabled)
        s_connect_to_peer (self);

    //  Peering monitor runs once per second
    smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
    </action>
</event>

<method name = "register status" template = "async function" async = "1">
    <doc>
    Register a status callback handler. The callback is invoked when
    the peering connection changes status (connected or disconnected).
    Note that the callback method is always sent to the parent object
    specified when the new peering is created.  If the callback function
    is NULL, no status callbacks will be made.
    </doc>
    <argument name = "status_fn" type = "amq_peering_status_fn *">Callback</argument>
    //
    <action>
    self->status_fn = status_fn;
    </action>
</method>

<method name = "register basic deliver" template = "async function" async = "1">
    <doc>
    Register a basic_deliver callback handler.  The callback in invoked
    when a basic message arrives from the remote peered server. Note that
    the callback method is always sent to the parent object specified when
    the new peering is created.  If the callback function is NULL, no basic
    delivery callbacks will be made.
    </doc>
    <argument name = "basic_deliver_fn" type = "amq_peering_basic_deliver_fn *">Callback</argument>
    //
    <action>
    self->basic_deliver_fn = basic_deliver_fn;
    </action>
</method>

<method name = "bind" template = "async function" async = "1">
    <doc>
    Sets a queue binding using the specified queue.bind method. This method
    sends the queue.bind method to the remote server and exchange so that all
    messages which match will be delivered to the peering's private queue.
    </doc>
    <argument name = "exchange" type = "char *">Remote exchange name</argument>
    <argument name = "routing key" type = "char *">Bind to routing key</argument>
    <argument name = "arguments" type = "icl_longstr_t *">Bind arguments</argument>
    //
    <possess>
    exchange = icl_mem_strdup (exchange);
    routing_key = icl_mem_strdup (routing_key);
    arguments = icl_longstr_dup (arguments);
    </possess>
    <release>
    icl_mem_free (exchange);
    icl_mem_free (routing_key);
    icl_longstr_destroy (&arguments);
    </release>
    //
    <action>
    amq_server_method_t
        *method;

    method = amq_peer_method_new_queue_bind (
        0, "", exchange, routing_key, arguments, self->nowait);
    </action>
</method>

<!--
    when connected
        - send queue.declare
        - when queue.declare-ok is returned,
            - send basic.consume
        -

<method name = "send xxx ?">
    - create private queue and consume from queue
        - done automatically on all peerings
    - make remote binding (onto private queue)
    - deliver incoming messages
        - acknowledge messages, later
</method>
-->

<method name = "peer connection start" template = "async function" async = "1">
    <doc>
    Handles a Connection.Start method coming from the peered server.
    </doc>
    <argument name = "method" type = "amq_peer_connection_start_t *" />
    //
    <action>
    </action>
</method>

<method name = "peer basic deliver" template = "async function" async = "1">
    <doc>
    Handles a Basic.Deliver method coming from the peered server.
    </doc>
    <argument name = "method" type = "amq_peer_basic_deliver_t *" />
    <argument name = "content" type = "amq_content_basic_t *" />
    //
    <action>
    </action>
</method>

<method name = "initialise">
    //  Load peering configuration data from amq_peering.cfg if present
    amq_peer_config = amq_peer_config_new ("amq_server_base.cfg", "amq_peering.cfg", FALSE);
    amq_peer_agent_init ();
</method>

<method name = "terminate">
    amq_peer_agent_term ();
    amq_peer_config_destroy (&amq_peer_config);
</method>

<private name = "async header">
static void s_connect_to_peer  (amq_peering_t *self);
static void s_disconnect_peer  (amq_peering_t *self);
</private>

<private name = "async footer">
static void
s_connect_to_peer (amq_peering_t *self)
{
    assert (!self->peer_agent_thread);

    icl_console_print ("Connecting to host=%s...", self->host);
    self->peer_agent_thread = amq_peer_agent_connection_thread_new (
        self,                           //  Callback for incoming methods
        self->host,
        self->virtual_host,
        self->auth_data,
        "Peering connection",           //  Instance name
        self->trace);

    self->channel_nbr = 1;              //  Single channel per connection
    amq_peer_agent_channel_open (self->peer_agent_thread, self->channel_nbr);
}

static void
s_disconnect_peer (amq_peering_t *self)
{
    if (self->peer_agent_thread) {
        if (!self->peer_agent_thread->zombie)
            amq_peer_agent_connection_close (self->peer_agent_thread);
        smt_thread_unlink (&self->peer_agent_thread);
    }
    if (self->connected) {
        self->connected = FALSE;
        self->offlined  = TRUE;
    }
}
</private>

<method name = "selftest">
    char
        *test_flag;
    amq_peering_t
        *peering;

    test_flag = getenv ("AMQ_PEERING_TEST");
    if (test_flag && *test_flag == '1') {
        //  Example of peering API usage

        peering = amq_peering_new (NULL, "localhost", "/", 2);
        amq_peering_set_login (peering, "peering");
        amq_peering_start (peering);

        smt_wait (0);                   //  Execute SMT engine

        amq_peering_destroy (&peering);
    }
</method>

</class>
