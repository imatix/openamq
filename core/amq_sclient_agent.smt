<?xml?>
<agent
    name    = "amq_sclient_agent"
    script  = "smt2c.gsl"
    animate = "1">

<!--include filename = "amq_common.smt" /-->

<public name = "types">
#include "amq_classes.h"
#include "amq_sclient.h"

#define AMQ_OK                 0
#define AMQ_TIMEOUT            1
#define AMQ_CONNECTION_CLOSED  2
#define AMQ_CONNECTION_ERROR   3
#define AMQ_CHANNEL_ERROR      4
#define AMQ_SOCKET_ERROR       5
#define AMQ_INVALID_COMMAND    6
</public>

<private name = "types">
#define AMQP_HEARTBEAT         30
#define AMQP_TRACE_NONE         0
#define AMQP_TRACE_LOW          1
#define AMQP_TRACE_MED          2
#define AMQP_TRACE_HIGH         3
#define AMQP_SOCKET_TIMEOUT     30 * 1000       /*  msecs  */

static int
    s_tracing = 0;
</private>

<handler name = "agent init">
    <argument name = "tracing" type = "int" />
    s_tracing = tracing;
    if (s_tracing > AMQP_TRACE_LOW)
        smt_socket_request_trace (TRUE);
</handler>

<!--  Messages  ----------------------------------------------------------->

<method name = "connection open">
    <field name = "hostname"     type = "char *">Server to connect to</field>
    <field name = "virtual path" type = "char *">Virtual host path</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "connection close">
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "channel open">
    <field name = "channel id"   type = "dbyte" >Channel number</field>
    <field name = "transacted"   type = "Bool"  >Use transacted mode?</field>
    <field name = "restartable"  type = "Bool"  >Use restartable mode?</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "channel ack">
    <field name = "channel id"   type = "dbyte" >Channel number</field>
    <field name = "message nbr"  type = "qbyte" >Message number</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "channel commit">
    <field name = "channel id"   type = "dbyte" >Channel number</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "channel rollback">
    <field name = "channel id"   type = "dbyte" >Channel number</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "channel close">
    <field name = "channel id"   type = "dbyte" >Channel number</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "handle open">
    <field name = "channel id"   type = "dbyte" >Channel number</field>
    <field name = "handle id"    type = "dbyte" >Handle number</field>
    <field name = "temporary"    type = "Bool"  >Temporary access?</field>
    <field name = "dest name"    type = "char *">Destination name</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "handle consume">
    <field name = "handle id"    type = "dbyte" >Handle number</field>
    <field name = "prefetch"     type = "dbyte" >Max pending messages</field>
    <field name = "no local"     type = "Bool"  >Don\'t deliver to self?</field>
    <field name = "unreliable"   type = "Bool"  >Don\'t want to ack</field>
    <field name = "dest name"    type = "char *">Destination name</field>
    <field name = "identifier"   type = "char *">Subscription identifier</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "handle unget">
    <field name = "handle id"    type = "dbyte" >Handle number</field>
    <field name = "message nbr"  type = "qbyte" >Message number</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "handle query">
    <field name = "handle id"    type = "dbyte" >Handle number</field>
    <field name = "message nbr"  type = "qbyte" >Message base</field>
    <field name = "dest name"    type = "char *">Destination name suffix</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "handle send">
    <field name = "handle_id"    type = "dbyte" >Channel number</field>
    <field name = "message"      type = "amq_message_t *"
                                                >Message to send</field>
    <field name = "dest_name"    type = "char *">Destination name</field>
    <field name = "result"       type = "int *" >Pointer to result of operation</field>
</method>

<method name = "handle flow">
    <field name = "handle id"    type = "dbyte">Handle number</field>
    <field name = "flow pause"   type = "Bool" >Pause the flow of messages?</field>
    <field name = "result"       type = "int *">Pointer to result of operation</field>
</method>

<method name = "handle close">
    <field name = "handle id"    type = "dbyte">Handle number</field>
    <field name = "result"       type = "int *">Pointer to result of operation</field>
</method>

<method name = "blocking receive">
    <field name = "msecs"        type = "long" >Timeout, in msecs</field>
    <field name = "result"       type = "int *">Pointer to result of operation</field>
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

        amq_sclient_t
            *client;                    /*  Serial client for callbacks      */
        amq_sclient_handle_notify_fn
            *handle_notify_callback;
        int
            *result;                    /*  Pointer to result of operation   */
        qbyte
            msecs;                      /*  Default socket timeout           */
    </context>

    <handler name = "thread new">
        <argument name = "client"       type = "amq_sclient_t *">Client to callback</argument>
        <argument name = "client name"  type = "char *">Client identifier</argument>
        <argument name = "login"        type = "char *">User login name</argument>
        <argument name = "password"     type = "char *">User password</argument>
        tcb->client = client;
        tcb->result = NULL;
        tcb->msecs  = AMQP_SOCKET_TIMEOUT;
        ipr_shortstr_cpy (tcb->client_name, client_name);
        ipr_shortstr_cpy (tcb->login,       login);
        ipr_shortstr_cpy (tcb->password,    password);
    </handler>

    <handler name = "thread init">
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
            tcb->result = connection_open_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "connect to server" />
            <action name = "send protocol header" />
            <action name = "read next command" />
        </method>
    </state>

    <action name = "connect to server">
        /*  Save the method fields in the control block                      */
        ipr_shortstr_cpy (tcb->hostname,     connection_open_m->hostname);
        ipr_shortstr_cpy (tcb->virtual_path, connection_open_m->virtual_path);

        /*  We extract the port number from the hostname if specified        */
        ipr_shortstr_cpy (tcb->hostname, connection_open_m->hostname);
        tcb->port = strchr (tcb->hostname, ':');
        if (tcb->port)
            *tcb->port++ = 0;           /*  Split the string                 */
        else
            tcb->port = AMQ_SERVER_PORT;

        tcb->socket = smt_socket_connect (
            thread, tcb->msecs, tcb->hostname, tcb->port,
            SMT_NULL_EVENT);
    </action>

    <action name = "send protocol header">
        coprintf ("I: connected to AMQP server on %s:%s", tcb->hostname, tcb->port);
        tcb->frame_header [0] = AMQP_ID;
        tcb->frame_header [1] = AMQP_VERSION;
        s_sock_write (thread, tcb->frame_header, 2);
    </action>

    <!--  EXPECT CONNECTION CHALLENGE  --------------------------------------->

    <state name = "expect connection challenge">
        <event name = "connection challenge" nextstate = "expect connection tune">
            <action name = "send connection response" />
            <action name = "read next command" />
        </event>
    </state>

    <action name = "send connection response">
        amq_field_list_t
            *fields;
        ipr_longstr_t
            *responses = NULL;
        char
            *mechanism = "NONE";

        if (streq (CONNECTION_CHALLENGE.mechanisms, "PLAIN")) {
            fields = amq_field_list_new ();
            amq_field_new_string  (fields, "LOGIN",    tcb->login);
            amq_field_new_string  (fields, "PASSWORD", tcb->password);
            responses = amq_field_list_flatten (fields);
            amq_field_list_destroy (&fields);
            mechanism = "PLAIN";
        }
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_response_new (mechanism, responses);
        send_the_frame (thread);

        ipr_longstr_destroy (&responses);
    </action>

    <!--  EXPECT CONNECTION TUNE  -------------------------------------------->

    <state name = "expect connection tune">
        <event name = "connection tune" nextstate = "after connection open">
            <action name = "process connection tune" />
            <action name = "send connection tune" />
            <action name = "send connection open" />
            <action name = "read next command" />
        </event>
    </state>

    <action name = "process connection tune">
        /*  Lower limits if server asks for that                             */
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

    <!--  AFTER CONNECTION OPEN  --------------------------------------------->

    <state name = "after connection open">
        <event name = "connection reply" nextstate = "connection active">
        </event>
        <event name = "connection close" nextstate = "initialise connection">
            <action name = "store connection close reply" />
            <action name = "send connection close" />
            <action name = "close connection" />
            if (tcb->result)
                *tcb->result = AMQ_CONNECTION_CLOSED;
        </event>
    </state>

    <!--  CONNECTION ACTIVE  ------------------------------------------------->

    <state name = "connection active">
        <!-- This method does not correspond to a specific AMQP command but
             rather to the dialog of reading commands until a HANDLE NOTIFY
             arrives.
          -->
        <method name = "blocking receive" nextstate = "connection active" >
            tcb->result = blocking_receive_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            tcb->msecs = blocking_receive_m->msecs;
            <action name = "read next command" />
            <call state = "expect handle notify" />
        </method>
        <!--
            We allow a specific set of client API methods. The defaults state
            catches any others as invalid.  We cannot get AMQP commands from
            the server in this state since we read the socket only after
            specific client API methods.
          -->
        <method name = "connection close" nextstate = "initialise connection">
            tcb->result = connection_close_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send connection close" />
            <action name = "read next command" />
            <call state = "expect connection close" />
            <action name = "close connection" />
        </method>
        <method name = "channel open">
            tcb->result = channel_open_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send channel open" />
        </method>
        <method name = "channel ack">
            tcb->result = channel_ack_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send channel ack" />
        </method>
        <method name = "channel commit">
            tcb->result = channel_commit_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send channel commit" />
        </method>
        <method name = "channel rollback">
            tcb->result = channel_rollback_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send channel rollback" />
        </method>
        <method name = "channel close">
            tcb->result = channel_close_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send channel close" />
            <action name = "read next command" />
            <call state = "expect channel close" />
        </method>
        <method name = "handle open">
            tcb->result = handle_open_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send handle open" />
        </method>
        <method name = "handle consume">
            tcb->result = handle_consume_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send handle consume" />
        </method>
        <method name = "handle unget">
            tcb->result = handle_unget_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send handle unget" />
        </method>
        <method name = "handle query">
            tcb->result = handle_query_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send handle query" />
            <action name = "read next command" />
            <call state = "expect handle index" />
        </method>
        <method name = "handle send">
            tcb->result = handle_send_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <call state = "sending message" event = "continue" />
        </method>
        <method name = "handle flow">
            tcb->result = handle_flow_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send handle flow" />
        </method>
        <method name = "handle close">
            tcb->result = handle_close_m->result;
            if (tcb->result)
                *tcb->result = AMQ_OK;
            <action name = "send handle close" />
            <action name = "read next command" />
            <call state = "expect handle close" />
        </method>
    </state>

    <action name = "send connection close">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_close_new (
            AMQP_REPLY_SUCCESS,         /*  Reply code                       */
            "Closing");                 /*  Reply text                       */
        send_the_frame (thread);
    </action>

    <action name = "close connection">
        smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
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

    <action name = "send handle consume">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_consume_new (
            handle_consume_m->handle_id,
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

    <action name = "send handle unget">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_unget_new (
            handle_unget_m->handle_id,
            0,                          /*  Confirm tag                      */
            handle_unget_m->message_nbr);
        send_the_frame (thread);
    </action>

    <action name = "send handle query">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_query_new (
            handle_query_m->handle_id,
            0,                          /*  All messages                     */
            handle_query_m->dest_name,
            NULL,                       /*  Selector string                  */
            NULL);                      /*  Selector MIME type               */
        send_the_frame (thread);
    </action>

    <action name = "send handle flow">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_flow_new (
            handle_flow_m->handle_id,
            0,                          /*  Confirmation tag                 */
            handle_flow_m->flow_pause);
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

    <!--  EXPECT HANDLE NOTIFY  ---------------------------------------------->

    <state name = "expect handle notify">
        <event name = "handle notify" nextstate = "after handle notify">
            <action name = "process handle notify" />
            <action name = "process message fragment" />
        </event>

        <!-- These AMQP commands are allowed but discarded  -->
        <event name = "connection ping">
            <action name = "read next command" />
        </event>
        <event name = "connection reply">
            <action name = "read next command" />
        </event>
        <event name = "channel reply">
            <action name = "read next command" />
        </event>
        <event name = "handle created">
            <action name = "read next command" />
        </event>
        <event name = "handle reply">
            <action name = "read next command" />
        </event>
   </state>

    <action name = "process handle notify">
        if (HANDLE_NOTIFY.fragment_size> tcb->frame_max) {
            coprintf ("E: oversized fragment, rejected");
            smt_thread_raise_exception (thread, channel_error_event);
        }
        s_sock_read (thread, tcb->fragment->data, HANDLE_NOTIFY.fragment_size);
    </action>

    <action name = "process message fragment">
        amq_sclient_handle_notify_t
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
            <action name = "read next command" />
        </event>
        <event name = "finished">
            <return/>
        </event>
    </state>

    <!--  EXPECT HANDLE INDEX ------------------------------------------------>

    <state name = "expect handle index">
        <event name = "handle index">
            <action name = "process handle index" />
            <return/>
        </event>

        <!-- These AMQP commands are allowed but discarded  -->
        <event name = "connection ping">
            <action name = "read next command" />
        </event>
        <event name = "connection reply">
            <action name = "read next command" />
        </event>
        <event name = "channel reply">
            <action name = "read next command" />
        </event>
        <event name = "handle created">
            <action name = "read next command" />
        </event>
        <event name = "handle reply">
            <action name = "read next command" />
        </event>
   </state>

    <action name = "process handle index">
        ;
    </action>

    <!--  EXPECT CONNECTION CLOSE  ------------------------------------------->

    <state name = "expect connection close">
        <event name = "connection close">
            <action name = "store connection close reply" />
            <return/>
        </event>
        <!-- These AMQP commands are allowed but discarded  -->
        <event name = "connection ping">
            <action name = "read next command" />
        </event>
        <event name = "connection reply">
            <action name = "read next command" />
        </event>
        <event name = "channel reply">
            <action name = "read next command" />
        </event>
        <event name = "channel close">
            <action name = "store channel close reply" />
            <action name = "read next command" />
        </event>
        <event name = "handle created">
            <action name = "read next command" />
        </event>
        <event name = "handle notify">
            <action name = "process handle notify" />
            <action name = "read next command" />
        </event>
        <event name = "handle index">
            <action name = "read next command" />
        </event>
        <event name = "handle prepare">
            <action name = "read next command" />
        </event>
        <event name = "handle ready">
            <action name = "read next command" />
        </event>
        <event name = "handle reply">
            <action name = "read next command" />
        </event>
        <event name = "handle close">
            <action name = "store handle close reply" />
            <action name = "read next command" />
        </event>
    </state>

    <action name = "store connection close reply">
        tcb->client->reply_code = CONNECTION_CLOSE.reply_code;
        ipr_shortstr_cpy (tcb->client->reply_text, CONNECTION_CLOSE.reply_text);
    </action>

    <action name = "store channel close reply">
        tcb->client->reply_code = CHANNEL_CLOSE.reply_code;
        ipr_shortstr_cpy (tcb->client->reply_text, CHANNEL_CLOSE.reply_text);
    </action>

    <action name = "store handle close reply">
        tcb->client->reply_code = HANDLE_CLOSE.reply_code;
        ipr_shortstr_cpy (tcb->client->reply_text, HANDLE_CLOSE.reply_text);
    </action>

    <!--  EXPECT CHANNEL CLOSE  ---------------------------------------------->

    <state name = "expect channel close">
        <event name = "connection close">
            <action name = "store connection close reply" />
            <return/>
        </event>
        <event name = "channel close">
            <action name = "store channel close reply" />
            <return/>
        </event>
        <!-- These AMQP commands are allowed but discarded  -->
        <event name = "connection ping">
            <action name = "read next command" />
        </event>
        <event name = "connection reply">
            <action name = "read next command" />
        </event>
        <event name = "channel reply">
            <action name = "read next command" />
        </event>
        <event name = "handle created">
            <action name = "read next command" />
        </event>
        <event name = "handle notify">
            <action name = "process handle notify" />
            <action name = "read next command" />
        </event>
        <event name = "handle index">
            <action name = "read next command" />
        </event>
        <event name = "handle prepare">
            <action name = "read next command" />
        </event>
        <event name = "handle ready">
            <action name = "read next command" />
        </event>
        <event name = "handle reply">
            <action name = "read next command" />
        </event>
        <event name = "handle close">
            <action name = "store handle close reply" />
            <action name = "read next command" />
        </event>
    </state>

    <!--  SENDING MESSAGE  --------------------------------------------------->

    <state name = "sending message">
        <event name = "continue">
            <action name = "send message fragment" />
        </event>
        <event name = "finished">
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

    <!--  EXPECT HANDLE CLOSE  ----------------------------------------------->

    <state name = "expect handle close">
        <event name = "connection close">
            <action name = "store connection close reply" />
            <return/>
        </event>
        <event name = "channel close">
            <action name = "store channel close reply" />
            <return/>
        </event>
        <event name = "handle close">
            <action name = "store handle close reply" />
            <return/>
        </event>
        <!-- These AMQP commands are allowed but discarded  -->
        <event name = "connection ping">
            <action name = "read next command" />
        </event>
        <event name = "connection reply">
            <action name = "read next command" />
        </event>
        <event name = "channel reply">
            <action name = "read next command" />
        </event>
        <event name = "handle created">
            <action name = "read next command" />
        </event>
        <event name = "handle notify">
            <action name = "process handle notify" />
            <action name = "read next command" />
        </event>
        <event name = "handle index">
            <action name = "read next command" />
        </event>
        <event name = "handle prepare">
            <action name = "read next command" />
        </event>
        <event name = "handle ready">
            <action name = "read next command" />
        </event>
        <event name = "handle reply">
            <action name = "read next command" />
        </event>
    </state>

    <!--  DEFAULTS  ========================================================-->

    <state name = "defaults">
        <!--  API methods  -->
        <method name = "connection open" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "channel open" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "channel ack" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "channel commit" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "channel rollback" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "channel close" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "handle open" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "handle close" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "handle consume" nextstate = "">
            <action name = "invalid client command" />
        </method>
        <method name = "handle unget">
            <action name = "invalid client command" />
        </method>
        <method name = "handle send" nextstate = "">
            <action name = "invalid client command" />
        </method>

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
        <event name = "connection close" nextstate = "">
            <action name = "store channel close reply" />
            <action name = "close connection" />
            if (tcb->result)
                *tcb->result = AMQ_CONNECTION_CLOSED;
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
            <action name = "process handle notify" />
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
        <event name = "connection error" nextstate = "">
            <action name = "send connection close" />
            <action name = "close connection" />
            if (tcb->result)
                *tcb->result = AMQ_CONNECTION_ERROR;
        </event>
        <event name = "channel error" nextstate = "">
            <action name = "send connection close" />
            <action name = "close connection" />
            if (tcb->result)
                *tcb->result = AMQ_CHANNEL_ERROR;
        </event>

        <!--  Events from SMT socket monitor  -->
        <event name = "socket input">
            <action name = "read next command" />
        </event>
        <event name = "socket error" nextstate = "" >
            <action name = "handle error"/>
            coprintf ("socket error event");
            if (tcb->result)
                *tcb->result = AMQ_SOCKET_ERROR;
        </event>
        <event name = "socket timeout" nextstate = "" >
            <action name = "handle error"/>
            coprintf ("socket timeout event");
            coprintf ("Connection to server timed out - shutting-down");
            if (tcb->result)
                *tcb->result = AMQ_TIMEOUT;
        </event>
    </state>

    <action name = "invalid client command">
        coprintf ("E: invalid method from client layer - shutting down");
        if (tcb->result)
            *tcb->result = AMQ_INVALID_COMMAND;
    </action>

    <action name = "invalid connection command">
        tcb->reply_code = AMQP_COMMAND_INVALID;
        smt_thread_raise_exception (thread, connection_error_event);
    </action>

    <action name = "invalid channel command">
        tcb->reply_code = AMQP_COMMAND_INVALID;
        smt_thread_raise_exception (thread, channel_error_event);
    </action>

    <action name = "handle error">
        coprintf ("E: TCP/IP error - %s", smt_thread_error (thread));
    </action>
</thread>

<action name = "decode the frame">
    amq_frame_free (&tcb->frame);
    tcb->command->cur_size = tcb->socket->io_size;
    tcb->frame = amq_frame_decode (tcb->command);
    if (tcb->frame) {
        if (s_tracing> AMQP_TRACE_NONE)
            amq_frame_dump (tcb->frame, "IN  ");

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
int amq_sclient_agent_register (
    smt_thread_handle_t *thread_handle,
    amq_sclient_callback_t callback, 
    amq_sclient_handle_notify_fn *function);
</public>

<private name = "functions">
#define tcb ((amq_sclient_agent_client_context_t *) thread->context)
int
amq_sclient_agent_register (
    smt_thread_handle_t  *thread_handle,
    amq_sclient_callback_t callback,
    amq_sclient_handle_notify_fn *function)
{
    int
        rc = 0;                         /*  Assume registration worked       */
    smt_thread_t
        *thread;

    if (! smt_thread_handle_valid (thread_handle)) {
        coprintf ("amq_sclient_agent: tried to register to dead thread.");
        rc = 1;
    }
    else {
        thread = thread_handle->thread;

        if (callback == AMQ_SCLIENT_HANDLE_NOTIFY)
            tcb->handle_notify_callback  = function;
        else {
            coprintf ("amq_sclient_agent: tried to register invalid callback '%u'", callback);
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
#define tcb ((amq_sclient_agent_client_context_t *) thread->context)

/*  Send the current frame and free frame block                              */

static void
send_the_frame (smt_thread_t *thread)
{
    amq_frame_encode (tcb->command, tcb->frame);
    ASSERT (tcb->command->cur_size == tcb->frame->size);

    if (s_tracing > AMQP_TRACE_NONE)
        amq_frame_dump (tcb->frame, "OUT ");

    if (tcb->frame->size >= 0xFFFF) {
        *(dbyte *) (tcb->frame_header)     = htons (0xFFFF);
        *(qbyte *) (tcb->frame_header + 2) = htonl (tcb->command->cur_size);
        s_sock_write (thread, tcb->frame_header, 6);
    }
    else {
        *(dbyte *) (tcb->frame_header) = htons ((dbyte) tcb->command->cur_size);
        s_sock_write (thread, tcb->frame_header, 2);
    }
    s_sock_write (thread, tcb->command->data, tcb->command->cur_size);
    amq_frame_free (&tcb->frame);
}

static void inline
s_sock_write (smt_thread_t *thread, byte *buffer, size_t size)
{
    smt_socket_request_write (thread, tcb->socket, tcb->msecs,
                              size, buffer, SMT_NULL_EVENT);
}

static void inline
s_sock_read (smt_thread_t *thread, byte *buffer, size_t size)
{
    smt_socket_request_read (thread, tcb->socket, tcb->msecs,
                             size, size, buffer, SMT_NULL_EVENT);
}
#undef tcb
</private>

<!--
    amq_common.smt

    Common SMT functions used by AMQP servers and clients
 -->

<state name = "defaults">
    <event name = "smt error" nextstate = "">
        coprintf ("smt error event");
        <action name = "handle error"/>
    </event>
</state>

<catch error = "SMT_SOCKET_ERROR"     event = "socket error"  />
<catch error = "SMT_TIMEOUT"          event = "socket timeout"/>
<catch error = "SMT_EMPTY"            event = "smt error" />
<catch error = "SMT_ILLEGAL_ARGUMENT" event = "smt error" />
<catch error = "SMT_SELECT_ERROR"     event = "smt error" />

<action name = "read next command">
    <action name = "read frame header normal" />
    <action name = "read frame header escaped" />
    <action name = "read frame body" />
    <action name = "decode the frame" />
    <action name = "reset default timeout" />
</action>

<action name = "read frame header normal">
    s_sock_read (thread, tcb->frame_header, 2);
</action>

<action name = "read frame header escaped">
    tcb->frame_size = ntohs (*(dbyte *) (tcb->frame_header));
    if (tcb->frame_size == 0xffff) {
        s_sock_read (thread, tcb->frame_header, 4);
        tcb->long_frame = TRUE;
    }
    else
        tcb->long_frame = FALSE;
</action>

<action name = "read frame body">
    if (tcb->long_frame)
        tcb->frame_size = ntohl (*(qbyte *) (tcb->frame_header));

    if (tcb->frame_size> tcb->frame_max) {
        coprintf ("E: received frame is too large (want %ld, have %ld)",
            tcb->frame_size, tcb->frame_max);
        smt_thread_raise_exception (thread, connection_error_event);
    }
    else
        s_sock_read (thread, tcb->command->data, tcb->frame_size);
</action>

<action name = "reset default timeout">
    tcb->msecs = AMQP_SOCKET_TIMEOUT;
</action>

</agent>

