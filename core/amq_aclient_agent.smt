<?xml?>
<agent
    name    = "amq_aclient_agent"
    script  = "smt2c.gsl"
    animate = "1" >

<include filename = "amq_common.smt" />

<public name = "types">
#include "amq_classes.h"
#include "amq_aclient.h"
</public>

<private name = "types">
#define AMQP_HEARTBEAT      30
#define AMQP_TRACE_NONE     0
#define AMQP_TRACE_LOW      1
#define AMQP_TRACE_MED      2
#define AMQP_TRACE_HIGH     3
static int
    s_tracing = 0;
</private>

<handler name = "agent initialise">
    <argument name = "tracing" type = "int" />
    s_tracing = tracing;
    if (s_tracing > AMQP_TRACE_LOW)
        smt_socket_request_trace (TRUE);
</handler>

<!--  Messages  ----------------------------------------------------------->

<method name = "connection open" >
    <field name = "hostname"     type = "char *">Server to connect to</field>
    <field name = "virtual path" type = "char *">Virtual host path</field>
</method>

<method name = "connection close" />

<method name = "channel open" >
    <field name = "channel id"   type = "dbyte">Channel number</field>
    <field name = "transacted"   type = "Bool" >Use transacted mode?</field>
    <field name = "restartable"  type = "Bool" >Use restartable mode?</field>
</method>

<method name = "channel ack" >
    <field name = "channel id"   type = "dbyte">Channel number</field>
    <field name = "message nbr"  type = "qbyte">Message number</field>
</method>

<method name = "channel commit" >
    <field name = "channel id"   type = "dbyte">Channel number</field>
</method>

<method name = "channel rollback" >
    <field name = "channel id"   type = "dbyte">Channel number</field>
</method>

<method name = "channel close" >
    <field name = "channel id"   type = "dbyte">Channel number</field>
</method>

<method name = "handle open" >
    <field name = "channel id"   type = "dbyte" >Channel number</field>
    <field name = "handle id"    type = "dbyte" >Handle number</field>
    <field name = "temporary"    type = "Bool"  >Temporary access?</field>
    <field name = "dest name"    type = "char *">Destination name</field>
</method>

<method name = "handle consume" >
    <field name = "handle id"    type = "dbyte" >Handle id</field>
    <field name = "prefetch"     type = "dbyte" >Max pending messages</field>
    <field name = "no local"     type = "Bool"  >Don\'t deliver to self?</field>
    <field name = "unreliable"   type = "Bool"  >Don\'t want to ack</field>
    <field name = "dest name"    type = "char *">Destination name</field>
    <field name = "identifier"   type = "char *">Subscription identifier</field>
</method>

<method name = "handle send" >
    <field name = "handle_id"    type = "dbyte"          >Channel number</field>
    <field name = "message"      type = "amq_message_t *">Message to send</field>
    <field name = "dest_name"    type = "char *"         >Destination name</field>
</method>

<method name = "handle close" >
    <field name = "handle id"    type = "dbyte">Handle number</field>
</method>


<!--  Client thread  ------------------------------------------------------>

<thread name = "client">
    <context>
        smt_socket_t
            *socket;                    /*  Socket for i/o                   */
        amq_bucket_t
            *command;                   /*  Bucket for command i/o           */
        amq_bucket_t
            *fragment;                  /*  Bucket for fragment i/o          */
        amq_frame_t
            *frame;                     /*  Current frame in or out          */
        amq_message_t
            *message;                   /*  Message being received           */
        byte
            frame_header [2 + 4];       /*  Two or six bytes                 */
        Bool
            long_frame;                 /*  Frame > 64k octets?              */
        size_t
            frame_size;                 /*  Upcoming frame size              */
        size_t
            frame_max;                  /*  Negotiated frame limit           */
        dbyte
            channel_max;                /*  Negotiated channel limit         */
        dbyte
            handle_max;                 /*  Negotiated handle limit          */
        dbyte
            heartbeat;                  /*  Connection heartbeat             */
        dbyte
            reply_code;                 /*  Last reply code to client        */
        dbyte
            handle_id;                  /*  Handle for message being sent    */
        ipr_shortstr_t
            client_name;                /*  Client name for connection       */
        ipr_shortstr_t
            login;                      /*  User login name                  */
        ipr_shortstr_t
            password;                   /*  User password                    */
        ipr_shortstr_t
            hostname;                   /*  Host to connect to               */
        ipr_shortstr_t
            virtual_path;               /*  Virtual host path                */
        char
            *port;                      /*  Somewhere in the host name?      */

        amq_aclient_t
            *client;                    /*  Asynch client for callbacks      */
        amq_aclient_connected_fn
            *connected_callback;
        amq_aclient_handle_created_fn
            *handle_created_callback;
        amq_aclient_handle_notify_fn
            *handle_notify_callback;
        amq_aclient_handle_reply_fn
            *handle_reply_callback;
        amq_aclient_handle_close_fn
            *handle_close_callback;
        amq_aclient_channel_close_fn
            *channel_close_callback;
        amq_aclient_monitor_fn
            *monitor_callback;
    </context>

    <handler name = "thread new">
        <argument name = "client"       type = "void *" >Client to callback</argument>
        <argument name = "client name"  type = "char *" >Client identifier</argument>
        <argument name = "login"        type = "char *" >User login name</argument>
        <argument name = "password"     type = "char *" >User password</argument>
        tcb->client = client;
        ipr_shortstr_cpy (tcb->client_name, client_name);
        ipr_shortstr_cpy (tcb->login,       login);
        ipr_shortstr_cpy (tcb->password,    password);
    </handler>

    <handler name = "thread initialise">
        thread->animate  = (s_tracing > AMQP_TRACE_MED);
        tcb->command     = amq_bucket_new ();
        tcb->fragment    = amq_bucket_new ();
        tcb->frame       = NULL;
        tcb->frame_max   = AMQ_BUCKET_SIZE;
        tcb->channel_max = AMQ_CHANNEL_MAX;
        tcb->handle_max  = AMQ_HANDLE_MAX;
    </handler>

    <handler name = "thread destroy">
        smt_socket_destroy  (&tcb->socket);
        amq_frame_free      (&tcb->frame);
        amq_bucket_destroy  (&tcb->command);
        amq_bucket_destroy  (&tcb->fragment);
        amq_message_destroy (&tcb->message);
    </handler>

    <!--  INITIALISE CONNECTION  --------------------------------------------->

    <state name = "initialise connection">
        <method name = "connection open" nextstate = "expect connection challenge">
            <action name = "connect to server" />
            <action name = "send protocol header" />
            <action name = "read next command" />
        </method>
        <event name = "socket error" nextstate = "">
            <action name = "report connection failed" />
        </event>
    </state>

    <action name = "connect to server">
        /*  Save the method fields in the control block                      */
        ipr_shortstr_cpy (tcb->hostname,     connection_open_m->hostname);
        ipr_shortstr_cpy (tcb->virtual_path, connection_open_m->virtual_path);

        tcb->port = strchr (tcb->hostname, ':');
        if (tcb->port)
            *tcb->port++ = 0;           /*  Split the string                 */
        else
            tcb->port = AMQ_SERVER_PORT;
        tcb->socket = smt_socket_connect (
            thread, 60000 /*msecs*/, tcb->hostname, tcb->port, SMT_NULL_EVENT);
    </action>

    <action name = "send protocol header">
        coprintf ("I: connected to AMQP server on %s:%s", tcb->hostname, tcb->port);
        tcb->frame_header [0] = AMQP_ID;
        tcb->frame_header [1] = AMQP_VERSION;
        s_sock_write (thread, tcb->frame_header, 2);
    </action>

    <action name = "report connection failed">
        coprintf ("E: could not connect to %s:%s (%s)",
            tcb->hostname, tcb->port, sockmsg ());
    </action>

    <!--  EXPECT CONNECTION CHALLENGE  --------------------------------------->

    <state name = "expect connection challenge">
        <event name = "connection challenge" nextstate = "expect connection tune">
            <action name = "send connection response" />
            <action name = "read next command" />
        </event>
    </state>

    <action name = "send connection response">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_response_new (
            "plain", NULL);
        send_the_frame (thread);
    </action>

    <!--  EXPECT CONNECTION TUNE  -------------------------------------------->

    <state name = "expect connection tune">
        <event name = "connection tune" nextstate = "connection active">
            <action name = "process connection tune" />
            <action name = "send connection tune" />
            <action name = "send connection open" />
            <action name = "wait for activity" />
            <action name = "set monitor timer" />
        </event>
    </state>

    <action name = "process connection tune">
    /*  Lower limits if server asks for that                                 */
    if (tcb->frame_max   > CONNECTION_TUNE.frame_max)
        tcb->frame_max   = CONNECTION_TUNE.frame_max;
    if (tcb->channel_max > CONNECTION_TUNE.channel_max)
        tcb->channel_max = CONNECTION_TUNE.channel_max;
    if (tcb->handle_max  > CONNECTION_TUNE.handle_max)
        tcb->handle_max  = CONNECTION_TUNE.handle_max;

    tcb->heartbeat = CONNECTION_TUNE.heartbeat;
    </action>

    <action name = "send connection tune">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_tune_new (
            tcb->frame_max,
            tcb->channel_max,
            tcb->handle_max,
            AMQP_HEARTBEAT,
            NULL);
        send_the_frame (thread);
    </action>

    <action name = "send connection open">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_open_new (
            1,                          /*  Confirm tag                      */
            tcb->virtual_path,
            tcb->client_name,
            NULL);                      /*  Connection options               */
        send_the_frame (thread);
    </action>

    <action name = "set monitor timer">
        if (tcb-> monitor_callback)
            smt_timer_request_delay (thread, 1000 /*msecs*/, timer_expired_event);
    </action>

    <!--  CONNECTION ACTIVE  ------------------------------------------------->

    <state name = "connection active">
        <!--
            We allow a specific set of AMQP commands from the server in this
            state. The defaults state catches any invalid AMQP commands.
            Since we can get client API methods at any time, we handle them
            all in the defaults state.
          -->
        <event name = "connection reply">
            <action name = "process connection reply" />
            <action name = "wait for activity" />
        </event>
        <event name = "handle created">
            <action name = "process handle created" />
            <action name = "wait for activity" />
        </event>
        <event name = "handle reply">
            <action name = "process handle reply" />
            <action name = "wait for activity" />
        </event>
        <event name = "handle notify" nextstate = "after handle notify">
            <action name = "process handle notify" />
            <action name = "process message fragment" />
        </event>
        <event name = "handle close">
            <action name = "process handle close" />
            <action name = "wait for activity" />
        </event>
        <event name = "channel close">
            <action name = "process channel close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "process connection reply">
        amq_aclient_connected_t
            args;

        if (tcb->connected_callback) {
            args.client = tcb->client;
            tcb->connected_callback (&args);
        }
    </action>

    <action name = "process channel close">
        amq_aclient_channel_close_t
            args;

        if (tcb->channel_close_callback) {
            args.client     = tcb->client;
            args.channel_id = CHANNEL_REPLY.channel_id;
            tcb->channel_close_callback (&args);
        }
    </action>

    <action name = "process handle created">
        amq_aclient_handle_created_t
            args;

        if (tcb->handle_created_callback) {
            args.client    = tcb->client;
            args.handle_id = HANDLE_CREATED.handle_id;
            ipr_shortstr_cpy (args.dest_name, HANDLE_CREATED.dest_name);
            tcb->handle_created_callback (&args);
        }
    </action>

    <action name = "process handle reply">
        amq_aclient_handle_reply_t
            args;

        if (tcb->handle_reply_callback) {
            args.client      = tcb->client;
            args.handle_id   = HANDLE_REPLY.handle_id;
            args.confirm_tag = HANDLE_REPLY.confirm_tag;
            args.reply_code  = HANDLE_REPLY.reply_code;
            ipr_shortstr_cpy (args.reply_text, HANDLE_REPLY.reply_text);
            tcb->handle_reply_callback (&args);
        }
    </action>

    <action name = "process handle close">
        amq_aclient_handle_close_t
            args;

        if (tcb->handle_close_callback) {
            args.client    = tcb->client;
            args.handle_id = HANDLE_CLOSE.handle_id;
            tcb->handle_close_callback (&args);
        }
    </action>

    <action name = "process handle notify">
        if (HANDLE_NOTIFY.fragment_size > tcb->frame_max) {
            coprintf ("E: oversized fragment, rejected");
            smt_thread_raise_exception (thread, channel_error_event);
        }
        s_sock_read (thread, tcb->fragment->data, HANDLE_NOTIFY.fragment_size);
    </action>

    <action name = "process message fragment">
        amq_aclient_handle_notify_t
            args;

        /*  tcb->message is the current message, if any                      */
        if (tcb->message == NULL)
            tcb->message = amq_message_new ();

        tcb->fragment->cur_size = tcb->socket->io_size;
        amq_message_record (tcb->message, tcb->fragment, HANDLE_NOTIFY.partial);
        /*  Grab new bucket since record method now owns our old one         */
        tcb->fragment = amq_bucket_new ();

        if (HANDLE_NOTIFY.partial)
            the_next_event = continue_event;
        else {
            if (tcb->handle_notify_callback) {
                args.client      = tcb->client;
                args.message     = tcb->message;
                args.handle_id   = HANDLE_NOTIFY.handle_id;
                args.message_nbr = HANDLE_NOTIFY.message_nbr;
                args.delivered   = HANDLE_NOTIFY.delivered;
                args.redelivered = HANDLE_NOTIFY.redelivered;
                ipr_shortstr_cpy (args.dest_name, HANDLE_NOTIFY.dest_name);
                tcb->handle_notify_callback (&args);
            }
            else
                amq_message_destroy (&tcb->message);
            the_next_event = finished_event;
        }
    </action>

    <!--  AFTER HANDLE NOTIFY  ----------------------------------------------->

    <state name = "after handle notify">
        <event name = "continue" nextstate = "expect handle notify">
            <action name = "wait for activity" />
        </event>
        <event name = "finished" nextstate = "connection active">
            <action name = "wait for activity" />
        </event>
    </state>

    <!--  EXPECT HANDLE NOTIFY  ---------------------------------------------->

    <state name = "expect handle notify">
        <event name = "handle notify" nextstate = "after handle notify">
            <action name = "process handle notify" />
            <action name = "process message fragment" />
        </event>
        <event name = "channel close" nextstate = "connection active">
            <action name = "send channel close" />
            <action name = "process channel close" />
            <action name = "wait for activity" />
        </event>
        <event name = "handle close" nextstate = "connection active">
            <action name = "process handle close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <!--  SENDING MESSAGE  --------------------------------------------------->

    <state name = "sending message">
        <event name = "continue" >
            <action name = "send message fragment" />
        </event>
        <event name = "finished" >
            <return/>
        </event>
    </state>

    <action name = "send message fragment">
        Bool
            partial;

        partial = amq_message_replay (
            handle_send_m->message, tcb->fragment, tcb->frame_max);

        /*  Send HANDLE SEND command frame                                   */
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_send_new (
            handle_send_m->handle_id,   /*  Handle to send to                */
            0,                          /*  Confirmation tag                 */
            tcb->fragment->cur_size,    /*  Size of following fragment       */
            partial,                    /*  Partial message?                 */
            FALSE,                      /*  Out of band data?                */
            FALSE,                      /*  Restarting large message?        */
            FALSE,                      /*  Working in streaming mode?       */
            partial? NULL:
              handle_send_m->dest_name);
        send_the_frame (thread);

        /*  Send data buffer (without any frame header)                      */
        s_sock_write (thread, tcb->fragment->data, tcb->fragment->cur_size);

        if (partial)
            the_next_event = continue_event;
        else {
            the_next_event = finished_event;
            amq_message_destroy (&handle_send_m->message);
        }
    </action>

    <!--  DEFAULTS  ========================================================-->

    <state name = "defaults">
        <!--  Commands received on the connection  -->
        <event name = "connection challenge">
            <action name = "invalid connection command" />
        </event>
        <event name = "connection tune">
            <action name = "invalid connection command" />
        </event>
        <event name = "connection ping">
            <action name = "invalid connection command" />
        </event>
        <event name = "connection reply">
            <action name = "invalid connection command" />
        </event>
        <event name = "connection close">
            <action name = "invalid connection command" />
        </event>
        <event name = "channel reply">
            <action name = "invalid connection command" />
        </event>
        <event name = "channel close">
            <action name = "invalid connection command" />
        </event>
        <event name = "handle created">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle notify">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle index">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle prepare">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle ready">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle reply">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle close">
            <action name = "invalid channel command" />
        </event>
        <event name = "connection error" nextstate = "" >
            <action name = "send connection close" />
            <action name = "close connection" />
        </event>
        <event name = "channel error" nextstate = "">
            <action name = "send connection close" />
            <action name = "close connection" />
        </event>

        <!-- Methods coming from client API  -->
        <method name = "connection open" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "connection close" nextstate = "" >
            <action name = "send connection close" />
            <action name = "close connection" />
        </method>
        <method name = "channel open">
            <action name = "send channel open" />
            <action name = "wait for activity" />
        </method>
        <method name = "channel ack">
            <action name = "send channel ack" />
            <action name = "wait for activity" />
        </method>
        <method name = "channel commit">
            <action name = "send channel commit" />
            <action name = "wait for activity" />
        </method>
        <method name = "channel rollback">
            <action name = "send channel rollback" />
            <action name = "wait for activity" />
        </method>
        <method name = "channel close">
            <action name = "send channel close" />
            <action name = "wait for activity" />
        </method>
        <method name = "handle open">
            <action name = "send handle open" />
            <action name = "wait for activity" />
        </method>
        <method name = "handle close">
            <action name = "send handle close" />
            <action name = "wait for activity" />
        </method>
        <method name = "handle consume">
            <action name = "send handle consume" />
            <action name = "wait for activity" />
        </method>
        <method name = "handle send" >
            <call state = "sending message" event = "continue" />
            <action name = "wait for activity" />
        </method>

        <!--  Events from SMT socket monitor  -->
        <event name = "socket input" >
            <action name = "read next command" />
        </event>
        <event name = "socket closed" nextstate = "" >
            <action name = "signal connection dropped" />
        </event>
        <event name = "timer expired" >
            <action name = "call monitor callback" />
            <action name = "wait for activity" />
            <action name = "set monitor timer" />
        </event>
    </state>

    <action name = "close connection">
        smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
    </action>

    <action name = "send connection close">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_close_new (
            AMQP_REPLY_SUCCESS,         /*  Reply code                       */
            "Closing");                 /*  Reply text                       */
        send_the_frame (thread);
    </action>

    <action name = "send channel open">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_channel_open_new (
            channel_open_m->channel_id,
            0,                          /*  Confirm tag                      */
            channel_open_m->transacted,
            channel_open_m->restartable,
            NULL,                       /*  Channel options                  */
            NULL);                      /*  Out of band options              */
        send_the_frame (thread);
    </action>

    <action name = "send channel ack">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_channel_ack_new (
            channel_ack_m->channel_id,
            0,                          /*  Confirm tag                      */
            channel_ack_m->message_nbr);
        send_the_frame (thread);
    </action>

    <action name = "send channel commit">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_channel_commit_new (
            channel_commit_m->channel_id,
            0,                          /*  Confirm tag                      */
            NULL);                      /*  Commit options                   */
        send_the_frame (thread);
    </action>

    <action name = "send channel rollback">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_channel_rollback_new (
            channel_rollback_m->channel_id,
            0,                          /*  Confirm tag                      */
            NULL);                      /*  Rollback options                 */
        send_the_frame (thread);
    </action>

    <action name = "send channel close">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_channel_close_new (
            channel_close_m->channel_id,
            AMQP_REPLY_SUCCESS,         /*  Reply code                       */
            "Closing");                 /*  Reply text                       */
        send_the_frame (thread);
    </action>

    <action name = "send handle open">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_open_new (
            handle_open_m->channel_id,
            handle_open_m->handle_id,
            AMQP_SERVICE_QUEUE,         /*  Service type                     */
            0,                          /*  Confirmation tag                 */
            TRUE,                       /*  Request producer access          */
            TRUE,                       /*  Request consumer access          */
            TRUE,                       /*  Request browser access           */
            handle_open_m->temporary,
            handle_open_m->dest_name,
            NULL,                       /*  Default mime_type                */
            NULL,                       /*  Default content encoding         */
            NULL);                      /*  Destination options              */
        send_the_frame (thread);
    </action>

    <action name = "send handle close">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_close_new (
            handle_close_m->handle_id,
            AMQP_REPLY_SUCCESS,         /*  Reply code                       */
            "Closing");                 /*  Reply text                       */
        send_the_frame (thread);
    </action>

    <action name = "send handle consume">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_consume_new (
            handle_close_m->handle_id,
            0,                          /*  Confirm tag                      */
            handle_consume_m->prefetch,
            handle_consume_m->no_local,
            handle_consume_m->unreliable,
            handle_consume_m->dest_name,
            handle_consume_m->identifier,
            NULL,                       /*  Selector string                  */
            NULL);                      /*  Selector MIME type               */
        send_the_frame (thread);
    </action>

    <action name = "signal connection dropped">
        coprintf ("Connection to server was lost - shutting-down");
    </action>

    <action name = "call monitor callback" >
        amq_aclient_monitor_t
            args;

        if (tcb-> monitor_callback) {
            args.client = tcb->client;
            tcb-> monitor_callback (&args);
        }
    </action>

    <action name = "invalid client command">
        smt_thread_raise_exception (thread, connection_error_event);
    </action>

    <action name = "invalid connection command">
        tcb->reply_code = AMQP_COMMAND_INVALID;
        smt_thread_raise_exception (thread, connection_error_event);
    </action>

    <action name = "invalid channel command">
        tcb->reply_code = AMQP_COMMAND_INVALID;
        smt_thread_raise_exception (thread, channel_error_event);
    </action>
</thread>

<action name = "decode the frame">
    amq_frame_free (&tcb->frame);
    tcb->command->cur_size = tcb->socket->io_size;
    tcb->frame = amq_frame_decode (tcb->command);
    if (tcb->frame) {
        if (s_tracing > AMQP_TRACE_NONE) {
            coprintf ("I: received frame size=%ld", tcb->frame->size);
            amq_frame_dump (tcb->frame);
        }
        switch (tcb->frame->type) {
            case FRAME_TYPE_CONNECTION_CHALLENGE:
                the_next_event = connection_challenge_event;
                break;
            case FRAME_TYPE_CONNECTION_TUNE:
                the_next_event = connection_tune_event;
                break;
            case FRAME_TYPE_CONNECTION_PING:
                the_next_event = connection_ping_event;
                break;
            case FRAME_TYPE_CONNECTION_REPLY:
                the_next_event = connection_reply_event;
                break;
            case FRAME_TYPE_CONNECTION_CLOSE:
                the_next_event = connection_close_event;
                break;
            case FRAME_TYPE_CHANNEL_REPLY:
                the_next_event = channel_reply_event;
                break;
            case FRAME_TYPE_CHANNEL_CLOSE:
                the_next_event = channel_close_event;
                break;
            case FRAME_TYPE_HANDLE_CREATED:
                the_next_event = handle_created_event;
                break;
            case FRAME_TYPE_HANDLE_NOTIFY:
                the_next_event = handle_notify_event;
                break;
            case FRAME_TYPE_HANDLE_INDEX:
                the_next_event = handle_index_event;
                break;
            case FRAME_TYPE_HANDLE_PREPARE:
                the_next_event = handle_prepare_event;
                break;
            case FRAME_TYPE_HANDLE_READY:
                the_next_event = handle_ready_event;
                break;
            case FRAME_TYPE_HANDLE_REPLY:
                the_next_event = handle_reply_event;
                break;
            case FRAME_TYPE_HANDLE_CLOSE:
                the_next_event = handle_close_event;
                break;
            default:
                tcb->reply_code = AMQP_FRAME_ERROR;
                smt_thread_raise_exception (thread, connection_error_event);
        }
    }
    else {
        tcb->reply_code = AMQP_FRAME_ERROR;
        smt_thread_raise_exception (thread, connection_error_event);
    }
</action>

<public name = "types">
int amq_aclient_agent_register (
    smt_thread_handle_t *thread_handle, amq_aclient_callback_t callback, void *function);
</public>

<private name = "functions">
#define tcb ((amq_aclient_agent_client_context_t *) thread->context)
int
amq_aclient_agent_register (
    smt_thread_handle_t  *thread_handle,
    amq_aclient_callback_t callback,
    void                 *function)
{
    int
        rc = 0;                         /*  Assume registration worked       */
    smt_thread_t
        *thread;

    if (! smt_thread_handle_valid (thread_handle)) {
        coprintf ("amq_aclient_agent: tried to register to dead thread.");
        rc = 1;
    }
    else {
        thread = thread_handle->thread;

        if (callback == AMQ_ACLIENT_CONNECTED)
            tcb->connected_callback      = (amq_aclient_connected_fn *)      function;
        else
        if (callback == AMQ_ACLIENT_HANDLE_CREATED)
            tcb->handle_created_callback = (amq_aclient_handle_created_fn *) function;
        else
        if (callback == AMQ_ACLIENT_HANDLE_REPLY)
            tcb->handle_reply_callback   = (amq_aclient_handle_reply_fn *)   function;
        else
        if (callback == AMQ_ACLIENT_HANDLE_NOTIFY)
            tcb->handle_notify_callback  = (amq_aclient_handle_notify_fn *)  function;
        else
        if (callback == AMQ_ACLIENT_HANDLE_CLOSE)
            tcb->handle_close_callback   = (amq_aclient_handle_close_fn *)   function;
        else
        if (callback == AMQ_ACLIENT_CHANNEL_CLOSE)
            tcb->channel_close_callback  = (amq_aclient_channel_close_fn *)  function;
        else
        if (callback == AMQ_ACLIENT_MONITOR) {
            tcb->monitor_callback        = (amq_aclient_monitor_fn *)        function;
            smt_timer_request_delay (thread, 1000 /*msecs*/, timer_expired_event);
        }
        else {
            coprintf ("amq_aclient_agent: tried to register invalid callback '%u'", callback);
            rc = 1;
        }
    }
    return (rc);
}
#undef tcb
</private>

<private name = "types">
static void        send_the_frame (smt_thread_t *thread);
static void inline s_sock_write   (smt_thread_t *thread, byte *buffer, size_t size);
static void inline s_sock_read    (smt_thread_t *thread, byte *buffer, size_t size);
</private>

<private name = "functions">
#define tcb ((amq_aclient_agent_client_context_t *) thread->context)

/*  Send the current frame and free frame block                              */

static void
send_the_frame (smt_thread_t *thread)
{
    if (s_tracing > AMQP_TRACE_NONE) {
        coprintf ("I: sending frame size=%ld", tcb->frame->size);
        amq_frame_dump (tcb->frame);
    }
    if (tcb->frame->size > 0xFFFF) {
        *(dbyte *) (tcb->frame_header)     = htons (0xFFFF);
        *(qbyte *) (tcb->frame_header + 2) = htonl (tcb->frame->size);
        s_sock_write (thread, tcb->frame_header, 6);
    }
    else {
        *(dbyte *) (tcb->frame_header) = htons ((dbyte) tcb->frame->size);
        s_sock_write (thread, tcb->frame_header, 2);
    }
    amq_frame_encode (tcb->command, tcb->frame);
    s_sock_write (thread, tcb->command->data, tcb->command->cur_size);
    amq_frame_free (&tcb->frame);
}

static void inline
s_sock_write (smt_thread_t *thread, byte *buffer, size_t size)
{
    smt_socket_request_write (thread, tcb->socket, 0, size, buffer, SMT_NULL_EVENT);
}

static void inline
s_sock_read (smt_thread_t *thread, byte *buffer, size_t size)
{
    smt_socket_request_read (thread, tcb->socket, 0, size, size, buffer, SMT_NULL_EVENT);
}

#undef  tcb
</private>

</agent>

