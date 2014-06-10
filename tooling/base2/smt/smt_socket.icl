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
<class
    name      = "smt_socket"
    comment   = "SMT socket class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    The smt_socket class is used to create socket objects for use with the
    smt_socket_request class.
</doc>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0"/>
</inherit>

<import class = "smt_thread"/>
<import class = "smt_socket_request"/>
<import class = "smt_timer_request"/>

<public name = "header" >
#include "base_apr.h"

#define SMT_SOCKET_VECTOR_MAX        16
</public>

<context>
    size_t       io_size;
    smt_socket_request_by_socket_t
                 *request_list;
    apr_pollfd_t pollfd;
    Bool         trace;                 //  Trace this socket?
</context>

<method name = "new">
    <doc>
    Creates a new socket.  After creating a socket you should set any options
    that your application requires, and then call smt_socket_connect or
    smt_socket_passive as appropriate.
    </doc>
    <local>
    apr_status_t
        status;
    </local>
    status = apr_pool_create (&self->pollfd.p, icl_global_pool);
    if (status == APR_SUCCESS) {
        apr_pool_tag    (self->pollfd.p, "$(selfname)_$(method.name)");
        self->io_size = 0;

        self->request_list = smt_socket_request_by_socket_new ();

        self->pollfd.desc_type   = APR_POLL_SOCKET;
        //  apr_pollset_poll may be implemented using select, in which case we
        //  need to explicitly ask for APR_POLLHUP, APR_POLLERR and APR_POLLNVAL
        self->pollfd.reqevents   = APR_POLLHUP | APR_POLLERR | APR_POLLNVAL;
        self->pollfd.rtnevents   = 0;
        self->pollfd.client_data = self;
        self->trace              = FALSE;
        status = apr_socket_create (&self->pollfd.desc.s,
            APR_INET, SOCK_STREAM, APR_PROTO_TCP, self->pollfd.p);
        if (status != APR_SUCCESS)
            smt_socket_destroy (&self);
    }
    else
        smt_socket_destroy (&self);
</method>

<method name = "destroy">
    <doc>
    Closes an open socket and immediately cancels any requests waiting on
    that socket.

    This method must be used to destroy sockets that are passively listening.

    This method may be used to rapidly close a connected socket.  If a tidy
    close is required, use smt_socket_request_close instead.
    </doc>
    <local>
    smt_socket_request_t
        *socket_request;
    smt_thread_t
        *thread;
    </local>
    //  Clear the socket request list first as we need to activate threads
    //  as we destroy socket requests.
    socket_request = smt_socket_request_by_socket_first (self->request_list);
    while (socket_request) {
        thread = socket_request->thread;
        smt_socket_request_destroy (&socket_request);
        if (thread->pending_count == 0)
            smt_thread_activate (thread);
            
        socket_request = smt_socket_request_by_socket_first (self->request_list);
    }
    smt_socket_request_by_socket_destroy (&self->request_list);
    

    //  Close apr socket if it was opened                                      
    if (self->pollfd.desc.s)
        apr_socket_close (self->pollfd.desc.s);

    //  Destroy apr pool                                                       
    apr_pool_destroy (self->pollfd.p);
</method>

<method name = "connect" template = "function">
    <argument name = "thread"  type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "msecs"   type = "qbyte"          >Timeout for connection</argument>
    <argument name = "host"    type = "const char *"   >Host to connect to</argument>
    <argument name = "service" type = "const char *"   >Service to connect to</argument>
    <argument name = "event"   type = "smt_event_t"    >Event to deliver on connection; SMT_NULL_EVENT for simple thread-blocking</argument>
    <doc>
    Connect to a remote service asychronously.

    Returns SMT_OK immediately if the request was correctly processed; returns
    SMT_SOCKET_ERROR on error.  Note that the socket is not connected
    when the function call returns, but only after the event has been
    delivered.
    </doc>
    <local>
    apr_port_t
        port;
    apr_sockaddr_t
        *sockaddr;
    apr_status_t
        status;
    char
        *endptr;
    smt_socket_request_t
        *request;
    </local>
    //  Look up host address and port number from service name.
    port = (apr_port_t) strtol (service, &endptr, 10);
    if (endptr && !*endptr)             //  Service is numeric                 
        status = apr_sockaddr_info_get (&sockaddr, host, APR_INET, port, 0, self->pollfd.p);
    else {
        status = apr_sockaddr_info_get (&sockaddr, host, APR_INET, 0, 0, self->pollfd.p);
        if (status == APR_SUCCESS)
            status = apr_getservbyname (sockaddr, service);
    }
    if (status == APR_SUCCESS) {
        apr_socket_opt_set (self->pollfd.desc.s, APR_SO_NONBLOCK, 1);
        status = apr_socket_connect (self->pollfd.desc.s, sockaddr);
        if (APR_STATUS_IS_EINPROGRESS (status))
            status = APR_SUCCESS;
    }
    if (status == APR_SUCCESS) {
        request = smt_socket_request_new (thread,
                                          SOCK_CONNECT,
                                          self,
                                          msecs,
                                          SMT_NULL_EVENT,
                                          event);
        if (!request)
            rc = thread->result;
    }
    if (status != APR_SUCCESS)
        rc = SMT_SOCKET_ERROR;
    if (rc != SMT_OK) {
        thread->result = rc;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>

<method name = "passive" template = "function">
    <argument name = "thread"       type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "service"      type = "const char *"   >Service to listen on</argument>
    <argument name = "ipaddress"    type = "const char *"   >IP address to bind to</argument>
    <argument name = "queue_length" type = "int"            >Length of connect queue</argument>
    <doc>
    Setup a socket for passive listening. The service should be a port number
    (as a string) or a well-known TCP/IP service.  The ipaddress can be NULL
    (for "0.0.0.0", meaning "all addresses that this server supports"), or a
    specific IP address, if wanted.  The queue length should be at least 3 and
    as much as 20-30 for very heavily used services.

    Returns SMT_OK immediately if the request was correctly processed;
    returns SMT_SOCKET_ERROR on error.
    </doc>
    <local>
    apr_port_t
        port;
    apr_sockaddr_t
        *sockaddr;
    apr_status_t
        status;
    char
        *endptr;
    </local>
    //  Look up host address and port number from service name.
    port = (apr_port_t) strtol (service, &endptr, 10);
    if (endptr && !*endptr)             //  Service is numeric
        status = apr_sockaddr_info_get (&sockaddr, ipaddress, APR_INET, port, 0, self->pollfd.p);
    else {
        status = apr_sockaddr_info_get (&sockaddr, ipaddress, APR_INET, 0, 0, self->pollfd.p);
        if (status == APR_SUCCESS)
            status = apr_getservbyname (sockaddr, service);
    }
    if (status == APR_SUCCESS) {
        apr_socket_opt_set (self->pollfd.desc.s, APR_SO_NONBLOCK, 1);
#       if (!defined (__WINDOWS__))
        /*  On BSD-socket systems we need to do this to allow the server to
         *  restart on a previously-used socket, without an annoying timeout
         *  of several minutes.  With winsock the reuseaddr option lets the
         *  server work with an already-used socket (!), so we don't do it.
         */
        apr_socket_opt_set (self->pollfd.desc.s, APR_SO_REUSEADDR, 1);
#       endif

        status = apr_socket_bind (self->pollfd.desc.s, sockaddr);
    }
    if (status == APR_SUCCESS)
        status = apr_socket_listen (self->pollfd.desc.s, queue_length);
    if (status != APR_SUCCESS)
        rc = SMT_SOCKET_ERROR;
    if (rc != SMT_OK) {
        thread->result = rc;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>

<method name = "set nodelay" template = "function">
    <doc>
    Sets the value of the TCP NODELAY option on the socket, which controls
    whether or not the Nagle algorithm is used for this socket.
    </doc>
    <argument name = "value" type = "int">Option value</argument>
    <local>
    apr_status_t
        status;
    </local>
    status = apr_socket_opt_set (self->pollfd.desc.s, APR_TCP_NODELAY, value);
    if (status != APR_SUCCESS)
        rc = SMT_SOCKET_ERROR;
</method>

<method name = "set rcvbuf" template = "function">
    <doc>
    Sets the TCP receive buffer size for the socket. Returns the old buffer 
    size, if known. Should be called before calling smt_socket_connect or
    smt_socket_passive on the socket.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <argument name = "new size" type = "int">New buffer size</argument>
    <argument name = "old size" type = "int *">Old buffer size</argument>
    <local>
    apr_status_t
        status = APR_SUCCESS;
    </local>
    if (old_size)
        status = apr_socket_opt_get (self->pollfd.desc.s, APR_SO_RCVBUF, old_size);
    if (status == APR_SUCCESS)
        status = apr_socket_opt_set (self->pollfd.desc.s, APR_SO_RCVBUF, new_size);
    if (status != APR_SUCCESS)
        rc = SMT_SOCKET_ERROR;
</method>

<method name = "set sndbuf" template = "function">
    <doc>
    Sets the TCP send buffer size for the socket. Returns the old buffer 
    size, if known. Should be called before calling smt_socket_connect or
    smt_socket_passive on the socket.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <argument name = "new size" type = "int">New buffer size</argument>
    <argument name = "old size" type = "int *">Old buffer size</argument>
    <local>
    apr_status_t
        status = APR_SUCCESS;
    </local>
    if (old_size)
        status = apr_socket_opt_get (self->pollfd.desc.s, APR_SO_SNDBUF, old_size);
    if (status == APR_SUCCESS)
        status = apr_socket_opt_set (self->pollfd.desc.s, APR_SO_SNDBUF, new_size);
    if (status != APR_SUCCESS)
        rc = SMT_SOCKET_ERROR;
</method>

<method name = "peeraddr" return = "addr" >
    <doc>
    Returns the peer IP address of a connected socket, if any.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <declare  name = "addr" type = "char *"      />
    <local>
        apr_sockaddr_t
            *sockaddr;
    </local>
    apr_socket_addr_get (&sockaddr, APR_REMOTE, self->pollfd.desc.s);
    apr_sockaddr_ip_get (&addr, sockaddr);
</method>

<method name = "trace" template = "function">
    <doc>
    Sets the trace flag for this socket.
    </doc>
    <argument name = "enabled" type = "Bool"/>
    self->trace = enabled;
</method>

<method name = "selftest" inherit = "none" />

</class>
