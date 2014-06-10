<?xml?>
<!--
    *** GENERATED FROM demo_server_proto.asl BY ASL_GEN USING GSL/4 ***

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
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
<agent
    name      = "demo_server_agent"
    script    = "smt_gen.gsl"
    animate   = "1"
    license   = "gpl"
    >

<!--  Theory of operation

      The protocol agent is comprised of several cooperating threads:

      Master thread:

      In the server, the Master thread creates the listening socket, accepts
      incoming connections on it and creates a Connection thread for each new
      connection. It also registers handlers to catch SIGTERM and SIGINT, and
      performs an orderly shutdown of all active connections on receipt of the
      interrupt event (see the "defaults" and "shutting down" states).

      Connection thread:

      The Connection thread handles connection initialization (the "initialise
      connection" state), sending outgoing protocol methods (the "connection
      active" state) and ulimately connection shutdown (the "close the
      connection" state). It is also responsible for creating and ensuring
      shutdown of the Input and Heartbeat threads.

      In the server, the Connection thread owns the corresponding synchronous
      demo_server_connection object. In the client, the Connection
      thread is created by the demo_client_connection object, and
      is responsible for signalling it in the event of a connection shutdown.

      The Connection thread handles the Direct Protocol, if requested by the
      client.

      Input thread:

      The input thread is responsible for decoding and processing any incoming
      protocol methods. In the server, protocol methods are passed by the
      s_execute_method and s_execute_functional_method functions to
      demo_server_method_execute for processing. In the client any
      connection and channel methods are processed internally by the agent, and
      s_execute_functional_method sends the method to
      demo_client_session for processing.

      Heartbeat thread and heartbeating:

      The Heartbeat thread asynchronously monitors peer liveness by waking up
      once every heartbeat interval and decrementing the liveness value. If
      the liveness reaches zero the peer is pronounced dead and the heartbeat
      thread will shut down the agent by shutting down the Connection and
      Input threads. smt_thread_shut_down is used since the agent threads are
      likely to be blocked on socket requests and we need to override this.

      The liveness value is reset to the maximum value (FULL_LIVENESS) each
      time the Input thread reads a frame header from the peer. This allows
      any frame to reset the liveness, not just heartbeat frames. Outgoing
      heartbeat frames are sent by the connection thread once every heartbeat
      interval.

      Channel management:

      The current implementation only supports a single channel. For
      compatibility with existing code which expects to use channel objects, we
      create and destroy a single channel object. In the server, the channel
      table and single channel object are stored in the
      demo_server_connection object. In the client, the channel
      table and single channel object are stored in the connection context and
      the channel object is also our interface to the
      demo_client_session object, which is the application
      interface to a channel.

      Error handling:

      Error handling is complex due to the requirement to initiate a connection
      close sequence (and wait for it to complete!) on receipt of a connection
      error, and also due to the fact that we cannot safely interrupt the input
      thread with an exception since it may be in the middle of reading a frame
      (between "read frame header" and "read frame payload") which would result
      in the stream being desynchronized.

      We recognize the following classes of error, and summarize how each class
      is handled by the thread that detects the error:

      Hard error:

      This includes any network errors, peer heartbeat stopping, and any
      protocol errors where it is not safe to intiate a normal connection close
      handshake, e.g. malformed frames.

      Input thread: Raise an "external exception" in the Connection thread and
      shut down. The "external error" will cause the Connection thread to shut
      down immediately.

      Connection thread: Shut down the Input and Heartbeat threads and shut
      down the agent.

      Connection error:

      A protocol error detected by the agent where we wish to intiate a
      connection close handshake.

      Input thread: Raise a "connection error" exception in the Connection
      thread. Then, transition to the "connection closing" state and wait for
      receipt of a "connection close ok". On receipt of the "close ok" notify
      the Connection thread and shut down. The "connection error" exception in
      the Connection thread will cause it to immediately transition to the
      "close the connection" state and initiate an active close.

      Connection thread: Transition to the "close the connection" state and
      initiate an active close. The input thread will eventually receive the
      "connection close ok", notify the Connection thread and shut down.

      Passive close:

      This occurs if the Input thread receives a "connection close" from the
      peer. We notify the Connection thread using a "connection close"
      exception, and shut down. The "connection close" exception will cause the
      Connection thread to transition to the "close the connection" state, send
      the "connection close ok" and shut down the agent.

      Channel error executing protocol method (server only):

      An error raise asynchronously by a server agent executing a protocol
      method through a call to demo_server_channel_error. In this
      case we wish to close the channel as soon as possible, so this will
      result in a high priority "channel error" method being queued with the
      connection thread, which will immediately transition the channel to a
      CLOSING state and send the "channel close".

      Connection error executing protocol method: (server only)

      An error raised synchronously by demo_server_method_execute
      while executing a connection method through a call to self_exception, or
      an error raised asynchronously by a server agent executing a protocol
      method through a call to demo_server_connection_error.

      In the synchronous case, the Input thread will raise a "connection error"
      exception and proceed as in the "Connection error" case.

      In the asynchronous case, this will result in a "connection error" method
      being queued with the Connection thread, and at the same time an
      exception raised in the demo_server_connection object. The
      "connection error" method will cause the Connection thread to intiate an
      active close. The presence of an exception in the
      demo_server_connection object will cause the input thread to
      abort processing on the next frame boundary and transition to the
      "connection closing" state, however we also handle the case where the
      Input thread may receive the "connection close ok" immediately, in which
      case it will notify the Connection thread and shut down.

      Notes regarding "syncapi":

      The client agent can be generated in one of two variants. Syncapi is the
      normal client API. The syncapi=0 case is used to generate agents
      suitable for use as an asynchronous client incorporated inside a broker
      (e.g. for peering). The asynchronous case is basically a "dumb" client
      with no channel objects, and all interaction with the caller is done by
      calling $(basename)_method_execute (Input) or via the output methods,
      and/or $(basename)_agent_push (Output).
-->

<import class = "asl" />
<import class = "asl_field" />
<import class = "demo_constants" />
<import class = "demo_broker" />
<import class = "demo_lease" />
<import class = "demo_server_method" />

<private name = "types">
#define BASE_ANIMATE            1
#define FULL_LIVENESS           4       //  Four missing heartbeats -> dead

//  Channel states
#define ASL_CHANNEL_CLOSED      0
#define ASL_CHANNEL_OPEN        1
#define ASL_CHANNEL_CLOSING     2
#define ASL_CHANNEL_OPENING     3

#include "version.h"
</private>

<!--  Catch SMT error codes                                                 -->

<catch error = "SMT_SOCKET_ERROR"   event = "socket error" />
<catch error = "SMT_SOCKET_CLOSING" event = "socket closing" />
<catch error = "SMT_TIMEOUT"        event = "socket timeout" />
<catch                              event = "smt error" />

<!---------------------------------------------------------------------------->
<!--  Application methods that the agent accepts                            -->

<!--  Set per-connection trace level
      Called from demo_server_connection                                    -->
<method name = "set trace" priority = "high">
    <argument name = "trace" type = "int" />
</method>

<!--  Operator requested connection shutdown
      Called from demo_server_connection                                    -->
<method name = "kill connection" priority = "high">
</method>

<!--  Called by demo_server_connection_error to signal an error processing a
      method on the connection                                              -->
<method name = "connection error" priority = "high">
    <argument name = "reply code" type = "dbyte" />
    <argument name = "reply text" type = "char *" />
    <argument name = "faulting method id" type = "dbyte" />
    <argument name = "faulting class id" type = "dbyte" />
</method>

<!--  Called by demo_server_channel_error to signal an error processing a
      method on a channel                                                   -->
<method name = "channel error" priority = "high">
    <argument name = "channel" type = "demo_server_channel_t *" />
    <argument name = "reply code" type = "dbyte" />
    <argument name = "reply text" type = "char *" />
    <argument name = "faulting method id" type = "dbyte" />
    <argument name = "faulting class id" type = "dbyte" />
    <possess>
        channel = demo_server_channel_link (channel);
    </possess>
    <release>
        demo_server_channel_unlink (&channel);
    </release>
</method>

<!--  Called by input thread when a connection start ok is received -->
<method name = "connection start ok" priority = "high">
</method>

<!--  Called by input thread when a connection tune ok is received -->
<method name = "connection tune ok" priority = "high">
</method>

<!--  Called by input thread when a connection open is received -->
<method name = "connection open" priority = "high">
</method>

<!--  Called by input thread when a channel open is received -->
<method name = "send channel open ok" priority = "high">
</method>

<!--  Called by input thread when a channel close is received -->
<method name = "send channel close ok" priority = "high">
</method>

<!--  Called by input thread when a connection close ok is received -->
<method name = "connection close ok" priority = "high">
</method>

<!--  Called by master thread during server shutdown -->
<method name = "shutdown" priority = "high">
</method>

<!--  Outgoing protocol methods                                             -->

<!-- Enable/Disable Flow From Peer -->
<method name = "channel flow">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "active"      type = "Bool" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Confirm A Flow Method -->
<method name = "channel flow-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "active"      type = "Bool" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Confirms An Exchange Declaration -->
<method name = "exchange declare-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Confirm Deletion Of An Exchange -->
<method name = "exchange delete-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Confirms A Queue Definition -->
<method name = "queue declare-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "queue"       type = "char *" />
    <argument name = "properties"  type = "icl_longstr_t *" />
    <possess>
        queue = icl_mem_strdup (queue);
        properties = icl_longstr_dup (properties);
    </possess>
    <release>
        icl_mem_free (queue);
        icl_longstr_destroy (&properties);
    </release>
</method>

<!-- Confirm Bind Successful -->
<method name = "queue bind-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Confirms A Queue Purge -->
<method name = "queue purge-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "message_count"  type = "qbyte" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Confirm Deletion Of A Queue -->
<method name = "queue delete-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "message_count"  type = "qbyte" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Confirm A New Consumer -->
<method name = "basic consume-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "consumer_tag"  type = "char *" />
    <possess>
        consumer_tag = icl_mem_strdup (consumer_tag);
    </possess>
    <release>
        icl_mem_free (consumer_tag);
    </release>
</method>

<!-- Confirm A Cancelled Consumer -->
<method name = "basic cancel-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "consumer_tag"  type = "char *" />
    <possess>
        consumer_tag = icl_mem_strdup (consumer_tag);
    </possess>
    <release>
        icl_mem_free (consumer_tag);
    </release>
</method>

<!-- Notify The Client Of A Consumer Message -->
<method name = "basic deliver">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "content"     type = "demo_content_basic_t *" />
    <argument name = "exchange"    type = "char *" />
    <argument name = "routing_key"  type = "char *" />
    <argument name = "delivery_tag"  type = "int64_t" />
    <!-- Tells the connection what to do when the content has been sent -->
    <argument name = "callback"    type = "void *" />
    <possess>
        assert (content);
        content  = demo_content_basic_link (content);
        exchange = icl_mem_strdup (exchange);
        routing_key = icl_mem_strdup (routing_key);
    </possess>
    <release>
        demo_content_basic_unlink (&content);
        icl_mem_free (exchange);
        icl_mem_free (routing_key);
    </release>
</method>

<!-- Provide Client With A Message -->
<method name = "basic get-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "content"     type = "demo_content_basic_t *" />
    <argument name = "exchange"    type = "char *" />
    <argument name = "routing_key"  type = "char *" />
    <argument name = "message_count"  type = "qbyte" />
    <!-- Tells the connection what to do when the content has been sent -->
    <argument name = "callback"    type = "void *" />
    <possess>
        assert (content);
        content  = demo_content_basic_link (content);
        exchange = icl_mem_strdup (exchange);
        routing_key = icl_mem_strdup (routing_key);
    </possess>
    <release>
        demo_content_basic_unlink (&content);
        icl_mem_free (exchange);
        icl_mem_free (routing_key);
    </release>
</method>

<!-- Indicate No Messages Available -->
<method name = "basic get-empty">
    <argument name = "channel nbr" type = "dbyte" />
    <possess>
    </possess>
    <release>
    </release>
</method>

<!-- Grant Access To The Sink -->
<method name = "direct put-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "lease"       type = "char *" />
    <possess>
        lease = icl_mem_strdup (lease);
    </possess>
    <release>
        icl_mem_free (lease);
    </release>
</method>

<!-- Grant Access To The Feed -->
<method name = "direct get-ok">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "lease"       type = "char *" />
    <possess>
        lease = icl_mem_strdup (lease);
    </possess>
    <release>
        icl_mem_free (lease);
    </release>
</method>

<!--  Pass an already-prepared method to the client                         -->
<method name = "push">
    <argument name = "channel nbr" type = "dbyte" />
    <argument name = "method" type = "demo_server_method_t *" />
    <possess>
        method = demo_server_method_link (method);
    </possess>
    <release>
        demo_server_method_unlink (&method);
    </release>
</method>

<!-- Send a message from a feed using the direct protocol                   -->
<method name = "direct out">
    <argument name = "content" type = "demo_content_basic_t *" />
    <argument name = "options" type = "byte" />
    <possess>
        content = demo_content_basic_link (content);
    </possess>
    <release>
        demo_content_basic_unlink (&content);
    </release>
</method>


<!---------------------------------------------------------------------------->
<!--  Initialise agent, create master thread                                -->

<handler name = "agent init">
    <argument name = "thread p" type = "smt_thread_t **" />
    smt_thread_t
        *thread;

    rc = SMT_OK;
    if (demo_server_config_trace (demo_server_config) >= ASL_TRACE_HIGH)
        smt_socket_request_trace (TRUE);
    thread = demo_server_agent_master_thread_new ();
    *thread_p = thread;
    smt_thread_wake (thread);
    smt_thread_unlink (&thread);
</handler>

<!---------------------------------------------------------------------------->
<!--  Master thread                                                         -->

<thread name = "master" schedule = "poll">
    <context>
        smt_socket_t
            *master_socket,             //  Socket for connect requests
            *client_socket;             //  Newly-accepted socket
    </context>

    <handler name = "thread init">
        if (demo_server_config_trace (demo_server_config) >= ASL_TRACE_MED)
            thread->animate = TRUE;
        smt_thread_set_next_event (thread, ok_event);
    </handler>

    <handler name = "thread destroy">
        smt_socket_destroy (&tcb->client_socket);
        smt_socket_destroy (&tcb->master_socket);
    </handler>

    <!--  Initialise Master
          Create listening socket                                           -->

    <state name = "initialise master">
        <event name = "ok" nextstate = "accepting connections">
            <action name = "initialise server" />
            <action name = "allow thread to settle" />
            <action name = "report server ready" />
        </event>
    </state>

    <action name = "initialise server">
        icl_shortstr_t
            hostname;                   //  Holds host name
        apr_pool_t
            *pool;                      //  APR pool for host addresses
        apr_sockaddr_t
            *sockaddr;                  //  The associated socket addresses
        char
            *port,                      //  Port to bind to
            *address,                   //  To hold socket address string
            *user,                      //  User to run as
            *group;                     //  Group to run as
        int
            rc = SMT_OK,
            new_size,                   //  New buffer size
            old_size;                   //  Old buffer size
        static Bool
            rcv_warned = FALSE,
            snd_warned = FALSE;

        port  = demo_server_config_port  (demo_server_config);
        user  = demo_server_config_user  (demo_server_config);
        group = demo_server_config_group (demo_server_config);
        if (atoi (port) < 1024
        ||  atoi (port) > 65535) {
            smt_log_print (demo_broker->alert_log,
                "E: port %s is not valid for this service", port);
            exit (1);                   //  FIXME: see BASE2-289
        }
        else {
            apr_pool_create (&pool, icl_global_pool);
            apr_pool_tag (pool, "demo_server_initialise_server)");

            //  Report hostname and all visible network interfaces
            if (apr_gethostname (hostname, ICL_SHORTSTR_MAX, pool) == APR_SUCCESS) {
                apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
                if (sockaddr) {
                    apr_sockaddr_ip_get (&address, sockaddr);
                    smt_log_print (demo_broker->alert_log,
                        "I: hostname is %s (%s)", hostname, address);
                }
                else
                    smt_log_print (demo_broker->alert_log,
                        "E: cannot translate host name '%s'", hostname);
            }
            //  Report which interfaces server is going to listen to
            icl_shortstr_cpy (hostname, demo_server_config_listen (demo_server_config));
            if (streq (hostname, "*")) {
                smt_log_print (demo_broker->alert_log,
                    "I: listening on port %s, all network interfaces", port);
                icl_shortstr_cpy (hostname, APR_ANYADDR);
            }
            else
                smt_log_print (demo_broker->alert_log,
                    "I: listening on port %s, interface %s", port, hostname);

            apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
            if (sockaddr) {
                apr_sockaddr_ip_get (&address, sockaddr);
                icl_shortstr_fmt (demo_broker->host, "%s:%s", address, port);
            }
            else
                smt_log_print (demo_broker->alert_log,
                    "E: cannot translate host name '%s'", hostname);

            apr_pool_destroy (pool);
            tcb->master_socket = smt_socket_new ();
            if (tcb->master_socket) {
                //  Set Nagle's algorithm on or off
                rc = smt_socket_set_nodelay (tcb->master_socket,
                    demo_server_config_tcp_nodelay (demo_server_config));
                if (rc)
                    smt_log_print (demo_broker->alert_log,
                        "W: could not set TCP/IP NODELAY option: "
                        "Socket Error");

                //  Report and set socket buffers if wanted
                new_size = demo_server_config_tcp_rcvbuf (demo_server_config);
                if (new_size) {
                    rc = smt_socket_set_rcvbuf (tcb->master_socket, new_size, &old_size);
                    if (!rcv_warned) {
                        rcv_warned = TRUE;
                        if (rc)
                            smt_log_print (demo_broker->alert_log,
                                "W: could not set TCP/IP receive buffer size: "
                                "Socket Error");
                        else
                            smt_log_print (demo_broker->alert_log,
                                "I: TCP/IP receive buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                new_size = demo_server_config_tcp_sndbuf (demo_server_config);
                if (new_size) {
                    rc = smt_socket_set_sndbuf (tcb->master_socket, new_size, &old_size);
                    if (!snd_warned) {
                        snd_warned = TRUE;
                        if (rc)
                            smt_log_print (demo_broker->alert_log,
                                "W: could not set TCP/IP send buffer size: "
                                "Socket Error");
                        else
                            smt_log_print (demo_broker->alert_log,
                                "I: TCP/IP sending buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                //  Start listening for connections on socket
                rc = smt_socket_passive (tcb->master_socket, thread, port, hostname, 500);

                //  Try to drop to safe user/group, complain if not possible
                if (*user && *group) {
                    if (ipr_uid_run_as (user, group)) {
                        smt_log_print (demo_broker->alert_log,
                            "E: could not run as user=%s group=%s", user, group);
                        exit (1);
                    }
                    else
                        smt_log_print (demo_broker->alert_log,
                            "I: server is now running as user=%s group=%s", user, group);
                }
            }
            if (!tcb->master_socket || (rc != SMT_OK)) {
                smt_log_print (demo_broker->alert_log,
                    "E: could not open port %s: %s", port,
                    smt_thread_error (thread));
                exit (1);               //  FIXME: see BASE2-289
            }
        }
    </action>

    <action name = "allow thread to settle">
        smt_timer_request_delay (thread, 1 * 1000 * 1000, SMT_NULL_EVENT);
    </action>

    <action name = "report server ready">
        smt_log_print (demo_broker->alert_log,
            "I: server ready for incoming DEMO connections");
        if (demo_server_config_direct (demo_server_config))
            smt_log_print (demo_broker->alert_log,
                "I: Direct Mode enabled for all client applications by default");
        smt_thread_set_next_event (thread, ready_event);
    </action>

   <!--  Accepting Connections
         Accept new connections and create connection threads               -->

    <state name = "accepting connections">
        <event name = "ready">
            <action name = "accept connection" />
        </event>
        <event name = "new connection">
            <action name = "create connection thread" />
            <action name = "accept connection" />
        </event>
        <event name = "socket error">
            <action name = "report accept error" />
            <action name = "accept connection" />
        </event>
    </state>

    <action name = "accept connection">
        smt_socket_request_accept (thread, tcb->master_socket, 0,
            new_connection_event, &tcb->client_socket);
    </action>

    <action name = "create connection thread">
        smt_thread_t
            *connection_thread;
        demo_server_agent_connection_context_t
            *connection_tcb;

        connection_thread = demo_server_agent_connection_thread_new ();
        connection_thread->priority = SMT_PRIORITY_HIGH;

        connection_tcb                    = connection_thread->context;
        connection_tcb->socket            = tcb->client_socket;
        connection_tcb->connection_thread = connection_thread;
        connection_tcb->input_thread      = NULL;
        connection_tcb->heartbeat_thread  = NULL;
        tcb->client_socket                = NULL;

        smt_thread_wake   ( connection_thread);
        smt_thread_unlink (&connection_thread);
    </action>

    <action name = "report accept error">
        int
            accept_retry_timeout;

        smt_log_print (demo_broker->alert_log,
            "E: could not accept connection: %s",
            smt_thread_error (thread));

        //  If we ran out of files wait the configured amount of time and retry
        if (thread->result == SMT_SOCKET_ERROR &&
           (APR_STATUS_IS_ENFILE(thread->error) ||
            APR_STATUS_IS_EMFILE(thread->error))) {
            accept_retry_timeout =
                demo_server_config_accept_retry_timeout (demo_server_config);
            smt_log_print (demo_broker->alert_log,
                "E: waiting %d seconds before accepting new connections",
                accept_retry_timeout);
            smt_timer_request_delay (thread, accept_retry_timeout * 1000 * 1000,
                SMT_NULL_EVENT);
        }
        //  Otherwise, some other error happened, assume it's bad
        else {
            abort ();
        }
    </action>

    <!--  Defaults for master thread                                        -->

    <state name = "defaults">
        <event name = "smt error" nextstate = "">
            <action name = "report smt error" />
        </event>
        <method name = "shutdown" nextstate = "">
            <action name = "close master socket" />
        </method>
    </state>

    <action name = "report smt error">
        smt_log_print (demo_broker->alert_log,
            "E: SMT error: %s", smt_thread_error (thread));
    </action>

    <action name = "close master socket">
        if (tcb->master_socket)
            smt_socket_request_close (thread, &tcb->master_socket, 0, SMT_NULL_EVENT);
    </action>

</thread>

<!---------------------------------------------------------------------------->
<!--  Connection context                                                    -->

<context name = "connection">
    smt_thread_t
        *connection_thread,             //  Master thread
        *input_thread,                  //  Socket input thread
        *heartbeat_thread;              //  Heartbeat monitor thread
    smt_socket_t
        *socket;                        //  Socket for i/o
    demo_server_connection_t
        *connection;                    //  Connection object
    demo_server_channel_t
        *channel;                       //  Channel object
    int
        channel_state;                  //  Internal channel state
    demo_server_method_t
        *method_in,                     //  Last received method
        *method_out;                    //  Last sent method
    asl_reader_t
        reader;                         //  Body reader
    ipr_bucket_t
        *bucket_in;                     //  Last received bucket
    icl_shortstr_t
        strerror;                       //  Reply text for method errors
    byte
        protocol_header [8];            //  Used to initiate connection
    asl_frame_header_t
        frame_header;                   //  Header for frame i/o
    dbyte
        out_class_id,                   //  Last class we sent
        out_method_id;                  //  Last method we sent
    qbyte
        frame_max,                      //  Negotiated maximum frame size
        heartbeat;                      //  Connection heartbeat interval
    int
        liveness,                       //  If this reaches zero, peer is dead
        trace;                          //  Trace level
    smt_timer_request_t
        *setup_timer;                   //  Connection setup timer

    //  Direct Protocol handling
    int
        type;                           //  Direct protocol type
    byte
        rlength;                        //  Request length from socket
    icl_shortstr_t
        request;                        //  Request string
    demo_lease_t
        *lease;                         //  Lease to sink or feed
    size_t
        batching;                       //  Configured batch size
    int
        direct_heartbeat;               //  Direct heartbeat, seconds
</context>

<!---------------------------------------------------------------------------->
<!--  Connection thread                                                     -->

<thread
    name = "connection"
    schedule = "poll"
    context = "connection"
    >
    <handler name = "thread init">
        apr_sockaddr_t
            *sockaddr;                  //  Address of peer
        char
            *address_ip;                //  IP address of peer
        icl_shortstr_t
            address_full;               //  Formatted address

        //  Create demo_server_connection object
        tcb->connection = demo_server_connection_new (
            demo_broker->connections, thread);
        demo_server_connection_list_push (
            demo_broker->connection_list,
            tcb->connection);

        tcb->channel_state = ASL_CHANNEL_CLOSED;
        tcb->liveness      = FULL_LIVENESS;
        tcb->frame_max     = tcb->connection->frame_max;
        tcb->heartbeat     = tcb->connection->heartbeat;
        tcb->trace         = tcb->connection->trace;
        tcb->direct_heartbeat = 5;

        //  Set peer address for reporting
        apr_socket_addr_get (&sockaddr, APR_REMOTE,
            tcb->socket->pollfd.desc.s);
        apr_sockaddr_ip_get (&address_ip, sockaddr);
        icl_shortstr_fmt (address_full, "%s:%u", address_ip, sockaddr->port);
        demo_server_connection_set_address (tcb->connection, address_full);

        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        smt_thread_set_next_event (thread, ok_event);
    </handler>

    <handler name = "thread destroy">
        tcb->connection_thread = NULL;

        //  Destroy demo_server_connection object
        demo_server_connection_destroy (&tcb->connection);

        //  Free resources
        demo_lease_unlink (&tcb->lease);
        demo_server_method_unlink (&tcb->method_in);
        demo_server_method_unlink (&tcb->method_out);
        demo_server_channel_destroy (&tcb->channel);
        ipr_bucket_unlink (&tcb->bucket_in);
        smt_socket_destroy (&tcb->socket);
    </handler>

    <!--  Initialise Connection                                             -->

    <state name = "initialise connection">
        <event name = "ok">
            <action name = "start setup timer" />
            <action name = "read protocol header" />
        </event>
        <event name = "have protocol header">
            <action name = "check protocol header" />
            <action name = "start input thread" />
            <action name = "send connection start" />
        </event>
        <!-- We have a Direct Protocol connection, check lease -->
        <event name = "direct protocol" nextstate = "direct connection">
            <action name = "accept direct connection" />
            <action name = "read direct request" />
            <action name = "check direct lease" />
        </event>
        <event name = "bad lease" nextstate = "">
            <action name = "flush the connection" />
        </event>
        <method name = "connection start ok">
            <action name = "send connection tune" />
        </method>
        <method name = "connection tune ok">
            <action name = "start heartbeat thread" />
            <action name = "start heartbeat timer" />
        </method>
        <method name = "connection open" nextstate = "connection active">
            <action name = "send connection open ok" />
        </method>
        <event name = "bad protocol" nextstate = "">
            <action name = "write desired header" />
            <action name = "flush the connection" />
            <action name = "report bad protocol" />
        </event>
        <event name = "version mismatch" nextstate = "">
            <action name = "write desired header" />
            <action name = "flush the connection" />
            <action name = "report version mismatch" />
        </event>
        <method name = "connection error" nextstate = "">
            <call state = "close the connection" event = "active close" />
        </method>
        <event name = "setup timeout" nextstate = "">
            <action name = "shut down slave threads" />
            <action name = "report setup timeout" />
        </event>
        <method name = "shutdown" nextstate = "">
            <action name = "shut down slave threads" />
        </method>
    </state>

    <action name = "start setup timer">
        //  Schedule a timeout event to protect against misbehaving clients
        tcb->setup_timer = smt_timer_request_delay (thread,
            demo_server_config_setup_timeout (demo_server_config) * 1000 * 1000,
            setup_timeout_event);
    </action>

    <action name = "read protocol header">
        assert (strlen ("DEMO") == 4);
        smt_socket_request_read (thread, tcb->socket, 0, 8, 8,
            tcb->protocol_header, have_protocol_header_event);
    </action>

    <action name = "check protocol header">
        //  Check the first 4 bytes are "DEMO"
        if (memcmp (tcb->protocol_header, "DEMO", 4) != 0)
            smt_thread_raise_exception (thread, bad_protocol_event);
        else
        //  Next we check the protocol number & version
        if (memcmp (tcb->protocol_header + 4, DP_HEADER, 4) == 0)
            smt_thread_raise_exception (thread, direct_protocol_event);
        else
        if (memcmp (tcb->protocol_header + 4, "\x00\x00\x09\x01", 4)
        &&  memcmp (tcb->protocol_header + 4, "\x01\x01\x00\x09", 4))
            smt_thread_raise_exception (thread, version_mismatch_event);
    </action>

    <action name = "write desired header">
        //  Write our own protocol header to the socket
        s_socket_write (thread, (byte *) "DEMO\x00\x00\x09\x01", 8);
    </action>

    <action name = "accept direct connection">
        tcb->batching = demo_server_config_batching (demo_server_config);
        if (tcb->batching < 2048)
            tcb->batching = 0;
        s_socket_write_shortstr (thread, "200 OK DP/0.1");
        s_socket_read (thread, &tcb->rlength, 1);
    </action>

    <action name = "read direct request">
        s_socket_read (thread, (byte *) tcb->request, (int) (tcb->rlength));
    </action>

    <action name = "check direct lease">
        icl_shortstr_t
            response;

        tcb->request [tcb->rlength] = 0;
        tcb->lease = demo_lease_search (tcb->request);
        if (tcb->lease) {
            tcb->type = tcb->lease->type;
            if (demo_lease_acquire (tcb->lease, thread, tcb->socket)) {
                s_socket_write_shortstr (thread, "402 BAD-LEASE Lease already acquired");
                demo_lease_unlink (&tcb->lease);
                smt_thread_raise_exception (thread, bad_lease_event);
            }
            else
            if (tcb->type == DP_SINK) {
                //  Confirm OK to client
                icl_shortstr_fmt (response, "200 OK Sink '%s' ready", tcb->lease->sink->name);
                s_socket_write_shortstr (thread, response);

                //  Slurp a bucket of data from the network
                //  Min. size for a packed 6-DMP content is 12 bytes
                //  We always read as much as we can, limited by ipr_bucket
                tcb->bucket_in = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
                smt_socket_request_read (thread, tcb->socket, 0,
                    12, tcb->bucket_in->max_size, tcb->bucket_in->data,
                    direct_in_event);
            }
            else
            if (tcb->type == DP_FEED) {
                //  Confirm OK to client
                icl_shortstr_fmt (response, "200 OK Feed '%s' ready", tcb->lease->feed->name);
                s_socket_write_shortstr (thread, response);
                //  Start feed heartbeat
                smt_timer_request_delay (thread, tcb->direct_heartbeat * 1000 * 1000, heartbeat_event);
            }
        }
        else {
            s_socket_write_shortstr (thread, "402 BAD-LEASE Lease is not valid for this connection");
            smt_thread_raise_exception (thread, bad_lease_event);
        }
    </action>

    <action name = "report bad protocol">
        smt_log_print (demo_broker->alert_log,
            "E: client at '%s' sent invalid protocol header",
            tcb->connection->client_address);
    </action>

    <action name = "report version mismatch">
        smt_log_print (demo_broker->alert_log,
            "E: version mismatch: client at '%s' sent (%u, %u, %u, %u)",
            tcb->connection->client_address,
            tcb->protocol_header [4],
            tcb->protocol_header [5],
            tcb->protocol_header [6],
            tcb->protocol_header [7]);
    </action>

    <action name = "start input thread">
        smt_thread_t
            *input_thread;

        //  Start input thread
        input_thread = demo_server_agent_input_thread_new (thread);

        tcb->input_thread = input_thread;

        //  This thread needs to watch the input thread so that it
        //  doesn't get swamped by incoming methods between socket polls.
        smt_thread_watch (thread, input_thread);

        smt_thread_wake   ( input_thread);
        smt_thread_unlink (&input_thread);
    </action>

    <action name = "send connection start">
        icl_longstr_t
            *properties,                //  Connection properties
            *mechanisms,                //  Supported authentication mechanisms
            *locales;                   //  Supported locales
        asl_field_list_t
            *field_list;                //  List for connection properties
        icl_shortstr_t
            system;                     //  System (platform) name

        field_list = asl_field_list_new (NULL);
        asl_field_new_string (field_list, "host", demo_broker->host);
        asl_field_new_string (field_list, "product", "ASL Demo Server");
        asl_field_new_string (field_list, "version", VERSION);
        asl_field_new_string (field_list, "copyright", COPYRIGHT);
        asl_field_new_string (field_list, "platform", icl_system_name (system));
        asl_field_new_string (field_list, "information", BUILDMODEL);
        asl_field_new_string (field_list, "connection", tcb->connection->id);
        asl_field_new_string (field_list, "identifier", demo_broker->identifier);
        asl_field_new_integer (field_list, "direct", demo_server_config_direct (demo_server_config));

        properties = asl_field_list_flatten (field_list);
        asl_field_list_destroy (&field_list);

        mechanisms = icl_longstr_new ("PLAIN", 5);
        locales    = icl_longstr_new ("en_US", 5);

        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_start (
            0, 9, properties, mechanisms,
            locales);
        s_send_method_out (thread, 0, demo_broker->debug_log);

        icl_longstr_destroy (&properties);
        icl_longstr_destroy (&mechanisms);
        icl_longstr_destroy (&locales);
    </action>

    <action name = "send connection tune">
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_tune (
            tcb->connection->channel_max,
            tcb->connection->frame_max,
            tcb->connection->heartbeat);
        s_send_method_out (thread, 0, demo_broker->debug_log);
    </action>

    <action name = "start heartbeat thread">
        smt_thread_t
            *heartbeat_thread;

        //  If client is heartbeat enabled, start heartbeat thread
        if (tcb->heartbeat) {
            heartbeat_thread = demo_server_agent_heartbeat_thread_new (thread);
            tcb->heartbeat_thread = heartbeat_thread;
            smt_thread_wake   ( heartbeat_thread);
            smt_thread_unlink (&heartbeat_thread);
        }
    </action>

    <action name = "start heartbeat timer">
        //  If client is heartbeat enabled, (re)start outgoing heartbeat timer
        if (tcb->heartbeat)
            smt_timer_request_delay (thread, tcb->heartbeat * 1000 * 1000,
                heartbeat_event);
    </action>

    <action name = "report setup timeout">
        smt_log_print (demo_broker->alert_log,
            "E: client timed out during connection setup, closing connection "  "(%s)", tcb->connection->client_address);
    </action>

    <!--  Connection Active                                                 -->
    <!--  Process any outgoing protocol methods.                            -->

    <state name = "connection active">
        <method name = "channel flow">
            <action name = "send channel flow" />
        </method>
        <method name = "channel flow-ok">
            <action name = "send channel flow-ok" />
        </method>
        <method name = "exchange declare-ok">
            <action name = "send exchange declare-ok" />
        </method>
        <method name = "exchange delete-ok">
            <action name = "send exchange delete-ok" />
        </method>
        <method name = "queue declare-ok">
            <action name = "send queue declare-ok" />
        </method>
        <method name = "queue bind-ok">
            <action name = "send queue bind-ok" />
        </method>
        <method name = "queue purge-ok">
            <action name = "send queue purge-ok" />
        </method>
        <method name = "queue delete-ok">
            <action name = "send queue delete-ok" />
        </method>
        <method name = "basic consume-ok">
            <action name = "send basic consume-ok" />
        </method>
        <method name = "basic cancel-ok">
            <action name = "send basic cancel-ok" />
        </method>
        <method name = "basic deliver">
            <action name = "send basic deliver" />
            <call state = "send content body if any" event = "start" />
        </method>
        <method name = "basic get-ok">
            <action name = "send basic get-ok" />
            <call state = "send content body if any" event = "start" />
        </method>
        <method name = "basic get-empty">
            <action name = "send basic get-empty" />
        </method>
        <method name = "direct put-ok">
            <action name = "send direct put-ok" />
        </method>
        <method name = "direct get-ok">
            <action name = "send direct get-ok" />
        </method>
        <method name = "push">
            <action name = "push method to client" />
            <call state = "send content body if any" event = "start" />
        </method>
        <!--  Channel close initiated by us due to error -->
        <method name = "channel error">
            <action name = "send channel close" />
        </method>
        <!--  Channel open initated by peer -->
        <method name = "send channel open ok">
            <action name = "send channel open ok" />
        </method>
        <!--  Channel close initiated by peer -->
        <method name = "send channel close ok">
            <action name = "send channel close ok" />
        </method>
        <!--  Connection close initated by us due to error -->
        <method name = "connection error">
            s_connection_exception (thread,
                connection_error_m->reply_code,
                connection_error_m->reply_text,
                connection_error_m->faulting_class_id,
                connection_error_m->faulting_method_id);
        </method>
        <!--  Called by master thread during server shutdown -->
        <!--  We raise an exception in the server connection object so that
              the Input thread picks up on this at the next frame boundary. -->
        <method name = "shutdown" nextstate = "">
            demo_server_connection_raise_exception (tcb->connection,
                100, "Operator requested server shutdown", 0, 0);
            <call state = "close the connection" event = "active close" />
        </method>
        <!--  Called by console -->
        <method name = "kill connection">
            s_connection_exception (thread,
                ASL_CONNECTION_FORCED,
                "Operator killed connection explicitly", 0, 0);
        </method>
        <!--  Triggered when an attempt to send a method
              on a OPENING/CLOSED/CLOSING channel is made. -->
        <event name = "discard method" nextstate = "connection active">
        </event>
    </state>

    <action name = "send channel flow">
        //  Check channel is OPEN
        assert (channel_flow_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_channel_flow (
                    channel_flow_m->active);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send channel flow-ok">
        //  Check channel is OPEN
        assert (channel_flow_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_channel_flow_ok (
                    channel_flow_ok_m->active);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send exchange declare-ok">
        //  Check channel is OPEN
        assert (exchange_declare_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_exchange_declare_ok (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send exchange delete-ok">
        //  Check channel is OPEN
        assert (exchange_delete_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_exchange_delete_ok (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send queue declare-ok">
        //  Check channel is OPEN
        assert (queue_declare_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_declare_ok (
                    queue_declare_ok_m->queue,
                    queue_declare_ok_m->properties);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send queue bind-ok">
        //  Check channel is OPEN
        assert (queue_bind_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_bind_ok (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send queue purge-ok">
        //  Check channel is OPEN
        assert (queue_purge_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_purge_ok (
                    queue_purge_ok_m->message_count);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send queue delete-ok">
        //  Check channel is OPEN
        assert (queue_delete_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_queue_delete_ok (
                    queue_delete_ok_m->message_count);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send basic consume-ok">
        //  Check channel is OPEN
        assert (basic_consume_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_consume_ok (
                    basic_consume_ok_m->consumer_tag);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send basic cancel-ok">
        //  Check channel is OPEN
        assert (basic_cancel_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_cancel_ok (
                    basic_cancel_ok_m->consumer_tag);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send basic deliver">
        //  Check channel is OPEN
        assert (basic_deliver_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_deliver (
                    basic_deliver_m->exchange,
                    basic_deliver_m->routing_key,
                    basic_deliver_m->delivery_tag);
            tcb->method_out->content =
                demo_content_basic_link (
                    basic_deliver_m->content
                );
            assert (tcb->method_out->content);
            tcb->connection->contents_out++;
            ipr_meter_count (demo_broker->ometer);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send basic get-ok">
        //  Check channel is OPEN
        assert (basic_get_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_get_ok (
                    basic_get_ok_m->exchange,
                    basic_get_ok_m->routing_key,
                    basic_get_ok_m->message_count);
            tcb->method_out->content =
                demo_content_basic_link (
                    basic_get_ok_m->content
                );
            assert (tcb->method_out->content);
            tcb->connection->contents_out++;
            ipr_meter_count (demo_broker->ometer);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send basic get-empty">
        //  Check channel is OPEN
        assert (basic_get_empty_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_basic_get_empty (
                );
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send direct put-ok">
        //  Check channel is OPEN
        assert (direct_put_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_direct_put_ok (
                    direct_put_ok_m->lease);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send direct get-ok">
        //  Check channel is OPEN
        assert (direct_get_ok_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out =
                demo_server_method_new_direct_get_ok (
                    direct_get_ok_m->lease);
            s_send_method_out (thread, 1,
                demo_broker->debug_log);

            //  Force immediate release of SMT method
            smt_method_destroy (&thread->method);
        }
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send connection open ok">
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_open_ok (NULL);
        s_send_method_out (thread, 0, demo_broker->debug_log);
    </action>

    <!--  Passive channel open -->

    <action name = "send channel open ok">
        icl_longstr_t
            *channel_id;                //  Unique channel ID sent to client

        //  Assert channel is OPENING
        assert (tcb->channel_state == ASL_CHANNEL_OPENING);
        //  We only support a single channel, so just send the same ID each time
        channel_id = icl_longstr_new ("0-1#", 4);
        //  Send the channel open ok
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out =
            demo_server_method_new_channel_open_ok (channel_id);
        s_send_method_out (thread, 1, demo_broker->debug_log);
        icl_longstr_destroy (&channel_id);
        //  Channel is now OPEN
        tcb->channel_state = ASL_CHANNEL_OPEN;
    </action>

    <!--  Passive channel close -->

    <action name = "send channel close ok">
        //  Assert channel is CLOSING
        assert (tcb->channel_state == ASL_CHANNEL_CLOSING);
        //  Send the channel close ok
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_channel_close_ok ();
        s_send_method_out (thread, 1, demo_broker->debug_log);
        //  Destroy the channel object
        demo_server_channel_destroy (&tcb->channel);
        //  Channel is now CLOSED
        tcb->channel_state = ASL_CHANNEL_CLOSED;
    </action>

    <action name = "push method to client">
        //  Check channel is OPEN
        assert (push_m->channel_nbr == 1);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Send method as-is, with no reconstruction
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out = demo_server_method_link (push_m->method);
            s_send_method_out (thread, 1, demo_broker->debug_log);
        }
        else
            //  Channel is not OPEN, discard method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <action name = "send channel close">
        //  Assert channel is OPEN
        assert (channel_error_m->channel == tcb->channel);
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Report the error
            s_report_error (thread, "Channel", "closing channel",
                channel_error_m->reply_code,
                channel_error_m->reply_text,
                channel_error_m->faulting_class_id,
                channel_error_m->faulting_method_id);
            //  Send the method
            demo_server_method_unlink (&tcb->method_out);
            tcb->method_out = demo_server_method_new_channel_close (
                channel_error_m->reply_code,
                channel_error_m->reply_text,
                channel_error_m->faulting_class_id,
                channel_error_m->faulting_method_id);
            s_send_method_out (thread, 1, demo_broker->debug_log);
            //  Channel is now CLOSING
            tcb->channel_state = ASL_CHANNEL_CLOSING;
        }
        else
            //  Channel is not OPEN
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE);
    </action>

    <!--  Send Content Body If Any                                          -->
    <!--  Sends a content header, followed by a number of body frames       -->

    <state name = "send content body if any">
        <event name = "start">
            <action name = "send content header" />
            <action name = "send content body frame" />
        </event>
        <event name = "continue">
            <action name = "send content body frame" />
        </event>
        <event name = "finished">
            <return />
        </event>
    </state>

    <action name = "send content header">
        ipr_bucket_t
            *payload = NULL;            //  Bucket ready to send

        if (tcb->method_out->content) {
            //  Chronometer on content sent to wire
            demo_content_basic_chrono_add (tcb->method_out->content);
            if (tcb->method_out->class_id == DEMO_SERVER_BASIC) {
                payload = demo_content_basic_replay_header (
                    tcb->method_out->content);
                s_write_payload (thread, 1, ASL_FRAME_HEADER, &payload);
                demo_content_basic_set_reader (
                    tcb->method_out->content, &tcb->reader, tcb->frame_max);
            }
        }
        else
            smt_thread_raise_exception (thread, finished_event);
    </action>

    <action name = "send content body frame">
        ipr_bucket_t
            *payload = NULL;            //  Bucket ready to send

        if (tcb->method_out->class_id == DEMO_SERVER_BASIC)
            payload = demo_content_basic_replay_body (
                tcb->method_out->content, &tcb->reader);
        if (payload) {
            s_write_payload (thread, 1, ASL_FRAME_BODY, &payload);
            smt_thread_set_next_event (thread, continue_event);
        }
        else {
            demo_server_channel_earn (tcb->channel);
            smt_thread_set_next_event (thread, finished_event);
        }
    </action>

    <!--  Defaults for connection thread                                    -->

    <state name = "defaults">
        <!--  Exception events -->
        <event name = "socket error" nextstate = "">
            <action name = "shut down slave threads" />
            <action name = "report socket error" />
        </event>
        <event name = "socket timeout" nextstate = "">
            <action name = "shut down slave threads" />
            <action name = "report socket timeout" />
        </event>
        <event name = "socket closing" nextstate = "">
            <action name = "shut down slave threads" />
            <action name = "report socket error" />
        </event>
        <event name = "smt error" nextstate = "">
            <action name = "shut down slave threads" />
            <action name = "report smt error" />
        </event>
        <!--  Passive close on input -->
        <event name = "connection close" nextstate = "">
            <call state = "close the connection" event = "passive close" />
        </event>
        <!--  Recoverable protocol/internal error on the connection -->
        <event name = "connection error" nextstate = "">
            <call state = "close the connection" event = "active close" />
        </event>
        <!--  Unrecoverable error on the connection -->
        <event name = "connection abort" nextstate = "">
            <action name = "shut down slave threads" />
        </event>
        <!--  Notification of exception from input or heartbeat thread -->
        <event name = "external exception" nextstate = "">
            <action name = "shut down slave threads" />
        </event>
        <!--  Event from heartbeat timer -->
        <event name = "heartbeat">
            <action name = "send heartbeat frame" />
            <action name = "start heartbeat timer" />
        </event>
        <!--  Late setup timer, ignore it -->
        <event name = "setup timeout">
            <!-- Do nothing -->
        </event>
        <!--  External events which can be handled in any state -->
        <method name = "set trace">
            tcb->trace                      = set_trace_m->trace;
            tcb->input_thread->animate      =
            tcb->heartbeat_thread->animate  =
            tcb->connection_thread->animate = (tcb->trace >= ASL_TRACE_MED);
            smt_socket_trace (tcb->socket, (tcb->trace >= ASL_TRACE_HIGH));
        </method>
    </state>

    <action name = "shut down slave threads">
        //  Shut down other two threads if they are still active.
        if (tcb->input_thread)
            smt_thread_shut_down (tcb->input_thread);
        if (tcb->heartbeat_thread)
            smt_thread_shut_down (tcb->heartbeat_thread);
    </action>

    <!--  Close The Connection                                              -->
    <!--  Called when closing the connection.
          For an active close we send a connection close and then discard
          anything until we get a corresponding connection close.ok.  For a
          passive close (initated by peer), we process the close, send a
          connection close.ok and shut down.                                -->

    <state name = "close the connection">
        <event name = "active close">
            <action name = "send connection close" />
        </event>
        <method name = "connection close ok">
            <action name = "shut down slave threads" />
            <action name = "flush the connection" />
            <return />
        </method>
        <event name = "passive close">
            <action name = "shut down slave threads" />
            <action name = "execute connection close" />
            <action name = "send connection close ok" />
            <action name = "flush the connection" />
            <return />
        </event>
        <event name = "close timeout">
            <action name = "shut down slave threads" />
            smt_log_print (demo_broker->alert_log,
                "E: timed out waiting for connection.close-ok from client"   " (%s, %s, %s, %s)",
                tcb->connection->client_address,
                tcb->connection->client_product,
                tcb->connection->client_version,
                tcb->connection->client_instance);
            <return />
        </event>
        <!--  Neither of the following two exceptions should occur under normal
              conditions in this state, but if we do get them then report
              and shut down immediately.                                    -->
        <event name = "connection error">
            <action name = "shut down slave threads" />
            smt_log_print (demo_broker->alert_log,
                "E: connection error while "   "waiting for connection.close-ok from client"   " (%s, %s, %s, %s)",
                tcb->connection->client_address,
                tcb->connection->client_product,
                tcb->connection->client_version,
                tcb->connection->client_instance);
            <return />
        </event>
        <event name = "connection close">
            <action name = "shut down slave threads" />
            smt_log_print (demo_broker->alert_log,
                "E: received connection close while "   "waiting for connection.close-ok from client"   " (%s, %s, %s, %s)",
                tcb->connection->client_address,
                tcb->connection->client_product,
                tcb->connection->client_version,
                tcb->connection->client_instance);
            <return />
        </event>
        <!--  Ignore any unhandled methods or events -->
        <default>
        </default>
    </state>

    <action name = "send connection close">
        //  Schedule a timeout event to protect against misbehaving clients
        smt_timer_request_delay (thread,
            demo_server_config_close_timeout (demo_server_config) * 1000 * 1000,
            close_timeout_event);
        //  Send connection close
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_close (
            tcb->connection->reply_code,
            tcb->connection->reply_text,
            tcb->connection->faulting_class_id,
            tcb->connection->faulting_method_id);
        s_send_method_out (thread, 0, demo_broker->debug_log);
    </action>

    <!--  We can't use s_execute_method here since that might pick up an
          async exception, and we already know it's a connection close -->
    <action name = "execute connection close">
        demo_server_method_execute (tcb->method_in, tcb->connection,
            NULL);
    </action>

    <action name = "send connection close ok">
        demo_server_method_unlink (&tcb->method_out);
        tcb->method_out = demo_server_method_new_connection_close_ok ();
        s_send_method_out (thread, 0, demo_broker->debug_log);
    </action>

    <action name = "flush the connection">
        if (tcb->socket)
            smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
    </action>

    <action name = "report socket error">
        smt_log_print (demo_broker->alert_log,
            "E: %s, connection to client lost "  "(%s, %s, %s, %s)",
            smt_thread_error (thread),
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    </action>

    <action name = "report socket timeout">
        smt_log_print (demo_broker->alert_log,
            "E: socket timeout, connection to client lost "  "(%s, %s, %s, %s)",
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    </action>

    <action name = "send heartbeat frame">
        ipr_bucket_t
           *bucket;                     //  Header bucket
        byte
            *bucket_ptr;

        bucket = ipr_bucket_new (ASL_HEADER_SIZE + 1);
        bucket_ptr = bucket->data;
        PUT_OCTET (bucket_ptr, ASL_FRAME_HEARTBEAT);
        PUT_SHORT (bucket_ptr, 0);
        PUT_LONG  (bucket_ptr, 0);
        PUT_OCTET (bucket_ptr, ASL_FRAME_END);
        bucket->cur_size = bucket_ptr - bucket->data;
        s_socket_write_bucket (thread, bucket);
        ipr_bucket_unlink (&bucket);
    </action>

    <!--  Direct Connection                                                 -->
    <state name = "direct connection">
        <event name = "direct in">
            <action name = "deliver direct contents" />
        </event>
        <method name = "direct out">
            <action name = "write direct content" />
        </method>
        <method name = "shutdown" nextstate = "">
            <action name = "flush the connection" />
        </method>
        <event name = "heartbeat">
            <action name = "write direct heartbeat" />
        </event>
        <event name = "socket closing" nextstate = "">
            <action name = "flush the connection" />
        </event>
    </state>

    <action name = "deliver direct contents">
        demo_content_basic_t
            *content;
        size_t
            pending,
            offset;
        byte
            options;                    //  Publish options octet

        //  Update bucket with amount read, check if complete
        tcb->bucket_in->cur_size += tcb->socket->io_size;
        pending = demo_content_basic_wire_pending (tcb->bucket_in);
        if (pending)
            smt_socket_request_read (
                thread, tcb->socket, 0,
                pending, pending, tcb->bucket_in->data + tcb->bucket_in->cur_size,
                direct_in_event);
        else {
            //  Process each content in the bucket
            offset = 0;
            tcb->lease->connection->traffic_in += tcb->bucket_in->cur_size;
            content = demo_content_basic_wire_get (tcb->bucket_in, &offset, &options);
            while (content) {
                if (tcb->trace >= ASL_TRACE_LOW) {
                    smt_log_print (demo_broker->debug_log,
                        "I: direct to sink '%s'", tcb->lease->sink->name);
                    icl_console_print ("I: direct to sink '%s'", tcb->lease->sink->name);
                }
                //  Chronometer on content received from wire
                demo_content_basic_chrono_add (content);

                ipr_meter_count (demo_broker->imeter);
                demo_lease_sink (tcb->lease, content, options);
                demo_content_basic_unlink (&content);
                content = demo_content_basic_wire_get (tcb->bucket_in, &offset, &options);
                icl_atomic_inc32 ((volatile qbyte *) &(demo_broker->direct_in));
                tcb->lease->connection->contents_in++;
            }
            //  Reset bucket and get fresh contents data
            tcb->bucket_in->cur_size = 0;
            smt_socket_request_read (thread, tcb->socket, 0,
                12, tcb->bucket_in->max_size, tcb->bucket_in->data,
                direct_in_event);
        }
    </action>

    <action name = "write direct content">
        ipr_bucket_t
            *bucket;
        smt_method_t
            *method;
        int
            rc;
        Bool
            batch;                      //  Do we batch more messages or not?

        //  Chronometer on content sent to wire
        demo_content_basic_chrono_add (direct_out_m->content);
        if (tcb->batching) {
            //  We start by putting one message into a batch-sized bucket
            bucket = ipr_bucket_new (tcb->batching);
            rc = demo_content_basic_wire_put (
                direct_out_m->content, bucket, direct_out_m->options);
            if (rc == 0)
                batch = TRUE;           //  Success, so continue batching
            else {
                //  If the message is oversized, grab a max.sized bucket and try again
                batch = FALSE;
                ipr_bucket_unlink (&bucket);
                bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
                rc = demo_content_basic_wire_put (
                    direct_out_m->content, bucket, direct_out_m->options);
                assert (rc == 0);       //  We can't have a message bigger than this
            }
        }
        else {
            //  Batching has been disabled
            batch = FALSE;
            bucket = ipr_bucket_new (demo_content_basic_wire_size (direct_out_m->content));
            rc = demo_content_basic_wire_put (
                direct_out_m->content, bucket, direct_out_m->options);
            assert (rc == 0);
        }
        ipr_meter_count (demo_broker->ometer);
        icl_atomic_dec32 ((volatile qbyte *) &(tcb->lease->pending));
        icl_atomic_inc32 ((volatile qbyte *) &(demo_broker->direct_out));
        tcb->lease->connection->contents_out++;
        if (tcb->trace >= ASL_TRACE_LOW) {
            smt_log_print (demo_broker->debug_log,
                "I: direct from feed '%s'", tcb->lease->feed->name);
            icl_console_print ("I: direct from feed '%s'", tcb->lease->feed->name);
        }
        while (batch && rc == 0 && thread->reply_list->smt_method.list_next != &thread->reply_list->smt_method) {
            method = smt_method_list_pop (thread->reply_list);
            if (method->event == direct_out_m_event) {
                //  Chronometer on content sent to wire
                demo_content_basic_chrono_add (
                    ((demo_server_agent_direct_out_t *) method->data)->content);
                rc = demo_content_basic_wire_put (
                    ((demo_server_agent_direct_out_t *) method->data)->content, bucket, direct_out_m->options);
                if (rc == 0) {
                    ipr_meter_count (demo_broker->ometer);
                    icl_atomic_dec32 ((volatile qbyte *) &(tcb->lease->pending));
                    icl_atomic_inc32 ((volatile qbyte *) &(demo_broker->direct_out));
                    tcb->lease->connection->contents_out++;
                    if (tcb->trace >= ASL_TRACE_LOW) {
                        smt_log_print (demo_broker->debug_log,
                            "I: direct from feed '%s'", tcb->lease->feed->name);
                        icl_console_print ("I: direct from feed '%s'", tcb->lease->feed->name);
                    }
                    smt_method_unlink (&method);
                }
            }
            if (method) {               //  Not bundled, push back & stop
                smt_method_list_push (thread->reply_list, method);
                smt_method_unlink (&method);
                break;
            }
        }
        tcb->lease->connection->traffic_out += bucket->cur_size;
        smt_socket_request_write_bucket (
            thread, tcb->socket, 0, bucket, SMT_NULL_EVENT);
        ipr_bucket_unlink (&bucket);
    </action>

    <action name = "write direct heartbeat">
        static byte
            null_message [4] = { 0, 0, 0, 0 };

        //  Send a null message and restart the heartbeat timer
        smt_socket_request_write (thread, tcb->socket, 0, 4, null_message, SMT_NULL_EVENT);
        smt_timer_request_delay  (thread, tcb->direct_heartbeat * 1000 * 1000, heartbeat_event);
    </action>
</thread>

<!---------------------------------------------------------------------------->
<!--  Input thread                                                          -->

<thread
    name = "input"
    schedule = "poll"
    context = "connection"
    share = "1"
    >

    <handler name = "thread init">
        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        smt_thread_set_next_event (thread, ok_event);
    </handler>

    <handler name = "thread destroy">
        tcb->input_thread = NULL;
    </handler>

    <state name = "start">
        <event name = "ok" nextstate = "have incoming method">
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
    </state>

    <!--  Read Method                                                       -->
    <!--  Reads a single method frame from the socket, looping on heartbeat
          frames.  Raises an exception for any other frames.                -->

    <state name = "read method">
        <event name = "read method">
            <action name = "read frame header" />
            <action name = "read frame payload" />
            <action name = "check frame format" />
            <action name = "decode method" />
            <return />
        </event>
    </state>

    <action name = "read frame header">
        ipr_bucket_unlink (&tcb->bucket_in);
        s_socket_read_bucket (thread, &tcb->bucket_in, ASL_HEADER_SIZE);
    </action>

    <action name = "read frame payload">
        byte
            *bucket_ptr;
        icl_shortstr_t
            reply_text;

        bucket_ptr = tcb->bucket_in->data;
        tcb->frame_header.type    =  bucket_ptr [0];
        tcb->frame_header.channel = (bucket_ptr [1] << 8)  +  bucket_ptr [2];
        tcb->frame_header.size    = (bucket_ptr [3] << 24) + (bucket_ptr [4] << 16)
                                  + (bucket_ptr [5] << 8)  +  bucket_ptr [6];

        if (tcb->frame_header.size > tcb->frame_max) {
            icl_shortstr_fmt (reply_text, "Peer sent oversized frame (%d > %d)",
                tcb->frame_header.size, tcb->frame_max);
            s_connection_abort (thread, ASL_FRAME_ERROR, reply_text, 0, 0);
        }
        else {
            //  Read method payload plus frame-end
            ipr_bucket_unlink (&tcb->bucket_in);
            s_socket_read_bucket (thread, &tcb->bucket_in,
                tcb->frame_header.size + 1);
        }
        //  We have successfully read a frame header, reset peer liveness
        tcb->liveness = FULL_LIVENESS;
    </action>

    <action name = "check frame format">
        //  The bucket must be at least 1 octet long, and end with a frame-end
        tcb->bucket_in->cur_size = tcb->socket->io_size;
        if (tcb->bucket_in->cur_size > 0
        &&  tcb->bucket_in->data [tcb->bucket_in->cur_size - 1] == ASL_FRAME_END) {
            //  Strip off frame-end octet
            tcb->bucket_in->cur_size--;
        }
        else
            s_connection_abort (thread, ASL_SYNTAX_ERROR,
                tcb->bucket_in->cur_size?
                "Badly-formatted method frame - missing frame-end octet":
                "Badly-formatted method frame - zero-sized frame",
                0, 0);
    </action>

    <action name = "decode method">
        //  Loop if we received a heartbeat frame
        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, read_method_event);
        else
        //  Decode method
        if (tcb->frame_header.type == ASL_FRAME_METHOD) {
            demo_server_method_unlink (&tcb->method_in);
            tcb->method_in = demo_server_method_decode (tcb->bucket_in, tcb->strerror);
            if (tcb->method_in) {
                if (tcb->trace >= ASL_TRACE_LOW)
                    demo_server_method_dump (tcb->method_in,
                    demo_broker->debug_log, "I: recv ");
            }
            else
                s_connection_abort (thread,
                    ASL_SYNTAX_ERROR, tcb->strerror, 0, 0);
        }
        else
            s_connection_exception (thread,
                ASL_SYNTAX_ERROR, "Expected a method frame", 0, 0);
    </action>

    <!--  Have Incoming Method                                              -->
    <!--  Process an incoming method or content.                            -->

    <state name = "have incoming method">
        <event name = "connection class" nextstate = "have connection method">
            <action name = "check connection method" />
        </event>
        <event name = "channel class" nextstate = "have channel method">
            <action name = "check channel method" />
        </event>
        <event name = "basic content" nextstate = "have incoming method">
            <call state = "read basic content" event = "header" />
            <action name = "execute functional method" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <event name = "other method" nextstate = "have incoming method">
            <action name = "execute functional method" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <!--  Triggered if we receive a method on
              a channel which is not OPEN. -->
        <event name = "discard method" nextstate = "have incoming method">
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
    </state>

    <action name = "check method class">
        if (tcb->method_in->class_id == DEMO_SERVER_CONNECTION)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id == DEMO_SERVER_CHANNEL)
            smt_thread_set_next_event (thread, channel_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_SERVER_BASIC
        &&  tcb->method_in->method_id == DEMO_SERVER_BASIC_PUBLISH)
            smt_thread_set_next_event (thread, basic_content_event);
        else
            smt_thread_set_next_event (thread, other_method_event);
    </action>

    <action name = "check connection method">
        //  Check channel is valid
        if (tcb->frame_header.channel != 0)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_START_OK)
            smt_thread_set_next_event (thread, connection_start_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_TUNE_OK)
            smt_thread_set_next_event (thread, connection_tune_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_OPEN)
            smt_thread_set_next_event (thread, connection_open_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE)
            smt_thread_set_next_event (thread, connection_close_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_close_ok_event);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Method not allowed",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    </action>

    <action name = "check channel method">
        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_OPEN)
            smt_thread_set_next_event (thread, channel_open_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_CLOSE)
            smt_thread_set_next_event (thread, channel_close_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_CLOSE_OK)
            smt_thread_set_next_event (thread, channel_close_ok_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_FLOW)
            smt_thread_set_next_event (thread, channel_flow_event);
        else
        if (tcb->method_in->method_id == DEMO_SERVER_CHANNEL_FLOW_OK)
            smt_thread_set_next_event (thread, channel_flow_ok_event);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Method not allowed",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    </action>

    <action name = "execute functional method">
        //  Trap possible async exception before executing method
        if (demo_server_connection_exception_raised (tcb->connection))
            smt_thread_raise_exception (thread, async_connection_error_event);
        else
        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        //  Check channel is OPEN
        if (tcb->channel_state == ASL_CHANNEL_OPEN)
            s_execute_functional_method (thread,
                tcb->method_in->class_id, tcb->method_in->method_id,
                tcb->out_class_id, tcb->out_method_id);
        else
            //  Channel is not OPEN, discard the method
            smt_thread_raise_exception (thread, discard_method_event);
    </action>

    <!--  Have Connection Method                                            -->
    <!--  The only connection method we can receive on input is a connection
          close.  In this case, we notify the connection thread and let it
          deal with it.                                                     -->

    <state name = "have connection method">
        <event name = "connection start ok" nextstate = "have incoming method">
            <action name = "execute connection start ok" />
            <action name = "notify connection of start ok" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <event name = "connection tune ok" nextstate = "have incoming method">
            <action name = "execute connection tune ok" />
            <action name = "notify connection of tune ok" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <event name = "connection open" nextstate = "have incoming method">
            <action name = "execute connection open" />
            <action name = "notify connection of open" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <event name = "connection close" nextstate = "">
            <action name = "notify connection of passive close" />
        </event>
        <event name = "connection close ok" nextstate = "">
            <action name = "notify connection of close ok" />
        </event>
    </state>

    <action name = "execute connection start ok">
        s_execute_method (thread, FALSE,
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_START_OK);
    </action>

    <action name = "notify connection of start ok">
        demo_server_agent_connection_start_ok (tcb->connection_thread);
    </action>

    <action name = "execute connection tune ok">
        s_execute_method (thread, FALSE,
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_TUNE_OK);
        tcb->frame_max = tcb->connection->frame_max;
        tcb->heartbeat = tcb->connection->heartbeat;
    </action>

    <action name = "notify connection of tune ok">
        demo_server_agent_connection_tune_ok (tcb->connection_thread);
    </action>

    <action name = "execute connection open">
        s_execute_method (thread, FALSE,
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_OPEN);
    </action>

    <action name = "notify connection of open">
        demo_server_agent_connection_open (tcb->connection_thread);
    </action>

    <action name = "notify connection of passive close">
        //  Report error if any
        s_report_error (thread, "Connection", "closing connection",
            tcb->method_in->payload.connection_close.reply_code,
            tcb->method_in->payload.connection_close.reply_text,
            tcb->method_in->payload.connection_close.class_id,
            tcb->method_in->payload.connection_close.method_id);
        //  Notify connection thread
        smt_thread_raise_exception (tcb->connection_thread,
            connection_close_event);
        smt_thread_wake (tcb->connection_thread);
    </action>

    <action name = "notify connection of close ok">
        demo_server_agent_connection_close_ok (tcb->connection_thread);
    </action>

    <!--  Have Channel Method                                               -->

    <state name = "have channel method">
        <!--  Peer initiated channel open -->
        <event name = "channel open" nextstate = "have incoming method">
            <action name = "execute channel open" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <!--  Peer initiated channel close -->
        <event name = "channel close" nextstate = "have incoming method">
            <action name = "execute channel close" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <!--  Response to channel close initiated by us -->
        <event name = "channel close ok" nextstate = "have incoming method">
            <action name = "execute channel close ok" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <!--  Peer initiated channel flow -->
        <event name = "channel flow" nextstate = "have incoming method">
            <action name = "execute channel flow" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
        <!--  Response to channel flow initated by us -->
        <event name = "channel flow ok" nextstate = "have incoming method">
            <action name = "execute channel flow ok" />
            <call state = "read method" event = "read method" />
            <action name = "check method class" />
        </event>
    </state>

    <!--  Peer initiated channel open                                       -->

    <action name = "execute channel open">
        //  Check channel is CLOSED
        if (tcb->channel_state == ASL_CHANNEL_CLOSED) {
            //  Create the channel
            tcb->channel = demo_server_channel_new (
                tcb->connection->channels, tcb->connection,
                1);
            //  Channel is now OPENING
            tcb->channel_state = ASL_CHANNEL_OPENING;
            //  Execute the channel open
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_OPEN);
            //  Queue the channel open ok
            if (!smt_thread_exception_raised (thread))
                demo_server_agent_send_channel_open_ok (tcb->connection_thread);
        }
        else
            //  Channel is not CLOSED
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is already open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_OPEN);
    </action>

    <!--  Peer initated channel close                                       -->

    <action name = "execute channel close">
        //  Check channel is OPEN
        if (tcb->channel_state == ASL_CHANNEL_OPEN) {
            //  Report error if any
            s_report_error (thread, "Channel", "closing channel",
                tcb->method_in->payload.channel_close.reply_code,
                tcb->method_in->payload.channel_close.reply_text,
                tcb->method_in->payload.channel_close.class_id,
                tcb->method_in->payload.channel_close.method_id);
            //  Channel is now CLOSING
            tcb->channel_state = ASL_CHANNEL_CLOSING;
            //  Execute the channel close
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE);
            //  Queue the channel close ok
            if (!smt_thread_exception_raised (thread))
                demo_server_agent_send_channel_close_ok (tcb->connection_thread);
        }
        else
            //  Channel is not OPEN
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE);
    </action>

    <!--  Response to channel close initiated by us                         -->

    <action name = "execute channel close ok">
        //  Check channel is CLOSING
        if (tcb->channel_state == ASL_CHANNEL_CLOSING) {
            //  Destroy the channel object
            demo_server_channel_destroy (&tcb->channel);
            //  Channel is now CLOSED
            tcb->channel_state = ASL_CHANNEL_CLOSED;
        }
        else
            //  Channel is not CLOSING
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not closing",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_CLOSE_OK);
    </action>

    <action name = "execute channel flow">
        if (tcb->channel_state == ASL_CHANNEL_OPEN)
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW);
        else
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW);
    </action>

    <action name = "execute channel flow ok">
        if (tcb->channel_state == ASL_CHANNEL_OPEN)
            s_execute_method (thread, TRUE,
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW_OK);
        else
            s_connection_exception (thread,
                ASL_CHANNEL_ERROR, "Channel is not open",
                DEMO_SERVER_CHANNEL, DEMO_SERVER_CHANNEL_FLOW_OK);
    </action>

    <!--  Read Basic Content                                                -->
    <!--  Called when reading basic content.          Read a header frame,
          followed by multiple body frames.  Loops on heartbeat frames,
          accepts only exception methods (see comments below).              -->

    <state name = "read basic content">
        <event name = "header">
            <action name = "read frame header" />
            <action name = "read frame payload" />
            <action name = "check frame format" />
            <action name = "expect content basic header" />
            <action name = "check if basic body complete" />
            <return />
        </event>
        <event name = "expect body">
            <action name = "read frame header" />
            <action name = "read frame payload" />
            <action name = "check frame format" />
            <action name = "expect content basic body" />
            <action name = "check if basic body complete" />
            <return />
        </event>
        <!--  Exception method received -->
        <event name = "method" nextstate = "have incoming method">
            <action name = "decode method" />
            <action name = "expect exception method" />
        </event>
    </state>

    <action name = "expect content basic header">
        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, header_event);
        else
        if (tcb->frame_header.type == ASL_FRAME_METHOD)
            smt_thread_raise_exception (thread, method_event);
        else
        if (demo_server_connection_exception_raised (tcb->connection))
            smt_thread_raise_exception (thread, async_connection_error_event);
        else
        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->frame_header.type == ASL_FRAME_HEADER) {
            ipr_meter_count (demo_broker->imeter);
            tcb->connection->contents_in++;
            tcb->method_in->content =
                demo_content_basic_new ();
            if (demo_content_basic_record_header (
                (demo_content_basic_t *)
                tcb->method_in->content,
                tcb->bucket_in))
                s_connection_exception (thread, ASL_SYNTAX_ERROR,
                    "Invalid content header frame",
                    tcb->method_in->class_id,
                    tcb->method_in->method_id);
            else
            if (((demo_content_basic_t *)
                tcb->method_in->content)->class_id != DEMO_SERVER_BASIC)
                s_connection_exception (thread, ASL_SYNTAX_ERROR,
                    "Content class does not match method class",
                    tcb->method_in->class_id,
                    tcb->method_in->method_id);

            //  Chronometer on content received from wire
            demo_content_basic_chrono_add (tcb->method_in->content);
        }
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Expected a content header frame",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    </action>

    <action name = "check if basic body complete">
        assert ((demo_content_basic_t *) tcb->method_in);
        assert ((demo_content_basic_t *) tcb->method_in->content);

        if (((demo_content_basic_t *)
            tcb->method_in->content)->body_size
        <   ((demo_content_basic_t *)
            tcb->method_in->content)->body_expect)
            smt_thread_raise_exception (thread, expect_body_event);
    </action>

    <action name = "expect content basic body">
        if (tcb->frame_header.type == ASL_FRAME_HEARTBEAT)
            smt_thread_raise_exception (thread, expect_body_event);
        else
        if (tcb->frame_header.type == ASL_FRAME_METHOD)
            smt_thread_raise_exception (thread, method_event);
        else
        if (demo_server_connection_exception_raised (tcb->connection))
            smt_thread_raise_exception (thread, async_connection_error_event);
        else
        //  Check channel is valid
        if (tcb->frame_header.channel != 1)
            s_connection_exception (thread, ASL_CHANNEL_ERROR,
            "Invalid channel",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
        else
        if (tcb->frame_header.type == ASL_FRAME_BODY)
            demo_content_basic_record_body (
                (demo_content_basic_t *)
                tcb->method_in->content, tcb->bucket_in);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Expected a content body frame",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    </action>

    <!--  If we receive a method while reading content, verify that it is a
          legitimate exception and process it immediately.                  -->

    <action name = "expect exception method">
        if (tcb->method_in->class_id  == DEMO_SERVER_CONNECTION &&
            tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_SERVER_CONNECTION &&
            tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_class_event);
        else
        if (tcb->method_in->class_id  == DEMO_SERVER_CHANNEL &&
            tcb->method_in->method_id == DEMO_SERVER_CHANNEL_CLOSE)
            smt_thread_set_next_event (thread, channel_class_event);
        else
            s_connection_exception (thread, ASL_SYNTAX_ERROR,
                "Method not allowed",
                tcb->method_in->class_id,
                tcb->method_in->method_id);
    </action>

    <!--  Defaults for input thread                                         -->
    <!--  Don't handle any exceptions ourselves, just pass them on to the
          connection thread.                                                -->

    <state name = "defaults">
        <!-- Exception events -->
        <event name = "socket error" nextstate = "">
            <action name = "notify connection of exception" />
            <action name = "report socket error" />
        </event>
        <event name = "socket timeout" nextstate = "">
            <action name = "notify connection of exception" />
            <action name = "report socket timeout" />
        </event>
        <event name = "socket closing" nextstate = "">
            <action name = "notify connection of exception" />
            <action name = "report socket error" />
        </event>
        <event name = "smt error" nextstate = "">
            <action name = "notify connection of exception" />
            <action name = "report smt error" />
        </event>
        <event name = "connection abort" nextstate = "">
            <action name = "notify connection of exception" />
        </event>
        <event name = "connection error" nextstate = "">
            <action name = "notify connection of error" />
            <call state = "connection closing" event = "closing" />
        </event>
        <!--  Connection error raised asynchronously by
              demo_server_connection_error.
              If this is the case the appropriate method will have been
              queued with the connection thread, so we want to abort
              processing as soon as possible. -->
        <event name = "async connection error" nextstate = "">
            <call state = "connection closing" event = "closing" />
        </event>
    </state>

    <action name = "notify connection of exception">
        smt_thread_raise_exception (tcb->connection_thread,
            external_exception_event);
        smt_thread_wake (tcb->connection_thread);
    </action>

    <action name = "notify connection of error">
        smt_thread_raise_exception (tcb->connection_thread,
            connection_error_event);
        smt_thread_wake (tcb->connection_thread);
    </action>

    <!--  Connection Closing                                                -->
    <!--  Called when a connection exception is raised in the input thread.
          The connection thread will initiate an active close, so we can
          discard anything else received in the mean time.                  -->

    <state name = "connection closing">
        <event name = "closing">
            <action name = "read frame header" />
            <action name = "read frame payload" />
            <action name = "check frame format" />
            <action name = "expect method frame" />
        </event>
        <event name = "method">
            <action name = "decode method" />
            <action name = "expect connection close ok" />
        </event>
        <event name = "discard frame">
            <action name = "read frame header" />
            <action name = "read frame payload" />
            <action name = "check frame format" />
            <action name = "expect method frame" />
        </event>
        <event name = "connection close ok" nextstate = "">
            <action name = "notify connection of close ok" />
        </event>
        <!--  Override to prevent looping -->
        <event name = "connection error" nextstate = "">
            <action name = "notify connection of exception" />
        </event>
    </state>

    <action name = "expect method frame">
        if (tcb->frame_header.type != ASL_FRAME_METHOD)
            smt_thread_set_next_event (thread, discard_frame_event);
        else
            smt_thread_set_next_event (thread, method_event);
    </action>

    <action name = "expect connection close ok">
        if (tcb->method_in->class_id  == DEMO_SERVER_CONNECTION &&
            tcb->method_in->method_id == DEMO_SERVER_CONNECTION_CLOSE_OK)
            smt_thread_set_next_event (thread, connection_close_ok_event);
        else
            smt_thread_set_next_event (thread, discard_frame_event);
    </action>

</thread>

<!---------------------------------------------------------------------------->
<!--  Heartbeat thread  ------------------------------------------------------>

<thread
    name = "heartbeat"
    schedule = "poll"
    context = "connection"
    share = "1"
    >

    <handler name = "thread init">
        thread->animate = (tcb->trace >= ASL_TRACE_MED);
        smt_thread_set_next_event (thread, peer_alive_event);
    </handler>

    <handler name = "thread destroy">
        tcb->heartbeat_thread = NULL;

    </handler>

    <state name = "monitor heartbeat">
        <event name = "peer alive" nextstate = "monitor heartbeat">
            <action name = "wait for heartbeat interval" />
            <action name = "check peer liveness" />
        </event>
        <event name = "peer slowing" nextstate = "monitor heartbeat">
            <action name = "report peer slowing" />
            <action name = "wait for heartbeat interval" />
            <action name = "check peer liveness" />
        </event>
        <event name = "peer stopped" nextstate = "">
            <action name = "shut down agent" />
            <action name = "report peer stopped" />
        </event>
    </state>

    <action name = "wait for heartbeat interval">
        smt_timer_request_delay (thread, tcb->heartbeat * 1000 * 1000, SMT_NULL_EVENT);
    </action>

    <action name = "check peer liveness">
        //  Decrease peer liveness.  If liveness reaches 1 log a warning, if
        //  liveness reaches zero, pronounce the peer dead
        tcb->liveness--;
        if (tcb->liveness) {
            if (tcb->liveness == 1)
                smt_thread_set_next_event (thread, peer_slowing_event);
            else
                smt_thread_set_next_event (thread, peer_alive_event);
        }
        else
            smt_thread_set_next_event (thread, peer_stopped_event);
    </action>

    <action name = "report peer slowing">
        smt_log_print (demo_broker->alert_log,
            "W: client heartbeat slowing (%s, %s, %s, %s)",
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    </action>

    <!--  If heartbeats stop we can't use an exception to notify the agent
          since it is likely to be blocked waiting for socket requests to
          complete.                                                         -->

    <action name = "shut down agent">
        if (tcb->input_thread)
            smt_thread_shut_down (tcb->input_thread);
        if (tcb->connection_thread)
            smt_thread_shut_down (tcb->connection_thread);
    </action>

    <action name = "report peer stopped">
        smt_log_print (demo_broker->alert_log,
            "W: client heartbeat stopped, closing connection (%s, %s, %s, %s)",
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    </action>

    <!--  Defaults for heartbeat thread                                     -->

    <state name = "defaults">
        <!-- Exception events -->
        <event name = "smt error" nextstate = "">
            <action name = "notify connection of exception" />
        </event>

    </state>

</thread>

<!---------------------------------------------------------------------------->
<!--  Helper code shared between agents  ------------------------------------->

<private name = "types">
//  Function prototypes

//  Raises a connection exception
static void
s_connection_exception (
    smt_thread_t *thread,
    dbyte reply_code,
    char *reply_text,
    dbyte faulting_class_id,
    dbyte faulting_method_id);

//  Aborts the connection immediately
static void
s_connection_abort (
    smt_thread_t *thread,
    dbyte reply_code,
    char *reply_text,
    dbyte faulting_class_id,
    dbyte faulting_method_id);

//  Helper function for reporting errors
static void
s_report_error (
    smt_thread_t *thread,
    char *context,
    char *action_taken,
    dbyte reply_code,
    char *reply_text,
    dbyte faulting_class_id,
    dbyte faulting_method_id);

//  Executes an incoming method
static inline void
s_execute_method (
    smt_thread_t *thread,
    Bool channel_method,
    int class_id,
    int method_id);

//  Executes an incoming functional method
static void
s_execute_functional_method (
    smt_thread_t *thread,
    dbyte class_id,
    dbyte method_id,
    dbyte out_class_id,
    dbyte out_method_id);

//  Writes a non-null payload to the socket
static void
s_write_payload (
    smt_thread_t *thread,
    dbyte channel,
    byte type,
    ipr_bucket_t **bucket_p);

//  Writes an outgoing method to the socket
static void
s_send_method_out (
    smt_thread_t *thread,
    dbyte channel,
    smt_log_t *log);

//  Helper functions for socket input/output
static inline void
s_socket_write (
    smt_thread_t *thread,
    byte *buffer,
    size_t size);

static inline void
s_socket_write_bucket (
    smt_thread_t *thread,
    ipr_bucket_t *bucket);

static void inline
s_socket_write_shortstr (
    smt_thread_t *thread,
    char *string);

static inline void
s_socket_read (
    smt_thread_t *thread,
    byte *buffer,
    size_t size);

static inline void
s_socket_read_bucket (
    smt_thread_t *thread,
    ipr_bucket_t **bucket_p,
    size_t size);

//  Table of predecessor methods for each method.  If >0, then when we get a
//  specific method we can assert that we previously sent the predecessor on
//  the same channel.
static dbyte s_class_index [6 + 1] = {
    10, 20, 40, 50, 60, 61500, 0
};
static dbyte s_method_index [6][11] = {
    {  10, 11, 20, 21, 30, 31, 40, 41, 50, 51,  0 },
    {  10, 11, 20, 21, 40, 41,  0,  0,  0,  0,  0 },
    {  10, 11, 20, 21,  0,  0,  0,  0,  0,  0,  0 },
    {  10, 11, 20, 21, 30, 31, 40, 41,  0,  0,  0 },
    {  10, 11, 20, 21, 30, 40, 50, 51, 52,  0,  0 },
    {  10, 11, 20, 21,  0,  0,  0,  0,  0,  0,  0 }
};
static dbyte s_predecessor [6][11] = {
    {   0, 10,  0, 20,  0, 30,  0, 40,  0, 50,  0 },
    {   0, 10,  0, 20,  0, 40,  0,  0,  0,  0,  0 },
    {   0, 10,  0, 20,  0,  0,  0,  0,  0,  0,  0 },
    {   0, 10,  0, 20,  0, 30,  0, 40,  0,  0,  0 },
    {   0, 10,  0, 20,  0,  0,  0, 50, 50,  0,  0 },
    {   0, 10,  0, 20,  0,  0,  0,  0,  0,  0,  0 }
};
</private>

<private name = "functions">
#define tcb ((demo_server_agent_connection_context_t *) thread->context)

//  Raises a connection exception
//  Called internally by the agent when a protocol error is detected
static void
s_connection_exception (
    smt_thread_t *thread,               //  Calling thread
    dbyte reply_code,                   //  Error code
    char *reply_text,                   //  Error text
    dbyte faulting_class_id,            //  Faulting class id, if any
    dbyte faulting_method_id)           //  Faulting method id, if any
{
    s_report_error (thread, "Connection", "closing connection",
        reply_code, reply_text, faulting_class_id, faulting_method_id);

    //  Raise an exception in the server connection object so that
    //  the input thread will pick up on this at the next frame boundary
    //  and abort processing any current method/content
    demo_server_connection_raise_exception (tcb->connection,
        reply_code, reply_text, faulting_method_id, faulting_class_id);
    smt_thread_raise_exception (thread, connection_error_event);
}

//  Aborts the connection immediately
//  Called internally by the agent if we detect an unrecoverable error
//  (e.g. malformed frame)
static void
s_connection_abort (
    smt_thread_t *thread,               //  Calling thread
    dbyte reply_code,                   //  Error code
    char *reply_text,                   //  Error text
    dbyte faulting_class_id,            //  Faulting class id, if any
    dbyte faulting_method_id)           //  Faulting method id, if any
{
    s_report_error (thread, "Connection", "aborting connection",
        reply_code, reply_text, faulting_class_id, faulting_method_id);

    smt_thread_raise_exception (thread, connection_abort_event);
}

//  Helper function to report connection and channel errors
static void
s_report_error (
    smt_thread_t *thread,               //  Calling thread
    char *context,                      //  Error context
    char *action_taken,                 //  Action taken
    dbyte reply_code,                   //  Error code
    char *reply_text,                   //  Error text
    dbyte faulting_class_id,            //  Faulting class id, if any
    dbyte faulting_method_id)           //  Faulting method id, if any
{
    if (reply_code == 100)
        return;
    if ((tcb->trace >= ASL_TRACE_MED || reply_code >= 500) &&
        faulting_class_id && faulting_method_id)
        smt_log_print (demo_broker->alert_log,
            "E: %s error: %d %s "
            "(faulting method %u, %u), %s "  "(%s, %s, %s, %s)",
            context, reply_code, reply_text,
            faulting_class_id, faulting_method_id, action_taken,
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
    else
        smt_log_print (demo_broker->alert_log,
            "E: %s error: %d %s, %s "  "(%s, %s, %s, %s)",
            context, reply_code, reply_text, action_taken,
            tcb->connection->client_address,
            tcb->connection->client_product,
            tcb->connection->client_version,
            tcb->connection->client_instance);
}

//  Executes an incoming method if it matches the requested method,
//  raises a connection exception if not.
static inline void
s_execute_method (
    smt_thread_t *thread,               //  Thread executing the method
    Bool channel_method,                //  FALSE == connection method
    int class_id,                       //  Expected class id
    int method_id                       //  Expected method id
    )
{
    //  Trap possible async exception before executing method
    if (demo_server_connection_exception_raised (tcb->connection))
        smt_thread_raise_exception (thread, async_connection_error_event);
    else
    if (tcb->method_in->class_id  == class_id
    &&  tcb->method_in->method_id == method_id) {
        demo_server_method_execute (tcb->method_in, tcb->connection,
            channel_method ? tcb->channel : NULL);
        if (demo_server_connection_exception_raised (tcb->connection))
            s_connection_exception (thread,
                tcb->connection->reply_code,
                tcb->connection->reply_text,
                tcb->connection->faulting_class_id,
                tcb->connection->faulting_method_id);
    }
    else
        s_connection_exception (thread, ASL_SYNTAX_ERROR,
            "Method not valid here",
            tcb->method_in->class_id,
            tcb->method_in->method_id);
}

//  Executes an incoming functional method.  Checks if we sent the predecessor
//  method (if any) and raises a connection exception if not.
static void
s_execute_functional_method (
    smt_thread_t *thread,               //  Thread executing the method
    dbyte class_id,                     //  Incoming class id
    dbyte method_id,                    //  Incoming method id
    dbyte out_class_id,                 //  Last sent class id
    dbyte out_method_id)                //  Last sent method id
{
    dbyte
        pred_method_id;                 //  Predecessor method id, if any
    uint
        class_index,
        method_index;

    //  Convert input class index (20, 30, 41,...) into lookup index
    for (class_index = 0; s_class_index [class_index]; class_index++)
        if (class_id == s_class_index [class_index])
            break;

    //  Convert input method index (20, 30, 41,...) into lookup index
    for (method_index = 0; s_method_index [class_index][method_index]; method_index++)
        if (method_id == s_method_index [class_index][method_index])
            break;

    //  If we got a method that requires a predecessor, check we sent that
    if (s_method_index [class_index][method_index]) {
        pred_method_id = s_predecessor [class_index][method_index];
        if (pred_method_id > 0
        && (pred_method_id != out_method_id
        &&  out_class_id == class_id))
            s_connection_exception (thread,
                ASL_SYNTAX_ERROR, "Method not allowed at this stage",
                method_id, class_id);
        else
            demo_server_method_execute (tcb->method_in, tcb->connection,
                tcb->channel);
            if (demo_server_connection_exception_raised (tcb->connection))
                s_connection_exception (thread,
                    tcb->connection->reply_code,
                    tcb->connection->reply_text,
                    tcb->connection->faulting_class_id,
                    tcb->connection->faulting_method_id);
    }
    else
        s_connection_exception (thread,
            ASL_SYNTAX_ERROR, "Method invalid - unknown method id",
            method_id, class_id);
}

//  Sends an outgoing method to the socket
static void
s_send_method_out (
    smt_thread_t *thread,
    dbyte channel,
    smt_log_t *log)
{
    ipr_bucket_t
        *payload;

    if (tcb->trace >= ASL_TRACE_LOW)
        demo_server_method_dump (tcb->method_out, log, "I: send ");

    //  Send the method
    payload = demo_server_method_encode (tcb->method_out);
    s_write_payload (thread, channel, ASL_FRAME_METHOD, &payload);

    tcb->out_class_id  = tcb->method_out->class_id;
    tcb->out_method_id = tcb->method_out->method_id;

}

//  Writes a non-null payload to the socket
static void
s_write_payload (
    smt_thread_t *thread,
    dbyte channel,
    byte type,                          //  Frame type indicator
    ipr_bucket_t **payload_p)
{
    ipr_bucket_t
        *payload = NULL,
        *bucket;                        //  Header bucket
    byte
        *bucket_ptr;
    static byte
        frame_end = ASL_FRAME_END;

    if (payload_p) {
        payload = *payload_p;
        if (!payload)
            return;                     //  Ignore empty payloads
    }
    if (payload->cur_size > tcb->frame_max) {
        icl_console_print ("E: frame type=%d is too large, discarded (size=%d, max=%d)",
            type, payload->cur_size, tcb->frame_max);
        return;
    }
    //  Write ASL frame header to normal bucket
    bucket = ipr_bucket_new (ASL_HEADER_SIZE);
    bucket_ptr = bucket->data;
    PUT_OCTET (bucket_ptr, type);
    PUT_SHORT (bucket_ptr, channel);
    PUT_LONG  (bucket_ptr, payload->cur_size);
    bucket->cur_size = bucket_ptr - bucket->data;

    s_socket_write_bucket (thread, bucket);
    s_socket_write_bucket (thread, payload);
    s_socket_write        (thread, &frame_end, 1);
    ipr_bucket_unlink (&bucket);
    ipr_bucket_unlink (payload_p);
}

//  Helper functions for socket input/output
static void inline
s_socket_write (
    smt_thread_t *thread,
    byte *buffer,
    size_t size)
{
    tcb->connection->traffic_out += size;
    smt_socket_request_write (
        thread,
        tcb->socket,
        0,
        size,
        buffer,
        SMT_NULL_EVENT);
}

static void inline
s_socket_write_bucket (
    smt_thread_t *thread,
    ipr_bucket_t *bucket)
{
    tcb->connection->traffic_out += bucket->cur_size;
    smt_socket_request_write_bucket (
        thread,
        tcb->socket,
        0,
        bucket,
        SMT_NULL_EVENT);
}

static void inline
s_socket_write_shortstr (
    smt_thread_t *thread,
    char *string)
{
    ipr_bucket_t
        *bucket;                        //  Formatted output

    assert (strlen (string) < 256);
    bucket = ipr_bucket_new (256);
    bucket->data [0] = (byte) strlen (string);
    memcpy (bucket->data + 1, string, strlen (string));
    bucket->cur_size = 1 + strlen (string);
    s_socket_write_bucket (thread, bucket);
    ipr_bucket_unlink (&bucket);
}

static void inline
s_socket_read (
    smt_thread_t *thread,
    byte *buffer,
    size_t size)
{
    if (size <= IPR_BUCKET_MAX_SIZE) {
        tcb->connection->traffic_in += size;
        smt_socket_request_read (
            thread,
            tcb->socket,
            0,
            size,
            size,
            buffer,
            SMT_NULL_EVENT);
    }
    else
        s_connection_abort (thread,
            ASL_FRAME_ERROR, "Illegal frame size specified", 0, 0);
}

static void inline
s_socket_read_bucket (
    smt_thread_t *thread,
    ipr_bucket_t **bucket_p,
    size_t size)
{
    if (size <= IPR_BUCKET_MAX_SIZE) {
        tcb->connection->traffic_in += size;
        smt_socket_request_read_bucket (
            thread,
            tcb->socket,
            0,
            size,
            size,
            bucket_p,
            SMT_NULL_EVENT);
    }
    else
        s_connection_abort (thread,
            ASL_FRAME_ERROR, "Illegal frame size specified", 0, 0);
}

#undef tcb
</private>

</agent>
