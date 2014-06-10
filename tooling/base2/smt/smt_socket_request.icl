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
    name      = "smt_socket_request"
    comment   = "SMT socket request class"
    version   = "1.0"
    script    = "icl_gen"
    >

<doc>
    The smt_socket_request class contains all the methods for doing activity
    on a connected socket.  The socket may be connected by smt_socket_connect
    or a slave socket created when by remote connection to a passively opened
    socket.

    You do not typically need to manipulate smt_socket_request objects; once
    created they do the work without further intervention by the application.
</doc>

<option name = "links"  value = "0"/>
<option name = "rwlock"  value = "0"/>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc"   value = "local" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "by_thread"/>
    <option name = "alloc"  value = "cache" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "by_socket"/>
    <option name = "alloc"  value = "cache" />
</inherit>

<import class = "ipr_bucket"/>
<import class = "icl_shortstr"/>
<import class = "smt_os_thread"/>
<import class = "smt_thread"/>
<import class = "smt_socket"/>
<import class = "smt_timer_request"/>

<public name = "header">
typedef enum {
    SOCK_NULL_REQUEST,
    SOCK_ACCEPT,
    SOCK_READ,
    SOCK_CONNECT,
    SOCK_WRITE,
    SOCK_CLOSE,
    SOCK_MONITOR
} smt_socket_request_type_t;

#include "base_apr.h"
</public>

<private name = "header">
//  Macros

#if (defined (__WINDOWS__))
    //  Under windows, CTRL-C does not generate a SIGINT but must be           
    //  trapped differently.  In particular it does not interrupt a            
    //  select().  To get around this we never block for more than a           
    //  certain period at a time, so that we can test for a raised signal.     
#   define SMT_MAX_TIMEOUT   100       //  Check for signal every 100us        
#endif

#define REQUEST_IS_INPUT(r)  (r->type == SOCK_READ     \
                           || r->type == SOCK_ACCEPT)
#define REQUEST_IS_OUTPUT(r) (r->type == SOCK_CONNECT  \
                           || r->type == SOCK_WRITE    \
                           || r->type == SOCK_CLOSE)

//  Static variables

static Bool
    s_smt_socket_request_trace = FALSE;
static volatile qbyte
    s_desc_array_start = 0;             //  Start differently each time

//  Request names

static char *request_type_name [] = {
    "NULL",
    "Accept",
    "Input",
    "Read",
    "Connect",
    "Output",
    "Write",
    "Close",
    "Monitor"
};

#define REQUEST_TYPE_NAME(t)                                                 \
            ((t < sizeof (request_type_name) / sizeof (char *))              \
             ? request_type_name [t] : "ILLEGAL")

//  Function prototypes

#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_SOCKET_REQUEST))
static void
    s_dump_trace_read                   (icl_os_thread_t thread, apr_time_t time, qbyte info);
static void
    s_dump_trace_write                  (icl_os_thread_t thread, apr_time_t time, qbyte info);
#endif
static void
    s_dump_buffer                       (smt_thread_id_t id, void *buffer, size_t size);
</private>

<context>
    smt_thread_t         *thread;       //  Thread to reply to                 
    smt_os_thread_t      *os_thread;    //  OS thread holding pollset          
    smt_socket_request_type_t
                         type;          //  Type of request                    
    smt_event_t          input_event;   //  Event to send on completion        
    smt_event_t          output_event;  //  Event to send on completion        
    smt_socket_t         *socket;       //  Socket for request                 
    smt_socket_t         **socket_p;    //  Pointer for accept/close           
    byte                 *buffer;       //  i/o buffer                         
    size_t               max_size;      //  Maximum size of buffer             
    size_t               min_size;      //  Minimum data to process
    size_t               cur_size;      //  Minimum data to process
    qbyte                msecs;         //  Timeout for this request
    smt_timer_request_t  *timer_request;//  Corresponding timer                
    ipr_bucket_t         *bucket;       //  Bucket used as buffer
    ipr_mother_t         *mother;       //  Mother object for no-copy read
    ipr_bucket_t         **slave_p;     //  Pointer to slave bucket to allocate
</context>

<method name = "initialise" private = "0"/>

<method name = "new">
    <argument name = "thread"       type = "smt_thread_t *"            />
    <argument name = "type"         type = "smt_socket_request_type_t" />
    <argument name = "socket"       type = "smt_socket_t *"            />
    <argument name = "msecs"        type = "qbyte"                     />
    <argument name = "input_event"  type = "smt_event_t"               />
    <argument name = "output_event" type = "smt_event_t"               />
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <animate  name = "input-event"  value = "thread->event_name[input_event]"  format = "%s"/>
    <animate  name = "output-event" value = "thread->event_name[output_event]" format = "%s"/>
    <doc>
    This method is for internal use only.
    </doc>
    <local>
    smt_socket_request_t
        *first_request,
        *request;
    Bool
        disallow,
        write_request;
    apr_int16_t
        reqevents;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif
    
    self->thread        = thread;
    self->os_thread     = thread->os_thread;
    self->type          = type;
    self->input_event   = input_event;
    self->output_event  = output_event;
    self->socket        = socket;
    self->socket_p      = NULL;
    self->buffer        = NULL;
    self->max_size      = 0;
    self->min_size      = 0;
    self->cur_size      = 0;
    self->msecs         = msecs;
    self->timer_request = NULL;
    self->bucket        = NULL;
    self->mother        = NULL;
    self->slave_p       = NULL;
    
    smt_socket_request_by_thread_queue (thread->socket_request_list, self);

    //  Find insertion point for new request.
    first_request = smt_socket_request_by_socket_first (socket->request_list);
    request  = first_request;
    disallow = FALSE;
    write_request = (thread->write_request == NULL && self->type == SOCK_WRITE);
    while (request) {
        if (REQUEST_IS_INPUT (self))
            disallow =  disallow
                     || request->type == SOCK_CLOSE;
        else
        if (REQUEST_IS_OUTPUT (self)) {
            if (REQUEST_IS_OUTPUT (request))
                write_request = FALSE;
            disallow =  disallow
                     || request->type == SOCK_CLOSE;
            if (REQUEST_IS_INPUT (request))
                break;
        }
        else
            disallow = TRUE;        //  Monitor must be only request       
            
        if (disallow)
            break;

        request = smt_socket_request_by_socket_next (&request);
    }
    if (disallow) {
        thread->result = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Illegal sequence: %s request follows %s request.",
                           REQUEST_TYPE_NAME (self->type),
                           REQUEST_TYPE_NAME (request->type));
        
        // Remove from lists and free.
        smt_socket_request_remove_from_all_containers (self);
        smt_socket_request_free (self);
        self = NULL;
    }
    else {
        if (write_request)
            thread->write_request = self;
            
        if (request)
            smt_socket_request_by_socket_relink_before (self, request);
        else
            smt_socket_request_by_socket_queue (socket->request_list,
                                                self);
        //  Modify requested poll events only if they have changed, in which 
        //  case also create a timer request.
        reqevents = socket->pollfd.reqevents
                  |  ((REQUEST_IS_INPUT (self)
                  ||  input_event != SMT_NULL_EVENT)
                   ?  APR_POLLIN :  0)
                  |  ((REQUEST_IS_OUTPUT (self)
                  ||  output_event != SMT_NULL_EVENT) 
                   ?  APR_POLLOUT : 0);
        if (!first_request) {
            socket->pollfd.reqevents = reqevents;
            icl_apr_assert (apr_pollset_add (thread->os_thread->pollset, &socket->pollfd));
            smt_socket_request_timer (self);
        }
        else
            if (reqevents != socket->pollfd.reqevents) {
                icl_apr_assert (apr_pollset_remove (thread->os_thread->pollset, &socket->pollfd));
                socket->pollfd.reqevents = reqevents;
                icl_apr_assert (apr_pollset_add (thread->os_thread->pollset, &socket->pollfd));
                smt_socket_request_timer (self);
            }
            
        //  Adjust pending count.                                          
        if ((REQUEST_IS_INPUT  (self) && self->input_event  == SMT_NULL_EVENT)
        ||  (REQUEST_IS_OUTPUT (self) && self->output_event == SMT_NULL_EVENT))
            thread->pending_count++;
                
        thread->os_thread->socket_request_count++;
    }
</method>

<method name = "timer">
    <argument name = "self" type = "$(selftype) *">The request</argument>
    <doc>
    Create a timer request for a socket request.  The timeout is in the 'msecs'
    field of the socket request.
    </doc>
    if (self->msecs) {
        //  Cancel existing timer request
        if (self->timer_request)
            smt_timer_request_destroy (&self->timer_request);

        self->timer_request = smt_timer_request_new (self->thread->os_thread,
                                                     smt_time_now () + 1000 * self->msecs,
                                                     SMT_NULL_EVENT);
        self->timer_request->thread = self->thread;
        self->timer_request->socket_request = self;
    }
</method>

<method name = "accept" return = "request">
    <argument name = "thread"  type = "smt_thread_t *"  >Thread to deliver reply to</argument>
    <argument name = "socket"  type = "smt_socket_t *"  >Passively opened socket to accept connection</argument>
    <argument name = "msecs"   type = "qbyte"           >Timeout for connection</argument>
    <argument name = "event"   type = "smt_event_t"     >Event to deliver on connection; SMT_NULL_EVENT for simple thread-blocking</argument>
    <argument name = "connect" type = "smt_socket_t **" >Pointer to result of passive connection</argument>
    <animate  name = "event"   value = "thread->event_name[event]"  format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Accepts a connection on a socket that was opened passively.

    On connection, the event is delivered, and/or the thread is woken, and
    an smt_socket object it put into *connect.
    </doc>
    <local>
    int
        rc = SMT_OK;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    if (socket->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket accept request on closed socket.");
    }
    else {
        request = smt_socket_request_new (thread,
                                          SOCK_ACCEPT,
                                          socket,
                                          msecs,
                                          event,
                                          SMT_NULL_EVENT);
        if (request)
            request->socket_p = connect;
        else
            rc = thread->result;
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "destroy public" condition = "0"/>

<method name = "destroy" inherit = "none">
    <argument name = "self" type = "$(selftype) *" ref = "1">Reference to object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number</argument>
    <doc>
    The purge method is for internal use only.  It may only be called from the
    OS thread that is managing the SMT thread.
    
    It destroys a socket request but does not wake a sleeping thread.
    </doc>
    <local>
    smt_socket_request_t
        *self_link,
        *next_request;
    apr_int16_t
        reqevents;
    </local>
    <!--  This method is done entirely manually, rather than through
          inheritence because it needs to do all the pollset stuff before
          removing the request from its containers.                        -->
    
    if (self) {

#if (defined (BASE_THREADSAFE) && defined (DEBUG))
        assert (apr_os_thread_current () == self->os_thread->apr_os_thread);
#endif
    
        //  Modify requested poll events only if they have changed. Since  
        //  apr complains if we close a socket before removing the poll    
        //  request, socket close is handled separately.                   
        if (self->type == SOCK_MONITOR
        ||  self->type == SOCK_CLOSE) {
            if (self->socket->pollfd.reqevents & (APR_POLLIN | APR_POLLOUT)) {
                icl_apr_assert (apr_pollset_remove (self->os_thread->pollset, &self->socket->pollfd));
                //  JS  Could probably make a more precise test here.
                if (self->type == SOCK_MONITOR)
                    apr_pollset_remove (self->os_thread->waking_pollset, &self->socket->pollfd);
                self->socket->pollfd.reqevents &= ~ (APR_POLLIN | APR_POLLOUT);
            }
        }
        else {
            reqevents = self->socket->pollfd.reqevents;
    
            self_link = self;
            next_request = smt_socket_request_by_socket_next (&self_link);

            //  Modify pollsets and activate timer for succeeding request
            //  if there is one.                
            if (REQUEST_IS_INPUT (self)) {
                if (! next_request)
                    reqevents &= ~APR_POLLIN;
                else
                    smt_socket_request_timer (next_request);
            }
            else
            if (REQUEST_IS_OUTPUT (self)) {
                if ((! next_request)
                ||  REQUEST_IS_INPUT (next_request))
                    reqevents &= ~APR_POLLOUT;
                else
                    smt_socket_request_timer (next_request);
            }
            
            if (reqevents != self->socket->pollfd.reqevents) {
                icl_apr_assert (apr_pollset_remove (self->os_thread->pollset, &self->socket->pollfd));
                self->socket->pollfd.reqevents = reqevents;
                if (reqevents & (APR_POLLIN | APR_POLLOUT)) {
                    icl_apr_assert (apr_pollset_add (self->os_thread->pollset, &self->socket->pollfd));
                }
            }
        }
        
        //  Destroy associated timer request if there is one.                      
        if (self->timer_request)
            smt_timer_request_destroy (&self->timer_request);

        self->os_thread->socket_request_count--;

        if ((REQUEST_IS_INPUT  (self) && self->input_event  == SMT_NULL_EVENT)
        ||  (REQUEST_IS_OUTPUT (self) && self->output_event == SMT_NULL_EVENT)) {
            assert (self->thread->pending_count);
            self->thread->pending_count--;
        }
            
        // Remove from lists and free.
        smt_socket_request_remove_from_all_containers (self);
        smt_socket_request_free (self);
        *self_p = NULL;
    }
</method>

<method name = "free">
    ipr_bucket_unlink (&self->bucket);
</method>

<method name = "return">
    <argument name = "self"   type = "smt_socket_request_t *" ref = "1" />
    <argument name = "result" type = "int"                    />
    <argument name = "error"  type = "int"                    />
    <doc>
    Socket request returns the given value and is destroyed.
    
    This method is for internal use only.
    </doc>
    <local>
    smt_event_t
        event;
    smt_thread_t
        *thread;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == self->os_thread->apr_os_thread);
#endif

    event = REQUEST_IS_INPUT (self)
          ? self->input_event
          : REQUEST_IS_OUTPUT (self)
          ? self->output_event 
          : SMT_NULL_EVENT;
    
    thread = self->thread;
    
    //  Wake up thread if self was last pending i/o request.                   
    if (event == SMT_NULL_EVENT) {
        self->thread->result = result;
        self->thread->error = error;
        if (self->thread->result != SMT_OK)
            self->thread->catcher (thread);
        if (self->type == SOCK_MONITOR)
            self->thread->monitor = NULL;
            
        smt_socket_request_destroy (self_p);
        if (thread->pending_count == 0)
            smt_thread_activate (thread);
    }
    else {
        //  Error code may be overwritten by subsequent errors, but we have
        //  no other way to communicate it to the thread.
        thread->error = error;
        smt_socket_request_destroy (self_p);
        smt_method_send (thread->reply_queue,
                         event,
                         SMT_PRIORITY_NORMAL,
                         result,
                         NULL,
                         NULL);
    }
</method>

<method name = "monitor" return = "request">
    <argument name = "thread"       type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "socket"       type = "smt_socket_t *" >Connected socket to monitor</argument>
    <argument name = "msecs"        type = "qbyte"          >Timeout</argument>
    <argument name = "input_event"  type = "smt_event_t"    >Event to deliver on socket ready for input</argument>
    <argument name = "output_event" type = "smt_event_t"    >Event to deliver on socket ready for output</argument>
    <animate  name = "input-event"  value = "thread->event_name[input_event]"  format = "%s"/>
    <animate  name = "output-event" value = "thread->event_name[output_event]" format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Starts monitoring a socket, superseding any existing monitor by the thread.
    Monitor is cancelled by socket activity or any other event being delivered.
    
    This method is used to monitor a socket for activity while simultaneously
    accepting events on the event queue.  It monitors the socket for input 
    and/or output depending on whether 'input' or 'output' has a value other
    than SMT_NULL_EVENT.
    </doc>
    <local>
    int
        rc = SMT_OK;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    assert (thread->status == SMT_THREAD_ACTIVE);
    if (!smt_socket_request_by_thread_isempty (thread->socket_request_list)) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket monitor request while another socket request active.");
    }
    else
    if (socket->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket monitor request on closed socket.");
    }
    else {
        request = smt_socket_request_new (thread,
                                          SOCK_MONITOR,
                                          socket,
                                          msecs,
                                          input_event,
                                          output_event);
        if (request)
            thread->monitor = request;
        else
            rc = thread->result;
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        thread->catcher (thread);
    }
</method>

<method name = "read" return = "request">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "socket"   type = "smt_socket_t *" >Connected socket to read data from</argument>
    <argument name = "msecs"    type = "qbyte"          >Timeout for socket to be ready</argument>
    <argument name = "min_size" type = "size_t"         >Minimum data size to read</argument>
    <argument name = "max_size" type = "size_t"         >Maximum data size to read</argument>
    <argument name = "buffer"   type = "byte *"         >Buffer to store data read</argument>
    <argument name = "event"    type = "smt_event_t"    >Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <animate  name = "event"   value = "thread->event_name[event]"  format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Reads data from a connected socket.  Delivers an event and/or
    wakes the thread when the data are read.
    </doc>
    <local>
    int
        rc = SMT_OK;
    apr_sockaddr_t
        *sockaddr;
    char
        *addr;
    size_t
        cur_size = 0;
    apr_status_t
        status;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    if (thread->monitor) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket read request while socket monitor is active.");
    }
    else
    if (socket->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket read request on closed socket.");
    }
    else {
        socket->io_size = 0;
        //  Read immediately if possible    
        if ((socket->pollfd.reqevents & APR_POLLIN) == 0) {
            cur_size = max_size;
            status = apr_socket_recv (socket->pollfd.desc.s, (char *) buffer, &cur_size);
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_SOCKET_REQUEST))
            icl_trace_record (NULL, s_dump_trace_read, cur_size);
#endif
            if ((s_smt_socket_request_trace || socket->trace) && cur_size > 0) {
                apr_socket_addr_get (&sockaddr, APR_REMOTE, socket->pollfd.desc.s);
                apr_sockaddr_ip_get (&addr, sockaddr);
                icl_console_print ("I: read %d bytes from %s req: %d bytes", cur_size, addr, max_size);
                s_dump_buffer (thread->id, buffer, cur_size);
            }
            if (status == APR_SUCCESS) {
                if (cur_size >= min_size) {
                    socket->io_size = cur_size;
                    thread->result = SMT_OK;
                    if (event != SMT_NULL_EVENT)
                        smt_thread_set_next_event (thread, event);
                }
            }
            else
            if (APR_STATUS_IS_EOF (status)) {
                rc = SMT_SOCKET_CLOSING;
                thread->error = status;
            }
            else
            if (! APR_STATUS_IS_EAGAIN (status)) {
                rc = SMT_SOCKET_ERROR;
                thread->error = status;
                smt_socket_request_reply_error_to_all_requests (socket, SMT_SOCKET_ERROR, status);
            }
        }
        if (cur_size < min_size
        &&  rc == SMT_OK) {
            request = smt_socket_request_new (thread,
                                              SOCK_READ,
                                              socket,
                                              msecs,
                                              event,
                                              SMT_NULL_EVENT);
            if (request) {
                request->max_size = max_size;
                request->min_size = min_size;
                request->cur_size = cur_size;
                request->buffer   = buffer;
            }
            else
                rc = thread->result;
        }
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "read bucket" return = "request">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "socket"   type = "smt_socket_t *" >Connected socket to read data from</argument>
    <argument name = "msecs"    type = "qbyte"          >Timeout for socket to be ready</argument>
    <argument name = "min_size" type = "size_t"         >Minimum data size to read</argument>
    <argument name = "max_size" type = "size_t"         >Maximum data size to read</argument>
    <argument name = "bucket_p" type = "ipr_bucket_t **">Pointer to created bucket</argument>
    <argument name = "event"    type = "smt_event_t"    >
        Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <animate  name = "event"   value = "thread->event_name[event]"  format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Reads data from a connected socket.  Delivers an event and/or
    wakes the thread when the data are read.
    </doc>
    <local>
    int
        rc = SMT_OK;
    ipr_bucket_t
        *bucket;
    apr_sockaddr_t
        *sockaddr;
    char
        *addr;
    size_t
        cur_size = 0;
    apr_status_t
        status;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    if (thread->monitor) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket read bucket request while socket monitor is active.");
    }
    else
    if (max_size > IPR_BUCKET_MAX_SIZE) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Maximum data size too big in smt_socket_request_read_buffer");
    }
    else
    if (socket->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket read bucket request on closed socket.");
    }
    else {
        bucket = ipr_bucket_new_ (file, line, max_size);
        *bucket_p = bucket;
        socket->io_size = 0;
        //  Read immediately if possible    
        if ((socket->pollfd.reqevents & APR_POLLIN) == 0) {
            cur_size = max_size;
            status = apr_socket_recv (socket->pollfd.desc.s, (char *) bucket->data, &cur_size);
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_SOCKET_REQUEST))
            icl_trace_record (NULL, s_dump_trace_read, cur_size);
#endif
            if ((s_smt_socket_request_trace || socket->trace) && cur_size > 0) {
                apr_socket_addr_get (&sockaddr, APR_REMOTE, socket->pollfd.desc.s);
                apr_sockaddr_ip_get (&addr, sockaddr);
                icl_console_print ("I: read %d bytes from %s req: %d bytes", cur_size, addr, max_size);
                s_dump_buffer (thread->id, bucket->data, cur_size);
            }
            if (status == APR_SUCCESS) {
                if (cur_size >= min_size) {
                    socket->io_size = cur_size;
                    thread->result = SMT_OK;
                    if (event != SMT_NULL_EVENT)
                        smt_thread_set_next_event (thread, event);
                }
            }
            else
            if (APR_STATUS_IS_EOF (status)) {
                rc = SMT_SOCKET_CLOSING;
                thread->error = status;
            }
            else
            if (! APR_STATUS_IS_EAGAIN (status)) {
                rc = SMT_SOCKET_ERROR;
                thread->error = status;
                smt_socket_request_reply_error_to_all_requests (socket, SMT_SOCKET_ERROR, status);
            }
        }
        if (cur_size < min_size
        &&  rc == SMT_OK) {
            request = smt_socket_request_new (thread,
                                              SOCK_READ,
                                              socket,
                                              msecs,
                                              event,
                                              SMT_NULL_EVENT);
            if (request) {
                request->max_size = max_size;
                request->min_size = min_size;
                request->cur_size = cur_size;
                request->buffer   = bucket->data;
                request->bucket   = bucket;
                ipr_bucket_link (bucket);
            }
            else
                rc = thread->result;
        }
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        ipr_bucket_unlink (bucket_p);
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "read mother" return = "request">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "socket"   type = "smt_socket_t *" >Connected socket to read data from</argument>
    <argument name = "msecs"    type = "qbyte"          >Timeout for socket to be ready</argument>
    <argument name = "size"     type = "size_t"         >Data size to return</argument>
    <argument name = "mother"   type = "ipr_mother_t *" ref = "1">Pointer to mother</argument>
    <argument name = "slave_p"  type = "ipr_bucket_t **">Pointer to slave bucket to create</argument>
    <argument name = "event"    type = "smt_event_t"    >
        Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <animate  name = "event"   value = "thread->event_name[event]"  format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Reads data from a connected socket using a mother object live data as a no-
    copy buffer.  If the mother pointer is NULL, a mother object is created.
    Returns a created slave bucket holding the requested data.
    
    Delivers an event and/or wakes the thread when the data are read.
    </doc>
    <local>
    int
        rc = SMT_OK;
    byte
        *buffer;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    if (thread->monitor) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket read bucket using mother request while socket monitor is active.");
    }
    else
    if (size > IPR_BUCKET_MAX_SIZE) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Maximum data size too big in smt_socket_request_read_bucket_using_mother");
    }
    else
    if (socket->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket read bucket using mother request on closed socket.");
    }
    else {
        if (!mother) {
            mother = ipr_mother_new ();
            *mother_p = mother;
        }
        buffer = ipr_mother_allocate (&mother, size);
        *slave_p = ipr_mother_claim (mother, size);
        if (*slave_p) {
            if (s_smt_socket_request_trace || socket->trace) {
                icl_console_print ("Returning %d bytes:", size);
                s_dump_buffer (thread->id, (*slave_p)->data, size);
            }
            thread->result = SMT_OK;
            if (event != SMT_NULL_EVENT)
                smt_thread_set_next_event (thread, event);
        }
        else {
            //  If not enough data already available, create a socket request.

            socket->io_size = 0;
            
            request = smt_socket_request_new (thread,
                                              SOCK_READ,
                                              socket,
                                              msecs,
                                              event,
                                              SMT_NULL_EVENT);
            if (request) {
                request->buffer   = buffer;
                *mother_p = mother;
                request->max_size = size;
                request->min_size = size;
                request->cur_size = ipr_mother_overhang (mother);;
                request->mother   = mother;
                request->slave_p  = slave_p;
            }
            else
                rc = thread->result;
        }
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "write" return = "request">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "socket"   type = "smt_socket_t *" >Connected socket to write data to</argument>
    <argument name = "msecs"    type = "qbyte"          >Timeout for socket to be ready</argument>
    <argument name = "size"     type = "size_t"         >Data size to write</argument>
    <argument name = "buffer"   type = "byte *"         >Buffer holding data to write</argument>
    <argument name = "event"    type = "smt_event_t"    >
        Event to deliver on write complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <animate  name = "event"    value = "thread->event_name[event]"  format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Writes data to a connected socket.  Delivers an event and/or
    wakes the thread when the data are written.
    </doc>
    <local>
    int
        rc = SMT_OK;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    if (thread->monitor) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket write request while socket monitor is active.");
    }
    else
    if (socket->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket write request on closed socket.");
    }
    else
    if (size == 0) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: buffer size is zero in smt_socket_request_write");
    }
    
    if (rc == SMT_OK) {
        request = smt_socket_request_new (thread,
                                          SOCK_WRITE,
                                          socket,
                                          msecs,
                                          SMT_NULL_EVENT,
                                          event);
        if (request) {
            request->max_size = size;
            request->min_size = size;
            request->cur_size = 0;
            request->buffer   = buffer;
        }
        else
            rc = thread->result;
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "write bucket" return = "request">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "socket"   type = "smt_socket_t *" >Connected socket to write data to</argument>
    <argument name = "msecs"    type = "qbyte"          >Timeout for socket to be ready</argument>
    <argument name = "bucket"   type = "ipr_bucket_t *" >Data in a bucket</argument>
    <argument name = "event"    type = "smt_event_t"    >
        Event to deliver on write complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <animate  name = "event"    value = "thread->event_name[event]"  format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Writes data to a connected socket from a bucket.  Delivers an event and/or
    wakes the thread when the data are written.  The bucket is also destroyed
    at this time.
    </doc>
    <local>
    int
        rc = SMT_OK;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    if (thread->monitor) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket write bucket request while socket monitor is active.");
    }
    else
    if (socket->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket write bucket request on closed socket.");
    }
    else
    if (bucket->cur_size == 0) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: buffer size is zero in smt_socket_request_write_bucket");
    }
    
    if (rc == SMT_OK) {
        request = smt_socket_request_new (thread,
                                          SOCK_WRITE,
                                          socket,
                                          msecs,
                                          SMT_NULL_EVENT,
                                          event);
        if (request) {
            request->max_size = bucket->cur_size;
            request->min_size = bucket->cur_size;
            request->cur_size = 0;
            request->buffer   = bucket->data;
            request->bucket   = bucket;
            ipr_bucket_link (bucket);
        }
        else
            rc = thread->result;
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "close" return = "request">
    <argument name = "thread"   type = "smt_thread_t *"  >Thread to deliver reply to</argument>
    <argument name = "socket"   type = "smt_socket_t **" >Connected socket to close</argument>
    <argument name = "msecs"    type = "qbyte"           >Timeout for socket to be ready</argument>
    <argument name = "event"    type = "smt_event_t"     >Event to deliver on close complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <animate  name = "event"    value = "thread->event_name[event]"  format = "%s"/>
    <declare  name = "request" type = "smt_socket_request_t *" default = "NULL">The socket request</declare>
    <doc>
    Wait for any pending activity to be complete, then close the socket.
    Delivers an event and/or wakes the thread when the data are read.
    </doc>
    <local>
    int
        rc = SMT_OK;
    apr_status_t
        status = APR_EAGAIN;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == thread->os_thread->apr_os_thread);
#endif

    if (thread->monitor) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket close request while socket monitor is active.");
    }
    else
    if ((*socket)->pollfd.desc.s == NULL) {
        rc = SMT_ILLEGAL_ARGUMENT;
        icl_console_print ("E: Attempt to make socket close request on closed socket.");
    }
    else
    //  Close immediately if possible    
    if (((*socket)->pollfd.reqevents & APR_POLLOUT) == 0) {
        //  Since APR insists that poll requests be removed before socket is   
        //  closed, we remove them now, then re-add if close unsuccessful.     
        if ((*socket)->pollfd.reqevents & (APR_POLLIN | APR_POLLOUT)) {
            icl_apr_assert (apr_pollset_remove (thread->os_thread->pollset, &(*socket)->pollfd));
            (*socket)->pollfd.reqevents &= ~ (APR_POLLIN | APR_POLLOUT);
        }
        status = apr_socket_close ((*socket)->pollfd.desc.s);
        if (status == APR_SUCCESS) {
            //  Set APR socket to NULL to avoid repeated closing               
            (*socket)->pollfd.desc.s = NULL;

            thread->result = SMT_OK;
            //  Find any other requests on the same socket    
            smt_socket_request_reply_error_to_all_requests (*socket, SMT_SOCKET_ERROR, APR_EPIPE);
            
            //  Finally destroy the original socket.  Do this last as it       
            //  cancels other requests on the same socket.                     
            smt_socket_destroy (socket);
        }
        else
        if (! APR_STATUS_IS_EAGAIN (status)) {
            rc = SMT_SOCKET_ERROR;
            thread->error = status;
        }
    }
    if (rc == SMT_OK
    &&  APR_STATUS_IS_EAGAIN (status)) {
        request = smt_socket_request_new (thread,
                                          SOCK_CLOSE,
                                          *socket,
                                          msecs,
                                          SMT_NULL_EVENT,
                                          event);
        if (request) {
            request->socket_p = socket;
        }
        else
            rc = thread->result;
    }
    if (rc != SMT_OK) {
        thread->result = rc;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "poll" return = "rc">
    <argument name = "os_thread" type = "smt_os_thread_t *"/>
    <argument name = "force"     type = "Bool">Force poll of all sockets?</argument>
    <declare  name = "rc"        type = "int" default = "SMT_OK" />
    <doc>
    This method checks for socket activity without blocking.
    Returns SMT_OK or SMT_FINISHED if no socket activity is awaited.
    
    This method is for internal use only.
    </doc>
    <local>
    apr_time_t
        time_now;
    apr_status_t
        status;
    apr_int32_t
        num_descs;
    apr_pollfd_t
        *descs;
    smt_thread_t
        *thread;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == os_thread->apr_os_thread);
#endif

    //  Check for expired timers                                               
    time_now = smt_time_now ();
    smt_timer_request_expire_all (os_thread, time_now);

    //  Two types of polling:
    //      1. Regular polling of entire pollset, done whenever a thread
    //         manager is about to be called for a 2nd time since last poll.
    //      2. Polling of pollset of waking threads only; each iteration.
    //         This is OK since waking_threads are all cleared on poll.
    
    num_descs = 0;
    status = APR_SUCCESS;
    if (force)
        status = apr_pollset_poll (os_thread->pollset, 0, &num_descs, (const apr_pollfd_t **) &descs);
    else
    if (!smt_thread_by_status_isempty (os_thread->waking_list))
        status = apr_pollset_poll (os_thread->waking_pollset, 0, &num_descs, (const apr_pollfd_t **) &descs);
    
    if (status == APR_SUCCESS
    ||  APR_STATUS_IS_TIMEUP (status)
    ||  APR_STATUS_IS_EINTR  (status)) {
        if (num_descs > 0)
            smt_socket_request_process_active_sockets (os_thread, num_descs, descs);
    }
    else {
        icl_apr_assert (status);
        rc = SMT_SELECT_ERROR;
    }

    thread = smt_thread_by_status_pop (os_thread->waking_list);
    while (thread) {
        //  JS: Sanity check cf AMQ-448
        assert (smt_thread_activity (thread));
        
        smt_socket_request_destroy (&thread->monitor);
        if (thread->pending_count == 0)
            smt_thread_activate (thread);
        
        thread = smt_thread_by_status_pop (os_thread->waking_list);
    }
</method>

<method name = "wait" return = "rc">
    <argument name = "os_thread" type = "smt_os_thread_t *"/>
    <declare  name = "rc"        type = "int" default = "SMT_OK" />
    <doc>
    This method checks for socket activity, blocking if required.

    Returns SMT_OK or SMT_SELECT_ERROR if something serious happened.

    This method is for internal use only.
    </doc>
    <local>
    apr_time_t
        time_now,
        time_expire;
    apr_interval_time_t
        timeout = 0;
    apr_int32_t
        num_descs;
    apr_pollfd_t
        *descs;
    apr_status_t
        status;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == os_thread->apr_os_thread);
#endif

    //  Check for expired timers.  If any are expired, skip rest of call.      
    time_now = smt_time_now ();
    if (smt_timer_request_expire_all (os_thread, time_now) == 0) {

#if (defined (__WINDOWS__))
    //  Start of the SIGINT checking loop.                                     
    while (TRUE) {
#endif
        //  Calculate time until next timer expires                            
        time_expire = smt_timer_request_first (os_thread);
        if (time_expire != 0)
            timeout = time_expire - time_now;
        else
            timeout = -1;               //  Block indefinitely.                
        
        if (rc == SMT_OK) {
#if (defined (__WINDOWS__))
            //  Shorten the timeout to SMT_MAX_TIMEOUT                         
            if (timeout < 0
            ||  timeout > SMT_MAX_TIMEOUT)
                timeout = SMT_MAX_TIMEOUT;
#endif

#if (defined (BASE_THREADSAFE))
            status = apr_pollset_poll (os_thread->pollset, timeout, &num_descs, (const apr_pollfd_t **) &descs);
#else
            if (os_thread->socket_request_count)
                status = apr_pollset_poll (os_thread->pollset, timeout, &num_descs, (const apr_pollfd_t **) &descs);
            else {
                //  If we are single-threaded, polling is no good here         
                apr_sleep (timeout);
                status = APR_TIMEUP;
            }
#endif

            if (status == APR_SUCCESS) {
                if (num_descs > 0) {
                    smt_socket_request_process_active_sockets (os_thread, num_descs, descs);
#if (defined (__WINDOWS__))
                    //  Break from the loop if there was socket activity.      
                    break;
#endif
                }
            }
            else
            if (! (APR_STATUS_IS_TIMEUP (status)
            ||     APR_STATUS_IS_EINTR  (status))) {
                icl_apr_assert (status);
                rc = SMT_SELECT_ERROR;
            }
        }

#if (defined (__WINDOWS__))
        //  All this rigmarole is so that we can perform the following test.   
        if (rc != SMT_OK
        ||  smt_signal_raised)
            break;

        //  Get the time now.  Also expire any requests, in order to avoid     
        //  a situation where the system clock ticks over at just the wrong    
        //  moment and we wind up with a negative timeout.                     
        time_now = smt_time_now ();
        if (smt_timer_request_expire_all (os_thread, time_now) != 0)
            break;
    }
#endif
    }
</method>

<method name = "trace">
    <argument name = "enabled" type = "Bool"/>
    s_smt_socket_request_trace = enabled;
</method>

<method name = "selftest" inherit = "none" />

<method name = "process active sockets" private = "1">
    <argument name = "os_thread" type = "smt_os_thread_t *">The OS thread</argument>
    <argument name = "num_descs" type = "apr_int32_t">The number of descriptors in the set</argument>
    <argument name = "descs"     type = "apr_pollfd_t *">The array of descriptors</argument>
    <doc>
    Process the requests associated with an array of descriptors returned from a
    poll.
    </doc>
    <local>
    apr_uint32_t
        desc_start,
        desc_index;
    apr_pollfd_t
        *desc;
    smt_socket_t
        *socket;
    smt_thread_t
        *thread;
#if (defined (BASE_THREADSAFE))
    char
        ch;
#    if (defined (__WINDOWS__))
    size_t
        nbytes;
#   endif
#endif
    </local>
    smt_socket_request_t
        *request,
        *next_request;
    apr_status_t
        status,                         //  Status from apt_sock_opt_get
        error = 0;                      //  Socket error, if any 

    //  We start at a different point in the array each time to avoid
    //  consistently favouring the same socket.
    desc_start = icl_atomic_inc32 (&s_desc_array_start);
    for (desc_index = 0; desc_index < (apr_uint32_t) num_descs; desc_index++) {
        desc   = &descs [(desc_start + desc_index) % (apr_uint32_t) num_descs];
        socket = desc->client_data;
#if (defined (BASE_THREADSAFE))
        //  If socket is NULL then this is activity on thread pipe.  Still     
        //  process socket activity first.                                     
        if (!socket) {
#    if (defined (__WINDOWS__))
            nbytes = 1;
            icl_apr_assert (apr_socket_recv (os_thread->sock_in, &ch, &nbytes));
            assert (nbytes == 1);
    
#    else
            icl_apr_assert (apr_file_getc (&ch, os_thread->pipe_in));
#    endif
        }
        else {
#endif
            //  APR_POLLNVAL means the socket is invalid.
            if (desc->rtnevents & APR_POLLNVAL)
                smt_socket_request_reply_error_to_all_requests (socket, SMT_ILLEGAL_ARGUMENT, 0);
            else {
                request = smt_socket_request_by_socket_first (socket->request_list);
                while (request) {
                    assert (request->os_thread == os_thread);
                    
                    //  Sanity check
                    assert (request->type == SOCK_MONITOR || request->thread->monitor == NULL);
                    assert (request->type != SOCK_MONITOR || request->thread->status == SMT_THREAD_SLEEPING
                                                          || request->thread->status == SMT_THREAD_POLLING
                                                          || request->thread->status == SMT_THREAD_WAKING);
                  
                    switch (request->type) {
                        case SOCK_NULL_REQUEST:
                            assert (0);
                            break;
                        case SOCK_ACCEPT:
                            if (desc->rtnevents & APR_POLLIN) {
                                next_request = request;
                                next_request = smt_socket_request_by_socket_next (&next_request);
                                if (!smt_socket_request_accept_connection (request))
                                    //  Aborted connection, restart
                                    request = NULL;
                                else
                                    request = next_request;
                            }
                            else
                            //  Should not occur, but handle anyway just in case
                            if (desc->rtnevents & APR_POLLERR) {
                                //  If APR_POLLERR is set, get error code.
                                status = apr_socket_opt_get (socket->pollfd.desc.s, APR_SO_ERROR, &error);
                                //  Some implementations return error from getsockopt instead
                                if (status != APR_SUCCESS && !error)
                                    error = status;
                                smt_socket_request_reply_error_to_all_requests (socket, SMT_SOCKET_ERROR, error);
                                request = NULL;
                            }
                            break;
                        case SOCK_READ:
                            //  Attempt to read data even if an error condition is detected,
                            //  the read call will return the correct error if present.
                            if (desc->rtnevents & (APR_POLLIN | APR_POLLHUP | APR_POLLERR))
                                request = smt_socket_request_read_data (request);
                            else
                                request = smt_socket_request_by_socket_next (&request);
                            break;
                        case SOCK_CONNECT:
                            //  Always check error code for connect, since some implementations
                            //  return APR_POLLIN on error.
                            status = apr_socket_opt_get (socket->pollfd.desc.s, APR_SO_ERROR, &error);
                            //  Some implementations return error from getsockopt instead
                            if (status != APR_SUCCESS && !error)
                                error = status;
                            if (!error) {
                                next_request = request;
                                next_request = smt_socket_request_by_socket_next (&next_request);
                                smt_socket_request_return (&request, SMT_OK, APR_SUCCESS);
                                request = next_request;
                            }
                            else {
                                smt_socket_request_reply_error_to_all_requests (socket, SMT_SOCKET_ERROR, error);
                                request = NULL;
                            }
                            break;
                        case SOCK_WRITE:
                            //  Attempt to write data even if an error condition is detected,
                            //  the write call will return the correct error if present.
                            if (desc->rtnevents & (APR_POLLOUT | APR_POLLHUP | APR_POLLERR))
                                request = smt_socket_request_write_data (request);
                            else
                                request = smt_socket_request_by_socket_next (&request);
                            break;
                        case SOCK_CLOSE:
                            //  If we get here, any pending write requests will have
                            //  been processed, so we can just close the socket
                            if (desc->rtnevents & (APR_POLLOUT | APR_POLLHUP | APR_POLLERR))
                                smt_socket_request_close_the_socket (request);
                            //  No more requests after close
                            request = NULL;
                            break;
                        case SOCK_MONITOR:
                            if (!(desc->rtnevents & APR_POLLERR)) {
                                thread = request->thread;

                                //  If APR_POLLHUP is set, there may be data left on the socket.
                                //  so we need to notify the thread.
                                if (desc->rtnevents & (APR_POLLIN | APR_POLLHUP))
                                    smt_thread_set_next_event (request->thread,
                                                               request->input_event);
                                else
                                if (desc->rtnevents & APR_POLLOUT)
                                    smt_thread_set_next_event (request->thread,
                                                               request->output_event);

                                smt_socket_request_destroy (&thread->monitor);
                                if (thread->pending_count == 0)
                                    smt_thread_activate (thread);
                            }
                            else {
                                //  If APR_POLLERR is set, get error code.
                                status = apr_socket_opt_get (socket->pollfd.desc.s, APR_SO_ERROR, &error);
                                //  Some implementations return error from getsockopt instead
                                if (status != APR_SUCCESS && !error)
                                    error = status;
                                smt_socket_request_reply_error_to_all_requests (socket, SMT_SOCKET_ERROR, error);
                            }
                            request = NULL; //  No more requests after monitor
                            break;
                    }
                }
            }
#if (defined (BASE_THREADSAFE))
        }
#endif
    }
</method>

<method name = "reply error to all requests" private = "1">
    <argument name = "socket" type = "smt_socket_t *">The socket</argument>
    <argument name = "result" type = "int">SMT result code</argument>
    <argument name = "error"  type = "int">System error number</argument>
    <doc>
    Send an error reply to all requests on a given socket and close the socket.
    </doc>
    <local>
    smt_socket_request_t
        *request;
    </local>
    request = smt_socket_request_by_socket_first (socket->request_list);
    while (request) {
        if (socket->pollfd.reqevents & (APR_POLLIN | APR_POLLOUT)) {
            icl_apr_assert (apr_pollset_remove (request->os_thread->pollset, &socket->pollfd));
            if (request->type == SOCK_MONITOR)
                apr_pollset_remove (request->os_thread->waking_pollset, &socket->pollfd);
            socket->pollfd.reqevents &= ~ (APR_POLLIN | APR_POLLOUT);
        }        
        smt_socket_request_return (&request, result, error);
        request = smt_socket_request_by_socket_first (socket->request_list);
    }
    //  Close the socket
    if (socket->pollfd.desc.s) {
        apr_socket_close (socket->pollfd.desc.s);
        socket->pollfd.desc.s = NULL;
    }
</method>

<method name = "accept connection" return = "rc" private = "1">
    <argument name = "self" type = "smt_socket_request_t *">The request</argument>
    <declare  name = "rc"   type = "Bool" default = "TRUE" />
    <doc>
    Process a socket accept request.
    </doc>
    <local>
    apr_socket_t
        *slave_handle;
    apr_status_t
        status;
    int
        result = SMT_OK;
    </local>
    //  Create dummy smt_socket structure
    *self->socket_p = smt_socket_new ();        
    if (*self->socket_p) {
        //  Close unneeded descriptor created by smt_socket_new()
        //  No other way to do this, see BASE2-313 and weep
        apr_socket_close ((*self->socket_p)->pollfd.desc.s);
        //  Actually accept the connection
        status = apr_socket_accept (&slave_handle, 
                                    self->socket->pollfd.desc.s, 
                                    (*self->socket_p)->pollfd.p);
        if (status == APR_SUCCESS) {
            apr_socket_opt_set (slave_handle, APR_SO_NONBLOCK, 1);
            apr_socket_opt_set (slave_handle, APR_TCP_NODELAY, 1);    // No Nagle
            //  Stash the new descriptor in the smt_socket created above
            (*self->socket_p)->pollfd.desc.s = slave_handle;
            smt_socket_request_return (&self, result, status);
        }
        else
        if (APR_STATUS_IS_EAGAIN (status)
        ||  APR_STATUS_IS_ECONNABORTED (status)
        ||  APR_STATUS_IS_EINTR (status)) {
            //  Connection was aborted or the accept system call was interrupted
            //  Return FALSE and the call will get restarted the next time we poll
            smt_socket_destroy (self->socket_p);
            rc = FALSE;
        }
        else {
            smt_socket_destroy (self->socket_p);
            result = SMT_SOCKET_ERROR;
            smt_socket_request_return (&self, result, status);
        }
    }
    //  smt_socket_new() failed, but we don't know why.  Assume it's because it
    //  ran out of file descriptors. See BASE2-313 for details on why, and weep.
    else {
        result = SMT_SOCKET_ERROR;
        smt_socket_request_return (&self, result, APR_EMFILE);
    }
</method>

<method name = "read data" return = "next request" private = "1">
    <argument name = "self" type = "smt_socket_request_t *">The request</argument>
    <declare  name = "next request" type = "smt_socket_request_t *">The next socket request to process</declare>
    <doc>
    Process a socket read request.
    </doc>
    <local>
    apr_sockaddr_t
        *sockaddr;
    char
        *addr;
    size_t
        bytes_req,
        bytes_read,
        overhang;
    apr_status_t
        status;
    </local>
    if (self->mother)
        bytes_req = bytes_read = ipr_mother_unfilled (self->mother);
    else
        bytes_req = bytes_read = self->max_size - self->cur_size;
    status = apr_socket_recv (
        self->socket->pollfd.desc.s,
        (char *) self->buffer + self->cur_size,
        &bytes_read);

#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_SOCKET_REQUEST))
    icl_trace_record (NULL, s_dump_trace_read, bytes_read);
#endif
    if ((s_smt_socket_request_trace || self->socket->trace) && bytes_read > 0) {
        apr_socket_addr_get (&sockaddr, APR_REMOTE, self->socket->pollfd.desc.s);
        apr_sockaddr_ip_get (&addr, sockaddr);
        icl_console_print ("I: read %d bytes from %s req: %d bytes", bytes_read, addr, bytes_req);
        s_dump_buffer (self->thread->id, self->buffer + self->cur_size, bytes_read);
    }
    if (status == APR_SUCCESS) {
        if (self->mother) {
            ipr_mother_fill (self->mother, bytes_read);
            overhang = ipr_mother_overhang (self->mother);
            while (self
               &&  self->type == SOCK_READ) {
                if (overhang > self->min_size - self->cur_size) {
                    overhang -= self->min_size - self->cur_size;
                    self->cur_size = self->min_size;
                }
                else {
                    self->cur_size += overhang;
                    overhang = 0;
                }
                if (self->cur_size >= self->min_size) {
                    self->socket->io_size = self->cur_size;
                
                    *self->slave_p = ipr_mother_claim (self->mother, self->cur_size);
                    assert (*self->slave_p);

                    if (s_smt_socket_request_trace || self->socket->trace) {
                        icl_console_print ("Returning %d bytes:", (*self->slave_p)->cur_size);
                        s_dump_buffer (self->thread->id, (*self->slave_p)->data, (*self->slave_p)->cur_size);
                    }
                    
                    next_request = self;
                    next_request = smt_socket_request_by_socket_next (&next_request);
                    assert (!next_request || next_request->mother == self->mother);
                    
                    smt_socket_request_return (&self, SMT_OK, APR_SUCCESS);
                    self = next_request;
                }
                else {
                    next_request = self;
                    next_request = smt_socket_request_by_socket_next (&next_request);
                    
                    //  Set timer from next SOCK_READ request to service
                    smt_socket_request_timer (self);
                        
                    break;
                }
            }
        }
        else {
            self->cur_size += bytes_read;
            if (self->cur_size >= self->min_size) {
                self->socket->io_size = self->cur_size;
                next_request = self;
                next_request = smt_socket_request_by_socket_next (&next_request);
                smt_socket_request_return (&self, SMT_OK, APR_SUCCESS);
            }
            else {
                next_request = self;
                next_request = smt_socket_request_by_socket_next (&next_request);
                smt_socket_request_timer (self);
            }
        }
    }
    else
    if (APR_STATUS_IS_EOF (status)) {
        //  If we got EOF, return any data we might have read
        self->socket->io_size = self->cur_size;
        next_request = self;
        next_request = smt_socket_request_by_socket_next (&next_request);
        smt_socket_request_return (&self, SMT_SOCKET_CLOSING, status);
    }
    else {
        next_request = self;
        next_request = smt_socket_request_by_socket_next (&next_request);
        smt_socket_request_return (&self, SMT_SOCKET_ERROR, status);
    }
                    
    //  Skip past any remaining input requests
    while (next_request && REQUEST_IS_INPUT (next_request))
        next_request = smt_socket_request_by_socket_next (&next_request);
</method>

<method name = "write data" return = "next request">
    <argument name = "self" type = "smt_socket_request_t *">The request</argument>
    <declare  name = "next request" type = "smt_socket_request_t *">The next socket request to process</declare>
    <doc>
    Process a socket write request.
    </doc>
    <local>
    smt_socket_request_t
        *request;
    struct iovec
        vector[SMT_SOCKET_VECTOR_MAX];
    qbyte
        nvec;
    apr_sockaddr_t
        *sockaddr;
    char
        *addr;
    size_t
        bytes_written,
        bytes_dumped;
    apr_status_t
        status;
    </local>
    //  Build vector of socket write requests
    
    vector[0].iov_base = self->buffer   + self->cur_size;
    vector[0].iov_len  = self->max_size - self->cur_size;
    
    nvec    = 1;
    request = self;
    request = smt_socket_request_by_socket_next (&request);
    while (nvec < SMT_SOCKET_VECTOR_MAX
       &&  request && request->type == SOCK_WRITE) {
        vector[nvec].iov_base = request->buffer;
        vector[nvec].iov_len  = request->max_size;
        nvec++;
        
        request = smt_socket_request_by_socket_next (&request);
    };
    
    //  Perform the vector write
    
    status = apr_socket_sendv (
        self->socket->pollfd.desc.s,
        vector, nvec,
        &bytes_written);

    //  Do tracing if active

#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_SOCKET_REQUEST))
    icl_trace_record (NULL, s_dump_trace_write, bytes_written); 
#endif
    if ((s_smt_socket_request_trace || self->socket->trace) && bytes_written > 0) {
        apr_socket_addr_get (&sockaddr, APR_REMOTE, self->socket->pollfd.desc.s);
        apr_sockaddr_ip_get (&addr, sockaddr);
        icl_console_print ("I: writing %d bytes to %s", bytes_written, addr);
        //  Note that s_dump_buffer cannot handle vector, so only dump bytes
        //  from one buffer in the vector.
        bytes_dumped = bytes_written;
        if (bytes_dumped > self->max_size - self->cur_size)
            bytes_dumped = self->max_size - self->cur_size;
        s_dump_buffer (self->thread->id, self->buffer + self->cur_size, bytes_dumped);
    }
    
    //  Work out what to do next
    
    request = self;
    //  Windows sockets returns zero bytes from write if the receiving end closed
    //  the connection.  Convert this to EPIPE.
    if (status == APR_SUCCESS && bytes_written == 0)
        status = APR_EPIPE;
    if (status == APR_SUCCESS) {
        //  Loop through requests that were performed
        do {
            assert (bytes_written >= 0);
            
            next_request = request;
            next_request = smt_socket_request_by_socket_next (&next_request);
            if (bytes_written >= request->max_size - request->cur_size) {
                bytes_written -= request->max_size - request->cur_size;
                
                smt_socket_request_return (&request, SMT_OK, APR_SUCCESS);
                
                request = next_request;
            }
            else {
                request->cur_size += bytes_written;
                
                //  Set timer from next SOCK_WRITE request to service
                smt_socket_request_timer  (request);
                
                //  Skip past any remaining output requests
                while (next_request && REQUEST_IS_OUTPUT (next_request))
                    next_request = smt_socket_request_by_socket_next (&next_request);
                    
                break;
            }
        } while (request && request->type == SOCK_WRITE);
    }
    else {
        next_request = smt_socket_request_by_socket_next (&request);
        //  Don't return error if status was EAGAIN - we could be doing
        //  pre-emptive writing.
        if (APR_STATUS_IS_EAGAIN (status)) {
            //  Skip past any remaining SOCK_WRITE or SOCK_CLOSE requests
            while (next_request && REQUEST_IS_OUTPUT (next_request))
                next_request = smt_socket_request_by_socket_next (&next_request);
        }
        else {
            //  EPIPE means that the write failed due to the socket being
            //  closed at the remote end, so kill any remaining write requests
            if (APR_STATUS_IS_EPIPE (status)) {
                smt_socket_request_return (&self, SMT_SOCKET_CLOSING, status);
                while (next_request && REQUEST_IS_OUTPUT (next_request) && next_request->type != SOCK_CLOSE) {
                    request = next_request;
                    next_request = smt_socket_request_by_socket_next (&next_request);
                    smt_socket_request_return (&request, SMT_SOCKET_CLOSING, status);
                }
            }
            else {
                smt_socket_request_reply_error_to_all_requests (self->socket, SMT_SOCKET_ERROR, status);
                next_request = NULL;
            }
        }
    }
</method>

<method name = "close the socket" private = "1">
    <argument name = "self" type = "smt_socket_request_t *">The request</argument>
    <doc>
    Process a socket close request.
    </doc>
    <local>
    smt_socket_t
        *socket;
    smt_socket_t
        **socket_p;
    apr_status_t
        status;
    </local>
    socket   = self->socket;
    socket_p = self->socket_p;

    //  Assume close was successful.  Otherwise we get into a mess with the    
    //  poll requests - apr insists they be removed before socket is closed.   
    smt_socket_request_return (&self, SMT_OK, APR_SUCCESS);

    //  Close the socket.                                           
    status = apr_socket_close (socket->pollfd.desc.s);

    //  Return error to any other requests on the same socket.                 
    smt_socket_request_reply_error_to_all_requests (socket, SMT_SOCKET_ERROR, APR_EPIPE);

    //  Finally destroy the original socket.  Do this last as it cancels       
    //  other requests on the same socket.                                     
    smt_socket_destroy (socket_p);
</method>

<private name = "footer">
#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_SOCKET_REQUEST))
static void 
s_dump_trace_read (icl_os_thread_t thread, apr_time_t time, qbyte info)
{
    icl_console_print_thread_time (thread, time, "smt_socket_request read %d bytes", info);
}

static void 
s_dump_trace_write (icl_os_thread_t thread, apr_time_t time, qbyte info)
{
    icl_console_print_thread_time (thread, time, "smt_socket_request wrote %d bytes", info);
}
#endif

//  Dump buffer in ASCII and HEX                                               

static void
s_dump_buffer (smt_thread_id_t id, void *buffer, size_t size)
{
#   define BYTES_PER_LINE   (16)

    static const char
        *hex_digit = "0123456789abcdef";
    size_t
        count,
        repeat_count = 0;
    byte
        *ucbuffer = (byte *) buffer,
        cur_byte;
    char
        last_buffer [(BYTES_PER_LINE * 2) + 1],
        hex_buffer  [(BYTES_PER_LINE * 2) + 1],
        text_buffer [ BYTES_PER_LINE + 1],
        *hex,
        *text;
    int
        remainder;

    assert (buffer);
    memset (last_buffer, 0, sizeof (last_buffer));
    while (size > 0) {
        hex       = hex_buffer;
        text      = text_buffer;
        count     = BYTES_PER_LINE;
        remainder = BYTES_PER_LINE - size;
        while (size > 0 && count > 0) {
            size--;
            count--;
            cur_byte = *ucbuffer;

            //  Is current byte a printable character?                         
            *text++ = (cur_byte < 32 || cur_byte > 127) ? '.' : cur_byte;
            *hex++  = hex_digit [cur_byte >> 4];
            *hex++  = hex_digit [cur_byte & 0x0f];
            ucbuffer++;
        }
        while (remainder > 0) {
            *hex++ = ' ';
            *hex++ = ' ';
            remainder--;
        }
        *hex = '\\0';
        *text = '\\0';
        if (memcmp (last_buffer, hex_buffer, sizeof (last_buffer)) == 0)
            repeat_count++;
        else {
            if (repeat_count) {
                icl_console_print ("%4lu> I: [repeated %d times]", id, repeat_count);
                repeat_count = 0;
            }
            icl_console_print ("%4lu> I: %s | %s", id, hex_buffer, text_buffer);
            memcpy (last_buffer, hex_buffer, sizeof (last_buffer));
        }
    }
    if (repeat_count)
        icl_console_print ("%4lu> I: [repeated %d times]", id, repeat_count);
}
</private>

</class>
