<!--
    amq_common.smt

    Common SMT functions used by AMQP servers and clients
 -->

<state name = "defaults">
    <event name = "socket closed" nextstate = "" >
    </event>
    <event name = "socket error"  nextstate = "" >
        <action name = "handle socket error">
        coprintf ("E: socket error (%s)", sockmsg ());
        </action>
    </event>
    <event name = "smt error" nextstate = "">
        <action name = "handle smt error">
        coprintf ("E: unhandled SMT kernel error: %d", thread->result);
        </action>
    </event>
    <event name = "shutdown" nextstate = "" />
</state>

<catch error = "SMT_SOCKET_CLOSED"    event = "socket closed" />
<catch error = "SMT_SOCKET_ERROR"     event = "socket error" />
<catch error = "SMT_FULL"             event = "smt error" />
<catch error = "SMT_EMPTY"            event = "smt error" />
<catch error = "SMT_ILLEGAL_ARGUMENT" event = "smt error" />
<catch error = "SMT_SELECT_ERROR"     event = "smt error" />
<catch error = "SMT_SEQUENCE_ERROR"   event = "smt error" />
<catch error = "SMT_CONNECT_ERROR"    event = "socket error" />

<action name = "wait for activity" >
    smt_socket_request_monitor (
        thread, tcb->socket, socket_input_event, SMT_NULL_EVENT);
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
        coprintf ("E: received frame is too large (want %ld, have %ld)",
            tcb->frame_size, tcb->frame_max);
        smt_thread_raise_exception (thread, connection_error_event);
    }
    else
        s_sock_read (thread, tcb->command->data, tcb->frame_size);
</action>

