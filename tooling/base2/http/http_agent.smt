<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
    name    = "http_agent"
    script  = "smt_gen.gsl"
    animate = "1"
    >
<private name = "types">
#define BASE_ANIMATE    1
</private>

<import class = "http" />

<handler name = "agent init">
    <argument name = "server" type = "http_server_t *" />
    //
    smt_thread_t
        *master_thread;
    http_agent_master_context_t
        *master_tcb;

    //  Currently we poke the values into the tcb
    master_thread = http_agent_master_thread_new ();
    master_tcb = master_thread->context;
    master_tcb->server = http_server_link (server);
    master_tcb->alert_log = smt_log_link (server->alert_log);
    master_tcb->debug_log = smt_log_link (server->debug_log);

    smt_thread_wake    (master_thread);
    smt_thread_unlink (&master_thread);
</handler>

<method name = "driver reply">
    <!-- Normal response to clients, context is same object
        provided in method call to back-end driver -->
</method>

<method name = "driver abort">
    <!-- aborts the client's request, used for clients
        that are misbehaving in a deliberate way -->
</method>

<catch error = "SMT_SOCKET_ERROR"   event = "socket error" />
<catch error = "SMT_SOCKET_CLOSING" event = "socket closing" />
<catch error = "SMT_TIMEOUT"        event = "socket timeout" />
<catch                              event = "smt error" />

<!-- Master thread ----------------------------------------------------------->

<thread name = "master">
    <context>
        http_server_t
            *server;                    //  Parent server instance
        smt_log_t
            *alert_log,                 //  Alert log file
            *debug_log;                 //  Debug log file
        smt_socket_t
            *master_socket,             //  Socket for connect requests
            *client_socket;             //  Newly-accepted socket
    </context>

    <handler name = "thread init">
        thread->animate = http_config_animate (http_config);
        smt_socket_request_trace (http_config_trace (http_config));
        smt_thread_set_next_event (thread, ok_event);
    </handler>

    <handler name = "thread destroy">
        smt_log_unlink     (&tcb->alert_log);
        smt_log_unlink     (&tcb->debug_log);
        http_server_unlink (&tcb->server);
        smt_socket_destroy (&tcb->client_socket);
        smt_socket_destroy (&tcb->master_socket);
    </handler>

    <!--  INITIALISE MASTER  ------------------------------------------------->

    <state name = "initialise master">
        <event name = "ok" nextstate = "accepting connections">
            <action name = "initialise server" />
            <action name = "allow server to settle" />
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

        port  = http_config_port  (http_config);
        user  = http_config_user  (http_config);
        group = http_config_group (http_config);
        if (atoi (port) < 80
        ||  atoi (port) > 65535) {
            smt_log_print (tcb->alert_log, "E: port %s is not valid for HTTP service", port);
            exit (1);                   //  FIXME: see BASE2-289
        }
        else {
            apr_pool_create (&pool, icl_global_pool);
            apr_pool_tag (pool, "http_initialise_server)");

            //  Report hostname and all visible network interfaces
            if (apr_gethostname (hostname, ICL_SHORTSTR_MAX, pool) == APR_SUCCESS) {
                apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
                if (sockaddr) {
                    apr_sockaddr_ip_get (&address, sockaddr);
                    smt_log_print (tcb->alert_log, "I: server host name is %s (%s)", hostname, address);
                }
                else
                    smt_log_print (tcb->alert_log, "E: cannot translate host name '%s'", hostname);
            }
            //  Report which interfaces server is going to listen to
            icl_shortstr_cpy (hostname, http_config_listen (http_config));
            if (streq (hostname, "*")) {
                smt_log_print (tcb->alert_log, "I: listening on port %s, all network interfaces", port);
                icl_shortstr_cpy (hostname, APR_ANYADDR);
            }
            else
                smt_log_print (tcb->alert_log, "I: listening on port %s, interface %s", port, hostname);

            apr_sockaddr_info_get (&sockaddr, hostname, APR_UNSPEC, 0, 0, pool);
            if (sockaddr)
                apr_sockaddr_ip_get (&address, sockaddr);
            else
                smt_log_print (tcb->alert_log, "E: cannot translate host name '%s'", hostname);

            apr_pool_destroy (pool);
            tcb->master_socket = smt_socket_new ();
            if (tcb->master_socket) {
                //  Set Nagle's algorithm on or off
                rc = smt_socket_set_nodelay (tcb->master_socket, http_config_tcp_nodelay (http_config));
                if (rc)
                    smt_log_print (tcb->alert_log, "W: could not set TCP/IP NODELAY option");

                //  Report and set socket buffers if wanted
                new_size = http_config_tcp_rcvbuf (http_config);
                if (new_size) {
                    rc = smt_socket_set_rcvbuf (tcb->master_socket, new_size, &old_size);
                    if (!rcv_warned) {
                        rcv_warned = TRUE;
                        if (rc)
                            smt_log_print (tcb->alert_log,
                                "W: could not set TCP/IP receive buffer size");
                        else
                            smt_log_print (tcb->alert_log,
                                "I: TCP/IP receive buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                new_size = http_config_tcp_sndbuf (http_config);
                if (new_size) {
                    rc = smt_socket_set_sndbuf (tcb->master_socket, new_size, &old_size);
                    if (!snd_warned) {
                        snd_warned = TRUE;
                        if (rc)
                            smt_log_print (tcb->alert_log,
                                "W: could not set TCP/IP send buffer size");
                        else
                            smt_log_print (tcb->alert_log,
                                "I: TCP/IP sending buffer was %d, setting to %d",
                                old_size, new_size);
                    }
                }
                //  Start listening for connections on socket
                rc = smt_socket_passive (tcb->master_socket, thread, port, hostname, 500);

                //  Try to drop to safe user/group, complain if not possible
                if (*user && *group) {
                    if (ipr_uid_run_as (user, group)) {
                        smt_log_print (tcb->alert_log,
                            "E: could not run as user=%s group=%s", user, group);
                        exit (1);
                    }
                    else
                        smt_log_print (tcb->alert_log,
                            "I: server is now running as user=%s group=%s", user, group);
                }
            }
            if (!tcb->master_socket || (rc != SMT_OK)) {
                smt_log_print (tcb->alert_log,
                    "E: could not open port %s: %s", port, smt_thread_error (thread));
                exit (1);               //  FIXME: see BASE2-289
            }
        }
    </action>

    <action name = "allow server to settle">
        smt_timer_request_delay (thread, 1 * 1000 * 1000, SMT_NULL_EVENT);
    </action>

    <action name = "report server ready">
        smt_log_print (tcb->debug_log, "I: ready for incoming HTTP requests");
        smt_thread_set_next_event (thread, ready_event);
    </action>

   <!--  ACCEPTING CONNECTIONS
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
        http_agent_connection_context_t
            *connection_tcb;

        connection_thread = http_agent_connection_thread_new ();
        connection_thread->priority = SMT_PRIORITY_HIGH;

        connection_tcb            = connection_thread->context;
        connection_tcb->socket    = tcb->client_socket;
        connection_tcb->server    = http_server_link (tcb->server);
        connection_tcb->alert_log = smt_log_link (tcb->alert_log);
        connection_tcb->debug_log = smt_log_link (tcb->debug_log);
        tcb->client_socket        = NULL;

        smt_thread_wake    (connection_thread);
        smt_thread_unlink (&connection_thread);
    </action>

    <action name = "report accept error">
        int
            accept_retry_timeout;

        smt_log_print (tcb->alert_log,
            "E: could not accept connection: %s", smt_thread_error (thread));

        //  If we ran out of files wait the configured amount of time and retry
        if (thread->result == SMT_SOCKET_ERROR &&
           (APR_STATUS_IS_ENFILE(thread->error) ||
            APR_STATUS_IS_EMFILE(thread->error))) {
            accept_retry_timeout = http_config_accept_retry_timeout (http_config);
            smt_log_print (tcb->alert_log,
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
        smt_log_print (tcb->alert_log,
            "E: SMT error: %s", smt_thread_error (thread));
    </action>

    <action name = "close master socket">
        if (tcb->master_socket)
            smt_socket_request_close (thread, &tcb->master_socket, 0, SMT_NULL_EVENT);
    </action>
</thread>

<!---------------------------------------------------------------------------->
<!--  Connection thread                                                     -->

<thread name = "connection">
    <context>
        http_server_t
            *server;                    //  Parent server instance
        smt_log_t
            *alert_log,                 //  Alert log file
            *debug_log;                 //  Debug log file
        smt_socket_t
            *socket;                    //  Socket to client
        ipr_bucket_t
            *bucket;                    //  Data read from client
        icl_shortstr_t
            address;                    //  IP address of client
        qbyte
            read_timeout,               //  Socket read timeout
            write_timeout;              //  Socket write timeout
        http_request_t
            *request;                   //  Parsed request context
        http_response_t
            *response;                  //  Response to client
        http_access_context_t
            *access_context;            //  Access module work area
        http_driver_context_t
            *driver_context;            //  Driver module work area
        FILE
            *response_stream;           //  File being sent to client
    </context>

    <handler name = "thread init">
        thread->animate    = http_config_animate (http_config);
        tcb->read_timeout  = http_config_read_timeout (http_config) * 1000;
        tcb->write_timeout = http_config_write_timeout (http_config) * 1000;
        icl_atomic_inc32 ((volatile qbyte *) &(tcb->server->clients));
        the_next_event = ok_event;
    </handler>

    <handler name = "thread destroy">
        icl_atomic_dec32 ((volatile qbyte *) &(tcb->server->clients));
        http_server_unlink (&tcb->server);
        smt_log_unlink (&tcb->alert_log);
        smt_log_unlink (&tcb->debug_log);
        smt_socket_destroy (&tcb->socket);
        ipr_bucket_unlink (&tcb->bucket);
        http_request_unlink (&tcb->request);
        http_response_unlink (&tcb->response);
        http_access_context_destroy (&tcb->access_context);
        http_driver_context_destroy (&tcb->driver_context);
    </handler>

    <!--  AFTER INIT  ------------------------------------------------------>

    <state name = "after init">
        <event name = "ok" nextstate = "have http request">
            <action name = "initialise http request" />
            <action name = "do access handlers open" />
            <action name = "prepare to read header" />
            <action name = "read next http request" />
        </event>
        <event name = "incomplete" nextstate = "have http request">
            <action name = "read next http request" />
        </event>
    </state>

    <action name = "initialise http request">
        apr_sockaddr_t
            *sockaddr;
        char
            *address;

        //  Get client IP address
        apr_socket_addr_get (&sockaddr, APR_REMOTE, tcb->socket->pollfd.desc.s);
        apr_sockaddr_ip_get (&address, sockaddr);
        icl_shortstr_cpy (tcb->address, address);
        apr_socket_opt_set (tcb->socket->pollfd.desc.s, APR_TCP_NODELAY, 1);
    </action>

    <action name = "do access handlers open">
        ipr_looseref_t
            *looseref;                      //  Access module reference
        int
            rc = HTTP_ACCESS_CONTINUE;

        assert (!tcb->access_context);
        tcb->access_context = http_access_context_new (tcb->server);
        http_access_context_set_thread  (tcb->access_context, thread);
        http_access_context_set_address (tcb->access_context, tcb->address);
        looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
        while (looseref) {
            rc = http_access_module_request_open (
                (http_access_module_t *) (looseref->object), tcb->access_context);
            if (rc == HTTP_ACCESS_ABORT) {
                //  Abort the current connection, do not send a response
                smt_thread_raise_exception (thread, access_abort_event);
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
    </action>

    <action name = "prepare to read header">
        //  Allocate new bucket for request data
        ipr_bucket_unlink (&tcb->bucket);
        tcb->bucket = ipr_bucket_new (http_config_header_max (http_config));
    </action>

    <action name = "read next http request">
        <action name = "read input from socket" />
        <action name = "check if header complete" />
        <action name = "parse http request" />
        <action name = "do access handlers before" />
        <action name = "check if content complete" />
    </action>

    <action name = "read input from socket">
        size_t
            available;                  //  Bytes available in bucket

        assert (tcb->bucket);
        available = tcb->bucket->max_size - tcb->bucket->cur_size - 1;
        if (available)
            smt_socket_request_read (
                thread,
                tcb->socket,
                tcb->read_timeout,
                1,
                available,
                tcb->bucket->data + tcb->bucket->cur_size,
                SMT_NULL_EVENT);
        else {
            smt_log_print (tcb->alert_log,
                "E: (%s) invalid HTTP request - header too large", tcb->address);
            smt_thread_raise_exception (thread, exception_event);
        }
    </action>

    <action name = "check if header complete">
        //  Update bucket size with amount of data read
        tcb->bucket->cur_size += tcb->socket->io_size;
        //  Look for complete header, ending in blank line
        if (ipr_bucket_find (tcb->bucket, 0, (byte *) "\r\n\r\n", 4)) {
            ipr_meter_count (tcb->server->imeter);
            icl_atomic_inc32 ((volatile qbyte *) &(tcb->server->requests));
        }
        else
            smt_thread_raise_exception (thread, incomplete_event);
    </action>

    <action name = "parse http request">
        //  Clean-up any old context for persistent connections
        http_request_unlink (&tcb->request);
        http_response_unlink (&tcb->response);

        //  Terminate bucket data to allow string processing
        tcb->bucket->data [tcb->bucket->cur_size] = 0;

        //  Create & parse request, and response
        tcb->request  = http_request_new (tcb->server, tcb->socket, tcb->bucket);
        tcb->response = http_response_new (tcb->server, tcb->request);
        http_request_set_address (tcb->request, tcb->address);

        http_access_context_set_request  (tcb->access_context, tcb->request);
        http_access_context_set_response (tcb->access_context, tcb->response);

        //  Bucket is now known as tcb->request->header
        ipr_bucket_unlink (&tcb->bucket);

        if (tcb->response->reply_code != HTTP_REPLY_OK)
            smt_thread_raise_exception (thread, http_error_event);
    </action>

    <action name = "do access handlers before">
        ipr_looseref_t
            *looseref;                      //  Access module reference
        int
            rc = HTTP_ACCESS_CONTINUE;

        looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
        while (looseref) {
            rc = http_access_module_request_before (
                (http_access_module_t *) (looseref->object), tcb->access_context);
            if (rc == HTTP_ACCESS_ABORT) {
                //  Abort the current connection, do not send a response
                smt_thread_raise_exception (thread, access_abort_event);
                break;
            }
            else
            if (rc == HTTP_ACCESS_RETRY) {
                //  Wait a short time and then retry the access modules
                smt_thread_raise_exception (thread, access_retry_event);
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
        if (rc == HTTP_ACCESS_CONTINUE
        &&  tcb->response->reply_code != HTTP_REPLY_OK)
            smt_thread_raise_exception (thread, http_error_event);
    </action>

    <action name = "check if content complete">
        //  If there is content, prepare to get it all
        if (http_request_content_parse (tcb->request))
            the_next_event = incomplete_event;
        else
            the_next_event = ok_event;
    </action>

    <!--  HAVE HTTP REQUEST  ----------------------------------------------->

    <state name = "have http request">
        <event name = "ok" nextstate = "waiting for server">
            <action name = "pass request to driver" />
        </event>
        <event name = "incomplete" nextstate = "have http request">
            <action name = "read content from socket" />
            <action name = "process content body" />
        </event>
    </state>

    <action name = "pass request to driver">
        http_driver_module_t
            *driver_module = NULL;      //  Driver, if found
        ipr_looseref_t
            *looseref;                  //  Driver module reference

        looseref = ipr_looseref_list_first (tcb->server->http_driver_module_list);
        while (looseref) {
            driver_module = (http_driver_module_t *) (looseref->object);
            if (ipr_str_prefixed (tcb->request->path, driver_module->path)) {
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
        if (looseref) {
            assert (!tcb->request->pathroot);
            if (tcb->driver_context)
                http_driver_context_init (tcb->driver_context, tcb->request, tcb->response);
            else
                tcb->driver_context = http_driver_context_new (
                    thread, tcb->server, tcb->request, tcb->response);

            assert (tcb->request->path);
            tcb->request->pathroot = icl_mem_strdup (driver_module->path);
            tcb->request->pathinfo = icl_mem_strdup (tcb->request->path + strlen (driver_module->path));
            if (tcb->request->method == HTTP_METHOD_GET)
                http_driver_module_request_get (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_HEAD)
                http_driver_module_request_head (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_POST)
                http_driver_module_request_post (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_PUT)
                http_driver_module_request_put (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_DELETE)
                http_driver_module_request_delete (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_MOVE)
                http_driver_module_request_move (driver_module, tcb->driver_context);
            else
            if (tcb->request->method == HTTP_METHOD_COPY)
                http_driver_module_request_copy (driver_module, tcb->driver_context);
            else
                assert (0);         //  Method has already been validated
        }
        else {
            http_response_set_error (tcb->response, HTTP_REPLY_NOTIMPLEMENTED,
                "The '%s' method is not implemented on this service", tcb->request->request_method);
            the_next_event = unimplemented_event;
        }
    </action>

    <action name = "read content from socket">
        size_t
            read_min, read_max;

        if (tcb->request->content)
            read_min = read_max = http_request_content_pending (tcb->request);
        else {
            read_max = MIN (IPR_BUCKET_MAX_SIZE, http_request_content_pending (tcb->request));
            read_min = MIN (read_max, (uint) http_request_content_pending (tcb->request));
        }
        tcb->bucket = ipr_bucket_new (read_max);
        smt_socket_request_read (
            thread,
            tcb->socket,
            tcb->read_timeout,
            read_min,
            read_max,
            tcb->bucket->data,
            SMT_NULL_EVENT);
    </action>

    <action name = "process content body">
        //  Update bucket size with amount of data read
        tcb->bucket->cur_size += tcb->socket->io_size;
        http_request_content_append (tcb->request, tcb->bucket);
        ipr_bucket_unlink (&tcb->bucket);

        if (http_request_content_pending (tcb->request))
            the_next_event = incomplete_event;
        else
            the_next_event = ok_event;
    </action>

    <!--  WAITING FOR SERVER  ---------------------------------------------->

    <state name = "waiting for server">
        <method name = "driver reply" nextstate = "sending dynamic">
            <action name = "format http reply" />
            <action name = "get next dynamic bucket" />
        </method>
        <method name = "driver abort" nextstate = "">
            <action name = "pause client for abort" />
            <action name = "flush the connection" />
            <action name = "do access handlers close" />
        </method>
        <event name = "unimplemented" nextstate = "sending dynamic">
            <action name = "format http reply" />
            <action name = "get next dynamic bucket" />
        </event>
    </state>

    <action name = "format http reply">
        http_response_format_reply (tcb->response);
    </action>

    <action name = "get next dynamic bucket">
        ipr_bucket_unlink (&tcb->bucket);
        tcb->bucket = ipr_bucket_list_pop (tcb->response->bucket_list);
        if (tcb->bucket)
            the_next_event = ok_event;
        else
            the_next_event = finished_event;
    </action>

    <action name = "pause client for abort">
        //  We wait for a while, then kill the connection.  This slows down a
        //  client that we perceive to be hostile
        smt_timer_request_delay (thread,
            http_config_abort_delay (http_config) * 1000 * 1000, SMT_NULL_EVENT);
    </action>

    <action name = "flush the connection">
        if (tcb->socket)
            smt_socket_request_close (thread, &tcb->socket, 0, SMT_NULL_EVENT);
    </action>

    <action name = "do access handlers close">
        ipr_looseref_t
            *looseref;                      //  Access module reference

        //  Do close handler only if open was done (that allocates the context)
        if (tcb->access_context) {
            looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
            while (looseref) {
                http_access_module_request_close (
                    (http_access_module_t *) (looseref->object), tcb->access_context);
                looseref = ipr_looseref_list_next (&looseref);
            }
        }
    </action>

    <!--  SENDING DYNAMIC  ------------------------------------------------->

    <state name = "sending dynamic">
        <event name = "ok">
            <action name = "send bucket to client" />
        </event>
        <event name = "bucket sent">
            <action name = "get next dynamic bucket" />
        </event>
        <event name = "finished" nextstate = "sending static">
            <action name = "open static content" />
            <action name = "get next static bucket" />
        </event>
        <event name = "empty" nextstate = "response sent">
            <action name = "do access handlers after" />
            <action name = "check if persistent connection" />
        </event>
    </state>

    <action name = "send bucket to client">
        smt_socket_request_write_bucket (
            thread,
            tcb->socket,
            tcb->write_timeout,
            tcb->bucket,
            bucket_sent_event);
    </action>

    <action name = "open static content">
        if (tcb->response->filename) {
            tcb->response_stream = fopen (tcb->response->filename, "rb");
            if (!tcb->response_stream) {
                smt_log_print (tcb->alert_log,
                    "E: cannot access '%s'", tcb->response->filename);
                http_response_set_error (tcb->response, HTTP_REPLY_INTERNALERROR,
                    "Cannot access static resource content");
                smt_thread_raise_exception (thread, http_error_event);
            }
        }
        else
            smt_thread_raise_exception (thread, empty_event);
    </action>

    <action name = "get next static bucket">
        ipr_bucket_unlink (&tcb->bucket);
        tcb->bucket = ipr_bucket_new (128000);
        ipr_bucket_load (tcb->bucket, tcb->response_stream);
        if (tcb->bucket->cur_size)
            the_next_event = ok_event;
        else
            the_next_event = finished_event;
    </action>

    <action name = "do access handlers after">
        ipr_looseref_t
            *looseref;                      //  Access module reference
        int
            rc;

        looseref = ipr_looseref_list_first (tcb->server->http_access_module_list);
        while (looseref) {
            rc = http_access_module_request_after (
                (http_access_module_t *) (looseref->object), tcb->access_context);
            if (rc == HTTP_ACCESS_ABORT) {
                //  Abort the current connection, do not send a response
                smt_thread_raise_exception (thread, access_abort_event);
                break;
            }
            looseref = ipr_looseref_list_next (&looseref);
        }
    </action>

    <action name = "check if persistent connection">
        ipr_meter_count (tcb->server->ometer);
        icl_atomic_inc32 ((volatile qbyte *) &(tcb->server->responses));
        if (tcb->request->persistent)
            the_next_event = persistent_event;
        else
            the_next_event = finished_event;
    </action>

    <!--  SENDING STATIC  -------------------------------------------------->

    <state name = "sending static">
        <event name = "ok">
            <action name = "send bucket to client" />
        </event>
        <event name = "bucket sent">
            <action name = "get next static bucket" />
        </event>
        <event name = "finished" nextstate = "response sent">
            <action name = "close static content" />
            <action name = "do access handlers after" />
            <action name = "check if persistent connection" />
        </event>
    </state>

    <action name = "close static content">
        fclose (tcb->response_stream);
    </action>

    <!--  RESPONSE SENT  --------------------------------------------------->

    <state name = "response sent">
        <event name = "persistent" nextstate = "have http request">
            <action name = "prepare to read header" />
            <action name = "read next http request" />
        </event>
        <event name = "incomplete" nextstate = "have http request">
            <action name = "read next http request" />
        </event>
        <event name = "finished" nextstate = "">
            <action name = "flush the connection" />
            <action name = "do access handlers close" />
        </event>
    </state>

    <!--  DEFAULTS  -------------------------------------------------------->

    <state name = "defaults">
        <event name = "http error" nextstate = "sending dynamic">
            <action name = "format http reply" />
            <action name = "get next dynamic bucket" />
        </event>
        <event name = "socket timeout" nextstate = "">
            <action name = "flush the connection" />
            <action name = "do access handlers close" />
        </event>
        <event name = "access abort">
            <action name = "pause client for abort" />
            <action name = "flush the connection" />
            <action name = "do access handlers close" />
        </event>
        <event name = "access retry" nextstate = "have http request">
            <action name = "pause client for retry" />
            <action name = "do access handlers before" />
            <action name = "check if content complete" />
        </event>
        <event name = "exception" nextstate = "">
            <action name = "flush the connection" />
            <action name = "do access handlers close" />
        </event>
        <event name = "socket error" nextstate = "">
            <action name = "do access handlers close" />
        </event>
        <event name = "socket closing" nextstate = "">
            <action name = "do access handlers close" />
        </event>
        <event name = "smt error" nextstate = "">
            smt_log_print (tcb->alert_log, "E: %s", smt_thread_error (thread));
            <action name = "do access handlers close" />
        </event>
        <event name = "shutdown" nextstate = "">
            <action name = "do access handlers close" />
        </event>
    </state>

    <action name = "pause client for retry">
        //  We wait for a second, then retry access control. This gives time for
        //  whatever is happening in the background to refresh our password data.
        //  The access module is responsible for checking whether the background
        //  process is finished safely, and ending the retry loop
        smt_timer_request_delay (thread, 1 * 1000 * 1000, SMT_NULL_EVENT);
    </action>
</thread>

</agent>

