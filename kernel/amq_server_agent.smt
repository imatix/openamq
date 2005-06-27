<?xml?>
<agent
    name    = "amq_server_agent"
    script  = "smt_gen.gsl"
    animate = "1">

<public name = "types">
#include "amq_classes.h"
#include "amq_smessage.h"
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

<catch error = "SMT_SOCKET_ERROR" event = "socket error" />
<catch error = "SMT_TIMEOUT"      event = "timeout"      />
<catch                            event = "smt error"    />

<handler name = "agent init">
    <argument name = "tracing" type = "int" />
    smt_thread_t
        *thread;
    
    s_tracing = tracing;
    if (s_tracing > AMQP_TRACE_LOW)
        smt_socket_request_trace (TRUE);
    thread = amq_server_agent_master_thread_new ();
    if (thread)
        smt_thread_activate (thread);
</handler>

<handler name = "agent term">
</handler>

<!--  Master thread  --------------------------------------------------------->

<thread name = "master">
    <context>
        smt_socket_t *socket;           /*  Socket for connect requests      */
        smt_socket_t *connect;          /*  Newly-accepted socket            */
    </context>

    <handler name = "thread init">
        thread->animate = (s_tracing > AMQP_TRACE_MED);
        smt_thread_set_next_event (thread, ok_event);
    </handler>
    <handler name = "thread destroy">
        smt_socket_destroy (&tcb->socket);
    </handler>

    <!--  INITIALISE MASTER  ------------------------------------------------->

    <state name = "initialise master">
        <event name = "ok" nextstate = "new connection">
            <action name = "initialise server" />
            <action name = "accept connection" />
        </event>
        <event name = "timeout" nextstate = "new connection">
            <action name = "server monitor" />
            <action name = "accept connection" />
        </event>
        <event name = "error" nextstate = "">
        </event>
    </state>

    <action name = "initialise server">
        char
            *port;                          /*  Port to listen on                */
        ipr_shortstr_t
            buffer;                         /*  Holds host name & error message  */
        apr_pool_t
            *pool;                          /*  APR pool for host addresses      */
        apr_sockaddr_t
            *sockaddr;                      /*  The associated socket addresses  */
        char
            *addr;                          /*  To hold socket address string    */

        port = ipr_config_locattr (amq_config, "/config/server", "port", AMQ_SERVER_PORT);
        apr_pool_create (&pool, NULL);
        apr_gethostname (buffer, IPR_SHORTSTR_MAX, pool);
        apr_sockaddr_info_get (&sockaddr, buffer, APR_UNSPEC, 0, 0, pool);
        while (sockaddr) {
            apr_sockaddr_ip_get (&addr, sockaddr);
            icl_console_print ("I: AMQP server binding to %s:%s", addr, port);
            sockaddr = sockaddr-> next;
        }
        apr_pool_destroy (pool);

        tcb-> socket = smt_socket_passive (thread, port, 5);
        if (!tcb->socket) {
            icl_console_print ("E: could not open AMQP port %s - %s",
                      port, smt_thread_error (thread));
            smt_thread_raise_exception (thread, error_event);
        }
    </action>

    <action name = "accept connection">
        smt_socket_request_accept (
            thread,
            tcb->socket,
            amq_global_monitor () * 1000,
            ok_event,
            &tcb->connect);
    </action>

    <action name = "server monitor">
        static int
            remaining = 1;
        uint
            table_idx;
        amq_vhost_t
            *vhost;
        amq_dest_t
            *dest;

        /*  Output queue statistics                                          */
        if (amq_global_monitor () && --remaining == 0) {
            for (table_idx = 0; table_idx < AMQ_VHOST_TABLE_MAXSIZE; table_idx++) {
                vhost = amq_vhosts->table_items [table_idx];
                if (vhost && vhost != AMQ_VHOST_DELETED) {
                    dest = amq_dest_list_first (vhost->dest_list);
                    while (dest) {
                        if (dest->queue->monitor_pending) {
                            icl_console_print ("Queue:%-30s  in:%6d  out:%6d  pending:%6d",
                                dest->key,
                                dest->queue->accept_count,
                                dest->queue->dispatch_count,
                                dest->queue->memory_queue_size);
                            dest->queue->monitor_pending = FALSE;
                        }
                        dest = amq_dest_list_next (dest);
                    }
                }
            }
            remaining = amq_global_monitor () ;
        }
    </action>

    <!--  NEW CONNECTION  ---------------------------------------------------->

    <state name = "new connection">
        <event name = "ok" nextstate = "new connection">
            <action name = "create client thread" />
            <action name = "accept connection" />
        </event>
        <event name = "timeout" nextstate = "new connection">
            <action name = "server monitor" />
            <action name = "accept connection" />
        </event>
    </state>

    <action name = "create client thread">
        smt_thread_t
            *client_thread;
        amq_server_agent_client_context_t
            *client_tcb;

        client_thread = amq_server_agent_client_thread_new ();
        client_tcb         = client_thread->context;
        memset (client_tcb, 0, sizeof (*client_tcb));  /*  JS  */
        client_tcb->socket = tcb-> connect;
        
        if (client_thread)
            smt_thread_activate (client_thread);
    </action>
</thread>


<!--  Client thread  --------------------------------------------------------->

<method name = "handle created">
    <argument name = "handle id" type = "dbyte">Handle number</argument>
    <argument name = "dest name" type = "char *">Destination name</argument>
</method>

<method name = "handle notify">
    <argument name = "handle id"   type = "dbyte">Handle number</argument>
    <argument name = "message nbr" type = "qbyte">Message reference</argument>
    <argument name = "recovery"    type = "Bool"  >Restarting large message?</argument>
    <argument name = "delivered"   type = "Bool"  >Message is being delivered?</argument>
    <argument name = "redelivered" type = "Bool"  >Message is being redelivered?</argument>
    <argument name = "dest name"   type = "char *">Originating destination</argument>
    <argument name = "message"     type = "amq_smessage_t *">Message to send</argument>
</method>

<method name = "handle index">
    <argument name = "handle id"    type = "dbyte">Handle number</argument>
    <argument name = "message nbr"  type = "qbyte">Top message number</argument>
    <argument name = "message list" type = "ipr_longstr_t *">Message numbers</argument>
</method>

<method name = "connection reply">
    <argument name = "confirm tag"  type = "dbyte" >Confirmation tag</argument>
</method>

<method name = "channel reply">
    <argument name = "confirm tag"  type = "dbyte" >Confirmation tag</argument>
</method>

<method name = "handle reply">
    <argument name = "confirm tag"  type = "dbyte" >Confirmation tag</argument>
</method>

<thread name = "client">
    <context>
        smt_socket_t
            *socket;                    /*  Socket for i/o                   */
        amq_vhost_t
            *vhost;                     /*  Virtual host for connection      */
        amq_connection_t
            *connection;                /*  Connection object                */
        amq_bucket_t
            *command;                   /*  Bucket for command i/o           */
        amq_bucket_t
            *fragment;                  /*  Bucket for fragment i/o          */
        amq_channel_t
            *channel;                   /*  Current channel being used       */
        amq_handle_t
            *handle;                    /*  Current handle being used        */
        amq_frame_t
            *frame;                     /*  Current frame in or out          */
        size_t
            frame_size;                 /*  Upcoming frame size              */
        size_t
            frame_max;                  /*  Negotiated frame limit           */
        byte
            frame_header [2 + 4];       /*  Two or six bytes                 */
        Bool
            long_frame;                 /*  Frame > 64k octets?              */
        dbyte
            channel_id;                 /*  Last-received channel id         */
        dbyte
            handle_id;                  /*  Last-received handle id          */
        dbyte
            reply_code;                 /*  Last reply code to client        */
        char
            *reply_text;                /*  Last reply text to client        */
        long
            client_id;                  /*  Client record for connection     */
    </context>

    <handler name = "thread init">
        thread->animate  = (s_tracing > AMQP_TRACE_MED);
        the_next_event   = ok_event;
        tcb->frame_max   = AMQ_BUCKET_MAX_SIZE;
        tcb->connection  = amq_connection_new (thread);
        tcb->command     = amq_bucket_new (tcb->frame_max);
    </handler>
    <handler name = "thread destroy">
        amq_handle_unlink      (&tcb->handle);
        smt_socket_destroy     (&tcb->socket);
        amq_bucket_destroy     (&tcb->command);
        amq_bucket_destroy     (&tcb->fragment);
        amq_connection_destroy (&tcb->connection);
        amq_frame_free         (&tcb->frame);
    </handler>

    <!--  INITIALISE CONNECTION  --------------------------------------------->

    <state name = "initialise connection">
        <event name = "ok" nextstate = "expect connection response">
            <action name = "read protocol header" />
            <action name = "check protocol header" />
            <action name = "send connection challenge" />
            <action name = "read next command" />
        </event>
        <event name = "connection error" nextstate = "">
            <action name = "close the connection" />
        </event>
    </state>

    <action name = "read protocol header">
        s_sock_read (thread, tcb->frame_header, 2);
    </action>

    <action name = "check protocol header">
        if (tcb->frame_header [0] != AMQP_ID
        ||  tcb->frame_header [1] != AMQP_VERSION) {
            icl_console_print ("E: client sent invalid protocol header - 0x%02x 0x%02x",
                tcb->frame_header [0], tcb->frame_header [1]);
            smt_thread_raise_exception (thread, connection_error_event);
        }
    </action>

    <action name = "send connection challenge">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_challenge_new (
            AMQP_VERSION,
            amq_global_mechanism_str (),
            NULL);                      /*  No fields in outgoing command    */
        send_the_frame (thread);
    </action>

    <action name = "read next command">
        <action name = "read frame header normal" />
        <action name = "read frame header escaped" />
        <action name = "read frame body" />
        <action name = "decode the frame" />
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

        if (tcb->frame_size > tcb->frame_max) {
            tcb->reply_code = AMQP_FRAME_ERROR;
            tcb->reply_text = "Client sent oversized fragment";
            smt_thread_raise_exception (thread, connection_error_event);
        }
        else
            s_sock_read (thread, tcb->command->data, tcb->frame_size);
    </action>

    <action name = "decode the frame">
        amq_frame_free (&tcb->frame);
        tcb->command->cur_size = tcb->socket->io_size;
        tcb->frame = amq_frame_decode (tcb->command);
        if (tcb->frame) {
            if (s_tracing > AMQP_TRACE_NONE)
                amq_frame_dump (tcb->frame, "IN  ");

            switch (tcb->frame->type) {
                /*  Connection commands                                          */
                case FRAME_TYPE_CONNECTION_RESPONSE:
                    the_next_event = connection_response_event;
                    break;
                case FRAME_TYPE_CONNECTION_TUNE:
                    the_next_event = connection_tune_event;
                    break;
                case FRAME_TYPE_CONNECTION_OPEN:
                    the_next_event = connection_open_event;
                    break;
                case FRAME_TYPE_CONNECTION_PING:
                    the_next_event = connection_ping_event;
                    break;
                case FRAME_TYPE_CONNECTION_CLOSE:
                    the_next_event = connection_close_event;
                    break;

                /*  Channel commands                                             */
                case FRAME_TYPE_CHANNEL_OPEN:
                    use_command_channel (thread, CHANNEL_OPEN.channel_id);
                    the_next_event = channel_open_event;
                    break;
                case FRAME_TYPE_CHANNEL_ACK:
                    use_command_channel (thread, CHANNEL_ACK.channel_id);
                    the_next_event = channel_method_event;
                    break;
                case FRAME_TYPE_CHANNEL_COMMIT:
                    use_command_channel (thread, CHANNEL_COMMIT.channel_id);
                    the_next_event = channel_method_event;
                    break;
                case FRAME_TYPE_CHANNEL_ROLLBACK:
                    use_command_channel (thread, CHANNEL_ROLLBACK.channel_id);
                    the_next_event = channel_method_event;
                    break;
                case FRAME_TYPE_CHANNEL_CLOSE:
                    use_command_channel (thread, CHANNEL_CLOSE.channel_id);
                    the_next_event = channel_close_event;
                    break;

                /*  Handle commands                                              */
                case FRAME_TYPE_HANDLE_OPEN:
                    use_command_channel (thread, HANDLE_OPEN.channel_id);
                    use_command_handle  (thread, HANDLE_OPEN.handle_id);
                    the_next_event  = handle_open_event;
                    break;
                case FRAME_TYPE_HANDLE_SEND:
                    use_command_handle (thread, HANDLE_SEND.handle_id);
                    the_next_event = handle_send_event;
                    break;
                case FRAME_TYPE_HANDLE_CONSUME:
                    use_command_handle (thread, HANDLE_CONSUME.handle_id);
                    the_next_event = handle_method_event;
                    break;
                case FRAME_TYPE_HANDLE_FLOW:
                    use_command_handle (thread, HANDLE_FLOW.handle_id);
                    the_next_event = handle_method_event;
                    break;
                case FRAME_TYPE_HANDLE_UNGET:
                    use_command_handle (thread, HANDLE_UNGET.handle_id);
                    the_next_event = handle_method_event;
                    break;
                case FRAME_TYPE_HANDLE_QUERY:
                    use_command_handle (thread, HANDLE_QUERY.handle_id);
                    the_next_event = handle_method_event;
                    break;
                case FRAME_TYPE_HANDLE_BROWSE:
                    use_command_handle (thread, HANDLE_BROWSE.handle_id);
                    the_next_event = handle_method_event;
                    break;
                case FRAME_TYPE_HANDLE_PREPARE:
                    use_command_handle (thread, HANDLE_PREPARE.handle_id);
                    the_next_event = handle_prepare_event;
                    break;
                case FRAME_TYPE_HANDLE_READY:
                    use_command_handle (thread, HANDLE_READY.handle_id);
                    the_next_event = handle_ready_event;
                    break;
                case FRAME_TYPE_HANDLE_CLOSE:
                    use_command_handle (thread, HANDLE_CLOSE.handle_id);
                    the_next_event = handle_close_event;
                    break;

                default:
                    tcb->reply_code = AMQP_FRAME_ERROR;
                    tcb->reply_text = "Frame type is not recognised";
                    smt_thread_raise_exception (thread, connection_error_event);
            }
        }
        else {
            tcb->reply_code = AMQP_FRAME_ERROR;
            tcb->reply_text = "Frame type is not recognised";
            smt_thread_raise_exception (thread, connection_error_event);
        }
    </action>

    <!--  EXPECT CONNECTION RESPONSE  ---------------------------------------->

    <state name = "expect connection response">
        <event name = "connection response" nextstate = "expect connection tune">
            <action name = "process connection response" />
            <action name = "send connection tune" />
            <action name = "read next command" />
        </event>
        <event name = "insufficient">
            <action name = "send connection challenge" />
            <action name = "read next command" />
        </event>
    </state>

    <action name = "process connection response">
        amq_connection_response (tcb->connection, &CONNECTION_RESPONSE);
        if (!tcb->connection->authorised) {
            tcb->reply_code = amq_global_error_code ();
            tcb->reply_text = amq_global_error_text ();
            smt_thread_raise_exception (thread, connection_error_event);
        }
    </action>

    <action name = "send connection tune">
        amq_field_list_t
            *fields;
        ipr_longstr_t
            *options = NULL;

        fields = amq_field_list_new (NULL);
        amq_field_new_integer (fields, "FRAME_MAX",   tcb->connection->frame_max);
        amq_field_new_integer (fields, "CHANNEL_MAX", tcb->connection->channel_max);
        amq_field_new_integer (fields, "HANDLE_MAX",  tcb->connection->handle_max);
        amq_field_new_integer (fields, "HEARTBEAT",   AMQP_HEARTBEAT);
        amq_field_new_integer (fields, "TXN_LIMIT",   amq_txn_limit);
        options = amq_field_list_flatten (fields);
        amq_field_list_destroy (&fields);

        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_tune_new (options, NULL);
        send_the_frame (thread);
        ipr_longstr_destroy (&options);
    </action>

    <!--  EXPECT CONNECTION TUNE  -------------------------------------------->

    <state name = "expect connection tune">
        <event name = "connection tune" nextstate = "expect connection tune">
            <action name = "process connection tune" />
            <action name = "read next command" />
        </event>
        <event name = "connection open" nextstate = "connection active">
            <action name = "process connection open" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "process connection tune">
        amq_connection_tune (tcb->connection, &CONNECTION_TUNE);
        tcb->frame_max = tcb->connection->frame_max;
    </action>

    <action name = "process connection open">
        amq_global_reset_error ();
        amq_connection_open (tcb->connection, amq_vhosts, &CONNECTION_OPEN);
        tcb->vhost      = tcb->connection->vhost;
        tcb->client_id  = tcb->connection->client_id;
        connection_reply_if_needed (thread, CONNECTION_OPEN.confirm_tag);
    </action>

    <action name = "wait for activity">
        smt_socket_request_monitor (
            thread, tcb->socket, socket_input_event, SMT_NULL_EVENT);
    </action>

    <!--  CONNECTION ACTIVE  ------------------------------------------------->

    <state name = "connection active">
        <event name = "connection ping">
            <action name = "process connection ping" />
        </event>
        <event name = "channel open" nextstate = "have channel open">
            <action name = "check channel state" />
        </event>
        <event name = "channel method" nextstate = "have channel method">
            <action name = "check channel state" />
        </event>
        <event name = "channel close" nextstate = "have channel close">
            <action name = "check channel state" />
        </event>
        <event name = "handle open" nextstate = "have handle open">
            <action name = "assert channel is open" />
            <action name = "check handle state" />
        </event>
        <event name = "handle method" nextstate = "have handle method">
            <action name = "use channel for handle" />
            <action name = "assert channel is open" />
            <action name = "check handle state" />
        </event>
        <event name = "handle send" nextstate = "have handle send">
            <action name = "use channel for handle" />
            <action name = "assert channel is open" />
            <action name = "check handle state" />
        </event>
        <event name = "handle prepare" nextstate = "have handle prepare">
            <action name = "use channel for handle" />
            <action name = "assert channel is open" />
            <action name = "check handle state" />
        </event>
        <event name = "handle close" nextstate = "have handle close">
            <action name = "use channel for handle" />
            <action name = "assert channel is open" />
            <action name = "check handle state" />
        </event>
        <event name = "channel closing">
            <action name = "signal not allowed for channel" />
            <action name = "send channel close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "process connection ping">
        if (CONNECTION_PING.respond) {
            amq_frame_free (&tcb->frame);
            tcb->frame = amq_frame_connection_ping_new (FALSE);
            send_the_frame (thread);
        }
    </action>

    <action name = "check channel state">
        if (tcb->channel == NULL)
            the_next_event = undefined_event;
        else
        if (tcb->channel->state == AMQ_CHANNEL_OPEN)
            the_next_event = open_event;
        else
        if (tcb->channel->state == AMQ_CHANNEL_CLOSING)
            the_next_event = closing_event;
    </action>

    <action name = "assert channel is open">
        if (tcb->channel == NULL || tcb->channel->state == AMQ_CHANNEL_CLOSED) {
            tcb->reply_code = AMQP_CHANNEL_ERROR;
            tcb->reply_text = "Channel is not open";
            smt_thread_raise_exception (thread, connection_error_event);
        }
        else
        if (tcb->channel->state == AMQ_CHANNEL_CLOSING)
            smt_thread_raise_exception (thread, channel_closing_event);
    </action>

    <action name = "check handle state">
        if (tcb->handle == NULL)
            the_next_event = undefined_event;
        else
        if (tcb->handle->state == AMQ_HANDLE_OPEN)
            the_next_event = open_event;
        else
        if (tcb->handle->state == AMQ_HANDLE_CLOSING)
            the_next_event = closing_event;
    </action>

    <action name = "use channel for handle">
        if (tcb->handle)
            tcb->channel_id = tcb->handle->channel_id;
        else {
            tcb->reply_code = AMQP_HANDLE_ERROR;
            tcb->reply_text = "Handle or channel is not open";
            smt_thread_raise_exception (thread, connection_error_event);
        }
    </action>

    <!--  HAVE CHANNEL OPEN  ------------------------------------------------->

    <state name = "have channel open">
        <event name = "undefined" nextstate = "connection active">
            <action name = "check sufficient resources" />
            <action name = "process channel open" />
            <action name = "wait for activity" />
        </event>
        <event name = "open">
            <action name = "invalid connection command" />
        </event>
        <event name = "closing" nextstate = "connection active" >
            <action name = "signal not allowed for channel" />
            <action name = "send channel close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "check sufficient resources">
    if (amq_max_memory > 0
    &&  icl_mem_usage () > amq_max_memory) {
        icl_console_print ("W: active memory has reached limit (%ld bytes)", amq_max_memory);
        icl_system_purge ();
        icl_console_print ("W: -- after garbage-collection: usage=%ld", icl_mem_usage ());
        if (icl_mem_usage () > amq_max_memory) {
            tcb->reply_code = AMQP_RESOURCE_ERROR;
            tcb->reply_text = "Server is too busy - try again later";
            smt_thread_raise_exception (thread, connection_error_event);
        }
    }
    </action>

    <action name = "process channel open">
        amq_global_reset_error ();
        amq_channel_unlink (&tcb->channel);
        tcb->channel = amq_channel_new (tcb->connection->channels, tcb->channel_id, tcb->connection, &CHANNEL_OPEN);
        if (!tcb->channel)
            tcb->reply_code = AMQP_SYNTAX_ERROR;

        channel_reply_if_needed (thread, CHANNEL_OPEN.confirm_tag);
    </action>

    <action name = "signal not allowed for channel">
        tcb->reply_code = AMQP_NOT_ALLOWED;
        tcb->reply_text = "Channel is closing, command not allowed";
    </action>

    <action name = "send channel close">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_channel_close_new (
            (dbyte) tcb->channel->key, tcb->reply_code, tcb->reply_text);
        send_the_frame (thread);
        tcb->channel->state = AMQ_CHANNEL_CLOSING;
    </action>

    <!--  HAVE CHANNEL METHOD  ----------------------------------------------->

    <state name = "have channel method">
        <event name = "undefined">
            <action name = "invalid connection command" />
        </event>
        <event name = "open" nextstate = "connection active">
            <action name = "process channel method" />
            <action name = "wait for activity" />
        </event>
        <event name = "closing" nextstate = "connection active" >
            <action name = "signal not allowed for channel" />
            <action name = "send channel close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "process channel method">
        amq_global_reset_error ();
        switch (tcb->frame->type) {
            case FRAME_TYPE_CHANNEL_COMMIT:
                amq_channel_commit (tcb->channel);
                channel_reply_if_needed (thread, CHANNEL_COMMIT.confirm_tag);
                break;

            case FRAME_TYPE_CHANNEL_ROLLBACK:
                amq_channel_rollback (tcb->channel);
                channel_reply_if_needed (thread, CHANNEL_ROLLBACK.confirm_tag);
                break;

            case FRAME_TYPE_CHANNEL_ACK:
                amq_channel_ack (tcb->channel, &CHANNEL_ACK);
                channel_reply_if_needed (thread, CHANNEL_ACK.confirm_tag);
                amq_vhost_dispatch (tcb->vhost);
                break;

            default:
                icl_console_print ("E: invalid frame type in process_channel_method");
                tcb->reply_code = AMQP_INTERNAL_ERROR;
                tcb->reply_text = "Internal server error";
                smt_thread_raise_exception (thread, connection_error_event);
        }
    </action>

    <!--  HAVE CHANNEL CLOSE  ------------------------------------------------>

    <state name = "have channel close">
        <event name = "undefined">
            <action name = "invalid connection command" />
        </event>
        <event name = "open" nextstate = "connection active">
            <action name = "signal reply success" />
            <action name = "send channel close" />
            <action name = "process channel close" />
            <action name = "wait for activity" />
        </event>
        <event name = "closing" nextstate = "connection active">
            <action name = "process channel close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "signal reply success">
        tcb->reply_code = AMQP_REPLY_SUCCESS;
        tcb->reply_text = "Closing";
    </action>

    <action name = "process channel close">
        /*  We destroy the channel object                                    */
        amq_channel_destroy (&tcb->channel);
    </action>

    <!--  HAVE HANDLE OPEN  -------------------------------------------------->

    <state name = "have handle open">
        <event name = "undefined" nextstate = "connection active">
            <action name = "check sufficient resources" />
            <action name = "process handle open" />
            <action name = "wait for activity" />
        </event>
        <event name = "open">
            <action name = "invalid channel command" />
        </event>
        <event name = "closing" nextstate = "connection active" >
            <action name = "signal not allowed for handle" />
            <action name = "send handle close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "process handle open">
        amq_global_reset_error ();
        amq_handle_unlink (&tcb->handle);
        tcb->handle = amq_handle_new (tcb->connection->handles, tcb->handle_id, tcb->channel, &HANDLE_OPEN);
        if (tcb->handle)
            tcb->handle->client_id = tcb->client_id;

        handle_reply_if_needed (thread, HANDLE_OPEN.confirm_tag);
    </action>

    <action name = "signal not allowed for handle">
        tcb->reply_code = AMQP_NOT_ALLOWED;
        tcb->reply_text = "Handle is closing, command not allowed";
    </action>

    <action name = "send handle close">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_close_new (
            (dbyte) tcb->handle->key, tcb->reply_code, tcb->reply_text);
        send_the_frame (thread);
        tcb->handle->state = AMQ_HANDLE_CLOSING;
    </action>

    <!--  HAVE HANDLE METHOD  ------------------------------------------------>

    <state name = "have handle method">
        <event name = "undefined">
            <action name = "invalid channel command" />
        </event>
        <event name = "open" nextstate = "connection active">
            <action name = "process handle method" />
            <action name = "wait for activity" />
        </event>
        <event name = "closing">
            <action name = "invalid channel command" />
        </event>
    </state>

    <action name = "process handle method">
        dbyte
            confirm_tag;

        amq_global_reset_error ();
        switch (tcb->frame->type) {
            case FRAME_TYPE_HANDLE_CONSUME:
                amq_handle_consume (tcb->handle, &HANDLE_CONSUME);
                handle_reply_if_needed (thread, HANDLE_CONSUME.confirm_tag);
                amq_vhost_dispatch (tcb->vhost);
                break;
            case FRAME_TYPE_HANDLE_FLOW:
                amq_handle_flow (tcb->handle, &HANDLE_FLOW);
                handle_reply_if_needed (thread, HANDLE_FLOW.confirm_tag);
                amq_vhost_dispatch (tcb->vhost);
                break;
            case FRAME_TYPE_HANDLE_UNGET:
                amq_handle_unget (tcb->handle, &HANDLE_UNGET);
                handle_reply_if_needed (thread, HANDLE_UNGET.confirm_tag);
                amq_vhost_dispatch (tcb->vhost);
                break;
            case FRAME_TYPE_HANDLE_QUERY:
                amq_handle_query (tcb->handle, &HANDLE_QUERY);
                handle_reply_if_needed (thread, 0);
                break;
            case FRAME_TYPE_HANDLE_BROWSE:
                amq_handle_browse (tcb->handle, &HANDLE_BROWSE);
                /*  Do not raise an exception on errors                      */
                if (amq_global_error_code ()) {
                    confirm_tag = HANDLE_BROWSE.confirm_tag;
                    amq_frame_free (&tcb->frame);
                    tcb->frame = amq_frame_handle_reply_new (
                        (dbyte) tcb->handle->key,
                        (dbyte) confirm_tag,
                        (dbyte) amq_global_error_code (),
                                amq_global_error_text ());
                    send_the_frame (thread);
                }
                break;
            default:
                icl_console_print ("E: invalid frame type in process_handle_method");
                tcb->reply_code = AMQP_INTERNAL_ERROR;
                tcb->reply_text = "Internal server error";
                smt_thread_raise_exception (thread, connection_error_event);
        }
    </action>

    <!--  HAVE HANDLE SEND  -------------------------------------------------->

    <state name = "have handle send">
        <event name = "undefined">
            <action name = "invalid channel command" />
        </event>
        <event name = "open" nextstate = "after handle send">
            <action name = "check sufficient resources" />
            <action name = "process handle send" />
            <action name = "process handle send fragment" />
        </event>
        <event name = "closing">
            <action name = "invalid channel command" />
        </event>
    </state>

    <action name = "process handle send">
        if (HANDLE_SEND.fragment_size > tcb->connection->frame_max) {
            tcb->reply_code = AMQP_FRAME_ERROR;
            tcb->reply_text = "Client sent oversized fragment";
            smt_thread_raise_exception (thread, channel_error_event);
        }
        /*  Grab a new bucket for the incoming message data                  */
        amq_bucket_destroy (&tcb->fragment);  /*  JS: ??? */
        tcb->fragment = amq_bucket_new (HANDLE_SEND.fragment_size);
        s_sock_read (thread, tcb->fragment->data, HANDLE_SEND.fragment_size);
    </action>

    <action name = "process handle send fragment">
        /*  tcb->channel->message_in is the current message, if any          */
        if (tcb->channel->message_in == NULL)
            tcb->channel->message_in = amq_smessage_new (tcb->handle);

        tcb->fragment->cur_size = tcb->socket->io_size;
        amq_smessage_record (
            tcb->channel->message_in, tcb->fragment, HANDLE_SEND.partial);
        amq_bucket_destroy (&tcb->fragment);

        if (HANDLE_SEND.partial)
            the_next_event = continue_event;
        else {
            amq_global_reset_error ();
            amq_handle_send (tcb->handle, &HANDLE_SEND, tcb->channel->message_in);
            amq_smessage_destroy (&tcb->channel->message_in);
            handle_reply_if_needed (thread, HANDLE_SEND.confirm_tag);
            the_next_event = finished_event;
        }
    </action>

    <!--  AFTER HANDLE SEND  ------------------------------------------------->

    <state name = "after handle send">
        <event name = "continue" nextstate = "expect handle send">
            <action name = "wait for activity" />
        </event>
        <event name = "finished" nextstate = "connection active">
            <action name = "wait for activity" />
        </event>
    </state>

    <!--  EXPECT HANDLE SEND  ------------------------------------------------>

    <state name = "expect handle send">
        <event name = "handle send" nextstate = "after handle send">
            <action name = "process handle send" />
            <action name = "process handle send fragment" />
        </event>
        <event name = "channel close" nextstate = "connection active">
            <action name = "signal reply success" />
            <action name = "send channel close" />
            <action name = "process channel close" />
            <action name = "wait for activity" />
        </event>
        <event name = "handle close" nextstate = "connection active">
            <action name = "process handle close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <!--  HAVE HANDLE PREPARE  ----------------------------------------------->

    <state name = "have handle prepare">
        <event name = "undefined">
            <action name = "invalid channel command" />
        </event>
        <event name = "open" nextstate = "after handle send">
            <action name = "process handle prepare" />
            <action name = "process handle prepare fragment" />
        </event>
        <event name = "closing">
            <action name = "invalid channel command" />
        </event>
    </state>

    <action name = "process handle prepare">
        if (HANDLE_PREPARE.fragment_size > tcb->connection->frame_max) {
            tcb->reply_code = AMQP_FRAME_ERROR;
            tcb->reply_text = "Client sent oversized fragment";
            smt_thread_raise_exception (thread, channel_error_event);
        }
            
        amq_bucket_destroy (&tcb->fragment);
        tcb->fragment = amq_bucket_new (HANDLE_PREPARE.fragment_size);
        s_sock_read (thread, tcb->fragment->data, HANDLE_PREPARE.fragment_size);
    </action>

    <action name = "process handle prepare fragment">
        qbyte
            size;

        /*  tcb->channel->message_in is the current message, if any          */
        if (tcb->channel->message_in == NULL)
            tcb->channel->message_in = amq_smessage_new (tcb->handle);

        /*  Check message header against known list                          */
        tcb->fragment->cur_size = tcb->socket->io_size;
        size = amq_smessage_prepare (tcb->channel->message_in, tcb->fragment);
        amq_bucket_destroy (&tcb->fragment);

        /*  Send HANDLE READY in response                                    */
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_ready_new (handle_notify_m->handle_id, size);
        send_the_frame (thread);

        amq_smessage_destroy (&tcb->channel->message_in);
    </action>

    <!--  HAVE HANDLE CLOSE  ------------------------------------------------->

    <state name = "have handle close">
        <event name = "undefined">
            <action name = "invalid channel command" />
        </event>
        <event name = "open" nextstate = "connection active">
            <action name = "signal reply success" />
            <action name = "send handle close" />
            <action name = "process handle close" />
            <action name = "wait for activity" />
        </event>
        <event name = "closing" nextstate = "connection active">
            <action name = "process handle close" />
            <action name = "wait for activity" />
        </event>
    </state>

    <action name = "process handle close">
        /*  We destroy the handle object                                     */
        amq_handle_unlink  (&tcb->handle);
        amq_handle_destroy (&tcb->handle);
    </action>

    <!--  SENDING MESSAGE  --------------------------------------------------->

    <state name = "sending message">
        <event name = "start">
            amq_bucket_destroy (&tcb->fragment);
            tcb->fragment = amq_bucket_new (tcb->frame_max);
            <action name = "send message fragment" />
        </event>
        <event name = "continue">
            <action name = "send message fragment" />
        </event>
        <event name = "finished">
            amq_bucket_destroy (&tcb->fragment);
            <return/>
        </event>
    </state>

    <action name = "send message fragment">
        Bool
            partial;

        partial = amq_smessage_replay (
            handle_notify_m->message, tcb->fragment, tcb->frame_max);

        /*  Send HANDLE NOTIFY command frame                                 */
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_notify_new (
            handle_notify_m->handle_id,
            handle_notify_m->message_nbr,
            tcb->fragment->cur_size,
            partial,                    /*  Partial message?                 */
            FALSE,                      /*  Out of band data?                */
            handle_notify_m->recovery,
            handle_notify_m->delivered,
            handle_notify_m->redelivered,
            partial? NULL: handle_notify_m->dest_name);
        send_the_frame (thread);

        /*  Send data buffer (without any frame header)                      */
        s_sock_write (thread, tcb->fragment->data, tcb->fragment->cur_size);

        if (partial)
            the_next_event = continue_event;
        else {
            the_next_event = finished_event;

            /*  We're finished with the message                              */
            amq_smessage_destroy (&handle_notify_m->message);
        }
    </action>

    <!--  CLIENT DEFAULTS  --------------------------------------------------->

    <state name = "defaults">
        <!--  Commands received on the connection  -->
        <event name = "connection close" nextstate = "">
            <action name = "signal reply success" />
            <action name = "send connection close" />
            <action name = "close the connection" />
        </event>
        <event name = "connection response">
            <action name = "invalid connection command" />
        </event>
        <event name = "connection tune">
            <action name = "invalid connection command" />
        </event>
        <event name = "connection open">
            <action name = "invalid connection command" />
        </event>
        <event name = "connection ping">
            <action name = "invalid connection command" />
        </event>
        <event name = "channel open">
            <action name = "invalid connection command" />
        </event>
        <event name = "channel method">
            <action name = "invalid connection command" />
        </event>
        <event name = "channel close">
            <action name = "invalid connection command" />
        </event>
        <event name = "handle open">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle method">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle ready">
            <action name = "invalid channel command" />
        </event>
        <event name = "handle close">
            <action name = "invalid channel command" />
        </event>
        <event name = "connection error" nextstate = "">
            <action name = "send connection close" />
            <action name = "close the connection" />
        </event>
        <event name = "channel error" nextstate = "connection active">
            <action name = "send channel close" />
            <action name = "wait for activity" />
        </event>

        <!--  Events from SMT socket monitor  -->
        <event name = "socket input">
            <action name = "read next command" />
        </event>

        <!-- Requests from self or amq classes to talk back to client -->
        <method name = "handle created">
            <action name = "send handle created" />
            <action name = "wait for activity" />
        </method>
        <method name = "handle notify">
            <call state = "sending message" event = "start" />
            <action name = "wait for activity" />
        </method>
        <method name = "handle index">
            <action name = "send handle index" />
            <action name = "wait for activity" />
        </method>
        <method name = "connection reply">
            <action name = "send connection reply" />
            <action name = "wait for activity" />
        </method>
        <method name = "channel reply">
            <action name = "send channel reply" />
            <action name = "wait for activity" />
        </method>
        <method name = "handle reply">
            <action name = "send handle reply" />
            <action name = "wait for activity" />
        </method>
    </state>

    <action name = "send connection close">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_close_new (tcb->reply_code, tcb->reply_text);
        send_the_frame (thread);
    </action>

    <action name = "close the connection">
        smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
        amq_connection_close (tcb->connection);
    </action>

    <action name = "invalid connection command">
        tcb->reply_code = AMQP_COMMAND_INVALID;
        tcb->reply_text = "Command is not valid at this time";
        smt_thread_raise_exception (thread, connection_error_event);
    </action>

    <action name = "invalid channel command">
        tcb->reply_code = AMQP_COMMAND_INVALID;
        tcb->reply_text = "Command is not valid at this time";
        smt_thread_raise_exception (thread, channel_error_event);
    </action>

    <action name = "send handle created">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_created_new (
            handle_created_m->handle_id, handle_created_m->dest_name);
        send_the_frame (thread);
    </action>

    <action name = "send handle index">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_index_new (
            handle_index_m->handle_id, handle_index_m->message_nbr, handle_index_m->message_list);
        send_the_frame (thread);
        ipr_longstr_destroy (&handle_index_m->message_list);
    </action>

    <action name = "send connection reply">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_connection_reply_new (
            connection_reply_m->confirm_tag,
            AMQP_REPLY_SUCCESS, NULL);
        send_the_frame (thread);
    </action>

    <action name = "send channel reply">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_channel_reply_new (
            (dbyte) tcb->channel->key,
            channel_reply_m->confirm_tag,
            AMQP_REPLY_SUCCESS, NULL);
        send_the_frame (thread);
    </action>

    <action name = "send handle reply">
        amq_frame_free (&tcb->frame);
        tcb->frame = amq_frame_handle_reply_new (
            (dbyte) tcb->handle->key,
            handle_reply_m->confirm_tag,
            AMQP_REPLY_SUCCESS, NULL);
        send_the_frame (thread);
    </action>
</thread>

<state name = "defaults">
    <event name = "socket error" nextstate = "">
    </event>
    <event name = "smt error" nextstate = "">
        <action name = "handle error"/>
    </event>
    <event name = "shutdown" nextstate = "" />
</state>

<action name = "handle error">
    icl_console_print ("E: %s", smt_thread_error (thread));
</action>

<private name = "types">
static void        send_the_frame (smt_thread_t *thread);
static void inline s_sock_write   (smt_thread_t *thread, byte *buffer, size_t size);
static void inline s_sock_read    (smt_thread_t *thread, byte *buffer, size_t size);
</private>

<private name = "functions">
#define tcb ((amq_server_agent_client_context_t *) thread->context)


/*  Get handle referred to in command, if not same as last, lookup in
    table of handles opened for this connection.
 */
static void
use_command_handle (smt_thread_t *thread, dbyte handle_id)
{
    if (handle_id == 0) {
        tcb->reply_code = AMQP_SYNTAX_ERROR;
        tcb->reply_text = "Handle ID may not be zero";
        smt_thread_raise_exception (thread, connection_error_event);
    }
    else
    if (tcb->handle_id != handle_id) {
        amq_handle_unlink (&tcb->handle);
        tcb->handle    = amq_handle_table_search (tcb->connection->handles, handle_id);
        tcb->handle_id = handle_id;
    }
}

/*  Get channel referred to in command, if not same as last, lookup in
    table of channels opened for this connection.
 */
static void
use_command_channel (smt_thread_t *thread, dbyte channel_id)
{
    if (channel_id == 0) {
        tcb->reply_code = AMQP_SYNTAX_ERROR;
        tcb->reply_text = "Channel ID may not be zero";
        smt_thread_raise_exception (thread, connection_error_event);
    }
    else
    if (tcb->channel_id != channel_id) {
        amq_channel_unlink (&tcb->channel);
        tcb->channel    = amq_channel_table_search (tcb->connection->channels, channel_id);
        tcb->channel_id = channel_id;
    }
}

static void
connection_reply_if_needed (smt_thread_t *thread, dbyte confirm_tag)
{
    tcb->reply_code = amq_global_error_code ();
    tcb->reply_text = amq_global_error_text ();
    if (tcb->reply_code)
        smt_thread_raise_exception (thread, connection_error_event);
    else
    if (confirm_tag)
        amq_server_agent_connection_reply (thread, confirm_tag);
}

static void
channel_reply_if_needed (smt_thread_t *thread, dbyte confirm_tag)
{
    tcb->reply_code = amq_global_error_code ();
    tcb->reply_text = amq_global_error_text ();

    if (tcb->reply_code)
        smt_thread_raise_exception (thread, channel_error_event);
    else
    if (confirm_tag)
        amq_server_agent_channel_reply (thread, confirm_tag);
}

static void
handle_reply_if_needed (smt_thread_t *thread, dbyte confirm_tag)
{
    tcb->reply_code = amq_global_error_code ();
    tcb->reply_text = amq_global_error_text ();

    if (tcb->reply_code)
        smt_thread_raise_exception (thread, channel_error_event);
    else
    if (confirm_tag)
        amq_server_agent_handle_reply (thread, confirm_tag);
}

/*  Send the current frame and free frame block                              */

static void
send_the_frame (smt_thread_t *thread)
{
    amq_frame_encode (tcb->command, tcb->frame);
    assert (tcb->command->cur_size == tcb->frame->size);

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
