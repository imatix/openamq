/*---------------------------------------------------------------------------
    smt_socket_request.c - smt_socket_request component

    The smt_socket_request class contains all the methods for doing activity
    on a connected socket.  The socket may be connected by smt_socket_connect
    or a slave socket created when by remote connection to a passively opened
    socket.

    You do not typically need to manipulate smt_socket_request objects; once
    created they do the work without further intervention by the application.
    Generated from smt_socket_request.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "smt_socket_request.h"         //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_socket_request_t

//  Shorthands for class methods

#define self_initialise     smt_socket_request_initialise
#define self_new            smt_socket_request_new
#define self_timer          smt_socket_request_timer
#define self_accept         smt_socket_request_accept
#define self_destroy        smt_socket_request_destroy
#define self_free           smt_socket_request_free
#define self_return         smt_socket_request_return
#define self_monitor        smt_socket_request_monitor
#define self_read           smt_socket_request_read
#define self_read_bucket    smt_socket_request_read_bucket
#define self_read_mother    smt_socket_request_read_mother
#define self_write          smt_socket_request_write
#define self_write_bucket   smt_socket_request_write_bucket
#define self_close          smt_socket_request_close
#define self_poll           smt_socket_request_poll
#define self_wait           smt_socket_request_wait
#define self_trace          smt_socket_request_trace
#define self_selftest       smt_socket_request_selftest
#define self_process_active_sockets  smt_socket_request_process_active_sockets
#define self_reply_error_to_all_requests  smt_socket_request_reply_error_to_all_requests
#define self_accept_connection  smt_socket_request_accept_connection
#define self_read_data      smt_socket_request_read_data
#define self_write_data     smt_socket_request_write_data
#define self_close_the_socket  smt_socket_request_close_the_socket
#define self_remove_from_all_containers  smt_socket_request_remove_from_all_containers
#define self_show           smt_socket_request_show
#define self_terminate      smt_socket_request_terminate
#define self_alloc          smt_socket_request_alloc
#define self_show_animation  smt_socket_request_show_animation
#define self_new_in_scope   smt_socket_request_new_in_scope

static void
    smt_socket_request_free (
smt_socket_request_t * self             //  Object reference
);

static void
    smt_socket_request_process_active_sockets (
smt_os_thread_t * os_thread,            //  The OS thread
apr_int32_t num_descs,                  //  The number of descriptors in the set
apr_pollfd_t * descs                    //  The array of descriptors
);

static void
    smt_socket_request_reply_error_to_all_requests (
smt_socket_t * socket,                  //  The socket
int result,                             //  SMT result code
int error                               //  System error number
);

static Bool
    smt_socket_request_accept_connection (
smt_socket_request_t * self             //  The request
);

static smt_socket_request_t *
    smt_socket_request_read_data (
smt_socket_request_t * self             //  The request
);

static void
    smt_socket_request_close_the_socket (
smt_socket_request_t * self             //  The request
);

static void
    smt_socket_request_terminate (
void);

#define smt_socket_request_alloc()      smt_socket_request_alloc_ (__FILE__, __LINE__)
static smt_socket_request_t *
    smt_socket_request_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

Bool
    smt_socket_request_animating = TRUE;  //  Animation enabled by default
static Bool
    s_smt_socket_request_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_smt_socket_request_mutex  = NULL;
#endif
#if (defined (BASE_THREADSAFE))
static apr_threadkey_t
    *threadkey;                         //  To hold thread-local cache header
#else
static smt_socket_request_t
        *s_cache = NULL;
#endif

//  Macros

#if (defined (__WINDOWS__))
    //  Under windows, CTRL-C does not generate a SIGINT but must be           
    //  trapped differently.  In particular it does not interrupt a            
    //  select().  To get around this we never block for more than a           
    //  certain period at a time, so that we can test for a raised signal.     
#   define SMT_MAX_TIMEOUT   100       //  Check for signal every 100us        
#endif

#define REQUEST_IS_INPUT(r)  (r->type == SOCK_READ       || r->type == SOCK_ACCEPT)
#define REQUEST_IS_OUTPUT(r) (r->type == SOCK_CONNECT    || r->type == SOCK_WRITE    || r->type == SOCK_CLOSE)

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

#define REQUEST_TYPE_NAME(t)                                                   ((t < sizeof (request_type_name) / sizeof (char *))    ? request_type_name [t] : "ILLEGAL")

//  Function prototypes

#if (defined (BASE_TRACE) || defined (BASE_TRACE_SMT_SOCKET_REQUEST))
static void
    s_dump_trace_read                   (icl_os_thread_t thread, apr_time_t time, qbyte info);
static void
    s_dump_trace_write                  (icl_os_thread_t thread, apr_time_t time, qbyte info);
#endif
static void
    s_dump_buffer                       (smt_thread_id_t id, void *buffer, size_t size);
/*  -------------------------------------------------------------------------
    smt_socket_request_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_smt_socket_request_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_smt_socket_request_mutex)
            s_smt_socket_request_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_smt_socket_request_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_smt_socket_request_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_create (&threadkey, NULL, icl_global_pool));
#endif

            s_smt_socket_request_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_smt_socket_request_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    smt_socket_request_new

    Type: Component method
    Creates and initialises a new smt_socket_request_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    Initialises the new item to be a valid list item.
    This method is for internal use only.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_new_ (
    smt_thread_t * thread,              //  Not documented
    smt_socket_request_type_t type,     //  Not documented
    smt_socket_t * socket,              //  Not documented
    qbyte msecs,                        //  Not documented
    smt_event_t input_event,            //  Not documented
    smt_event_t output_event,           //  Not documented
    char * file,                        //  Source file for callSource file for call
    size_t line                         //  Line number for callLine number for call
)
{
smt_socket_request_t
    *first_request,
    *request;
Bool
    disallow,
    write_request;
apr_int16_t
    reqevents;
    smt_socket_request_t *
        self = NULL;                    //  Object reference

if (!s_smt_socket_request_active)
    self_initialise ();
self = smt_socket_request_alloc_ (file, line);
if (self) {
    self->object_tag   = SMT_SOCKET_REQUEST_ALIVE;

self->by_socket_next = self;
self->by_socket_prev = self;
self->by_socket_head = NULL;
self->by_thread_next = self;
self->by_thread_prev = self;
self->by_thread_head = NULL;
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
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_timer

    Type: Component method
    Create a timer request for a socket request.  The timeout is in the 'msecs'
    field of the socket request.
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_timer (
    smt_socket_request_t * self         //  The request
)
{

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
}
/*  -------------------------------------------------------------------------
    smt_socket_request_accept

    Type: Component method
    Accepts a connection on a socket that was opened passively.

    On connection, the event is delivered, and/or the thread is woken, and
    an smt_socket object it put into *connect.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_accept (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t * socket,              //  Passively opened socket to accept connection
    qbyte msecs,                        //  Timeout for connection
    smt_event_t event,                  //  Event to deliver on connection; SMT_NULL_EVENT for simple thread-blocking
    smt_socket_t ** connect             //  Pointer to result of passive connection
)
{
int
    rc = SMT_OK;
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_destroy

    Type: Component method
    The purge method is for internal use only.  It may only be called from the
    OS thread that is managing the SMT thread.
    
    It destroys a socket request but does not wake a sleeping thread.
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_destroy_ (
    smt_socket_request_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
smt_socket_request_t
    *self_link,
    *next_request;
apr_int16_t
    reqevents;
    smt_socket_request_t *
        self = *self_p;                 //  Dereferenced Reference to object reference


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
}
/*  -------------------------------------------------------------------------
    smt_socket_request_free

    Type: Component method
    Freess a smt_socket_request_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_free (
    smt_socket_request_t * self         //  Object reference
)
{
#if (defined (BASE_THREADSAFE))
    smt_socket_request_t
        **s_cache;
    void
        *s_cache_p;
#endif


if (self) {

ipr_bucket_unlink (&self->bucket);
        self->object_tag = SMT_SOCKET_REQUEST_DEAD;
#if (defined (BASE_THREADSAFE))
        icl_apr_assert (apr_threadkey_private_get (&s_cache_p, threadkey));
        s_cache = s_cache_p;
        self->local_next = *s_cache;
        *s_cache = self;
#else
        self->local_next = s_cache;
        s_cache = self;
#endif
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_socket_request_return

    Type: Component method
    Socket request returns the given value and is destroyed.
    
    This method is for internal use only.
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_return (
    smt_socket_request_t * ( * self_p ),   //  Not documented
    int result,                         //  Not documented
    int error                           //  Not documented
)
{
smt_event_t
    event;
smt_thread_t
    *thread;
    smt_socket_request_t *
        self = *self_p;                 //  Dereferenced self

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
}
/*  -------------------------------------------------------------------------
    smt_socket_request_monitor

    Type: Component method
    Starts monitoring a socket, superseding any existing monitor by the thread.
    Monitor is cancelled by socket activity or any other event being delivered.
    
    This method is used to monitor a socket for activity while simultaneously
    accepting events on the event queue.  It monitors the socket for input 
    and/or output depending on whether 'input' or 'output' has a value other
    than SMT_NULL_EVENT.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_monitor (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t * socket,              //  Connected socket to monitor
    qbyte msecs,                        //  Timeout
    smt_event_t input_event,            //  Event to deliver on socket ready for input
    smt_event_t output_event            //  Event to deliver on socket ready for output
)
{
int
    rc = SMT_OK;
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_read

    Type: Component method
    Reads data from a connected socket.  Delivers an event and/or
    wakes the thread when the data are read.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_read (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t * socket,              //  Connected socket to read data from
    qbyte msecs,                        //  Timeout for socket to be ready
    size_t min_size,                    //  Minimum data size to read
    size_t max_size,                    //  Maximum data size to read
    byte * buffer,                      //  Buffer to store data read
    smt_event_t event                   //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
)
{
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
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_read_bucket

    Type: Component method
    Reads data from a connected socket.  Delivers an event and/or
    wakes the thread when the data are read.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_read_bucket_ (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t * socket,              //  Connected socket to read data from
    qbyte msecs,                        //  Timeout for socket to be ready
    size_t min_size,                    //  Minimum data size to read
    size_t max_size,                    //  Maximum data size to read
    ipr_bucket_t ** bucket_p,           //  Pointer to created bucket
    smt_event_t event,                  //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
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
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_read_mother

    Type: Component method
    Reads data from a connected socket using a mother object live data as a no-
    copy buffer.  If the mother pointer is NULL, a mother object is created.
    Returns a created slave bucket holding the requested data.
    
    Delivers an event and/or wakes the thread when the data are read.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_read_mother_ (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t * socket,              //  Connected socket to read data from
    qbyte msecs,                        //  Timeout for socket to be ready
    size_t size,                        //  Data size to return
    ipr_mother_t * ( * mother_p ),      //  Pointer to mother
    ipr_bucket_t ** slave_p,            //  Pointer to slave bucket to create
    smt_event_t event,                  //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
int
    rc = SMT_OK;
byte
    *buffer;
    ipr_mother_t *
        mother = *mother_p;             //  Dereferenced Pointer to mother
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_write

    Type: Component method
    Writes data to a connected socket.  Delivers an event and/or
    wakes the thread when the data are written.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_write (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t * socket,              //  Connected socket to write data to
    qbyte msecs,                        //  Timeout for socket to be ready
    size_t size,                        //  Data size to write
    byte * buffer,                      //  Buffer holding data to write
    smt_event_t event                   //  Event to deliver on write complete; SMT_NULL_EVENT for simple thread-blocking
)
{
int
    rc = SMT_OK;
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_write_bucket

    Type: Component method
    Writes data to a connected socket from a bucket.  Delivers an event and/or
    wakes the thread when the data are written.  The bucket is also destroyed
    at this time.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_write_bucket (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t * socket,              //  Connected socket to write data to
    qbyte msecs,                        //  Timeout for socket to be ready
    ipr_bucket_t * bucket,              //  Data in a bucket
    smt_event_t event                   //  Event to deliver on write complete; SMT_NULL_EVENT for simple thread-blocking
)
{
int
    rc = SMT_OK;
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_close

    Type: Component method
    Wait for any pending activity to be complete, then close the socket.
    Delivers an event and/or wakes the thread when the data are read.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_close (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_socket_t ** socket,             //  Connected socket to close
    qbyte msecs,                        //  Timeout for socket to be ready
    smt_event_t event                   //  Event to deliver on close complete; SMT_NULL_EVENT for simple thread-blocking
)
{
int
    rc = SMT_OK;
apr_status_t
    status = APR_EAGAIN;
    smt_socket_request_t *
        request = NULL;                 //  The socket request

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

    return (request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_poll

    Type: Component method
    This method checks for socket activity without blocking.
    Returns SMT_OK or SMT_FINISHED if no socket activity is awaited.
    
    This method is for internal use only.
    -------------------------------------------------------------------------
 */

int
    smt_socket_request_poll (
    smt_os_thread_t * os_thread,        //  Not documented
    Bool force                          //  Force poll of all sockets?
)
{
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
    int
        rc = SMT_OK;                    //  Not documented

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_wait

    Type: Component method
    This method checks for socket activity, blocking if required.

    Returns SMT_OK or SMT_SELECT_ERROR if something serious happened.

    This method is for internal use only.
    -------------------------------------------------------------------------
 */

int
    smt_socket_request_wait (
    smt_os_thread_t * os_thread         //  Not documented
)
{
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
    int
        rc = SMT_OK;                    //  Not documented

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_trace

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_trace (
    Bool enabled                        //  Not documented
)
{

s_smt_socket_request_trace = enabled;
}
/*  -------------------------------------------------------------------------
    smt_socket_request_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_socket_request_process_active_sockets

    Type: Component method
    Process the requests associated with an array of descriptors returned from a
    poll.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_process_active_sockets (
    smt_os_thread_t * os_thread,        //  The OS thread
    apr_int32_t num_descs,              //  The number of descriptors in the set
    apr_pollfd_t * descs                //  The array of descriptors
)
{
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
}
/*  -------------------------------------------------------------------------
    smt_socket_request_reply_error_to_all_requests

    Type: Component method
    Send an error reply to all requests on a given socket and close the socket.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_reply_error_to_all_requests (
    smt_socket_t * socket,              //  The socket
    int result,                         //  SMT result code
    int error                           //  System error number
)
{
smt_socket_request_t
    *request;

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
}
/*  -------------------------------------------------------------------------
    smt_socket_request_accept_connection

    Type: Component method
    Process a socket accept request.
    -------------------------------------------------------------------------
 */

static Bool
    smt_socket_request_accept_connection (
    smt_socket_request_t * self         //  The request
)
{
apr_socket_t
    *slave_handle;
apr_status_t
    status;
int
    result = SMT_OK;
    Bool
        rc = TRUE;                      //  Not documented

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_read_data

    Type: Component method
    Process a socket read request.
    -------------------------------------------------------------------------
 */

static smt_socket_request_t *
    smt_socket_request_read_data (
    smt_socket_request_t * self         //  The request
)
{
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
    smt_socket_request_t *
        next_request;                   //  The next socket request to process

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

    return (next_request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_write_data

    Type: Component method
    Process a socket write request.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_write_data (
    smt_socket_request_t * self         //  The request
)
{
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
    smt_socket_request_t *
        next_request;                   //  The next socket request to process

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

    return (next_request);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_close_the_socket

    Type: Component method
    Process a socket close request.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_close_the_socket (
    smt_socket_request_t * self         //  The request
)
{
smt_socket_t
    *socket;
smt_socket_t
    **socket_p;
apr_status_t
    status;

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
}
/*  -------------------------------------------------------------------------
    smt_socket_request_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_remove_from_all_containers (
    smt_socket_request_t * self         //  The itemThe item
)
{

smt_socket_request_by_thread_remove (self);
smt_socket_request_by_socket_remove (self);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
smt_socket_request_t
    *self;
int
    container_links;


self = item;
container_links = 0;
if (self->by_thread_head)
   container_links++;
if (self->by_socket_head)
   container_links++;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <smt_socket_request file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    smt_socket_request_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_terminate (
void)
{

if (s_smt_socket_request_active) {
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_delete (threadkey));
#endif

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_smt_socket_request_mutex);
#endif
        s_smt_socket_request_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    smt_socket_request_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_socket_request_t *
    smt_socket_request_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if (defined (BASE_THREADSAFE))
    smt_socket_request_t
        **s_cache;
    void
        *s_cache_p;
#endif

    smt_socket_request_t *
        self = NULL;                    //  Object reference

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_get (&s_cache_p, threadkey));
    s_cache = s_cache_p;
    if (!s_cache) {
        s_cache = malloc (sizeof (smt_socket_request_t **));
        *s_cache = NULL;
        icl_apr_assert (apr_threadkey_private_set (s_cache, threadkey));
    }
    else
        self = *s_cache;

    if (self) {
        *s_cache = self->local_next;
        self->local_next = NULL;
    }
    else
        self = (smt_socket_request_t *) malloc (sizeof (smt_socket_request_t));
#else
    if (s_cache) {
        self = s_cache;
        s_cache = self->local_next;
        self->local_next = NULL;
    }
    else
        self = (smt_socket_request_t *) malloc (sizeof (smt_socket_request_t));
#endif



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_socket_request_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_socket_request_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_new_in_scope_ (
    smt_socket_request_t * * self_p,    //  Not documented
    icl_scope_t * _scope,               //  Not documented
    smt_thread_t * thread,              //  Not documented
    smt_socket_request_type_t type,     //  Not documented
    smt_socket_t * socket,              //  Not documented
    qbyte msecs,                        //  Not documented
    smt_event_t input_event,            //  Not documented
    smt_event_t output_event,           //  Not documented
    char * file,                        //  Source file for callSource file for call
    size_t line                         //  Line number for callLine number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_socket_request_new_ (thread,type,socket,msecs,input_event,output_event,file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_socket_request_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
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
        *hex = '\0';
        *text = '\0';
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

//  Embed the version information in the resulting binary

char *EMBED__smt_socket_request_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_socket_request_component  = "smt_socket_request ";
char *EMBED__smt_socket_request_version   = "1.0 ";
char *EMBED__smt_socket_request_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_socket_request_filename  = "smt_socket_request.icl ";
char *EMBED__smt_socket_request_builddate  = "2011/03/01 ";
char *EMBED__smt_socket_request_version_end  = "VeRsIoNeNd:ipc";

