/*---------------------------------------------------------------------------
    smt_socket.c - smt_socket component

    The smt_socket class is used to create socket objects for use with the
    smt_socket_request class.
    Generated from smt_socket.icl by icl_gen using GSL/4.
    
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
#include "smt_socket.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_socket_t

//  Shorthands for class methods

#define self_new            smt_socket_new
#define self_annihilate     smt_socket_annihilate
#define self_connect        smt_socket_connect
#define self_passive        smt_socket_passive
#define self_set_nodelay    smt_socket_set_nodelay
#define self_set_rcvbuf     smt_socket_set_rcvbuf
#define self_set_sndbuf     smt_socket_set_sndbuf
#define self_peeraddr       smt_socket_peeraddr
#define self_trace          smt_socket_trace
#define self_selftest       smt_socket_selftest
#define self_terminate      smt_socket_terminate
#define self_show           smt_socket_show
#define self_destroy        smt_socket_destroy
#define self_alloc          smt_socket_alloc
#define self_free           smt_socket_free
#define self_cache_initialise  smt_socket_cache_initialise
#define self_cache_purge    smt_socket_cache_purge
#define self_cache_terminate  smt_socket_cache_terminate
#define self_show_animation  smt_socket_show_animation
#define self_new_in_scope   smt_socket_new_in_scope

static void
    smt_socket_annihilate (
smt_socket_t * ( * self_p )             //  Reference to object reference
);

#define smt_socket_alloc()              smt_socket_alloc_ (__FILE__, __LINE__)
static smt_socket_t *
    smt_socket_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_socket_free (
smt_socket_t * self                     //  Object reference
);

static void
    smt_socket_cache_initialise (
void);

static void
    smt_socket_cache_purge (
void);

static void
    smt_socket_cache_terminate (
void);

Bool
    smt_socket_animating = TRUE;        //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_socket_new

    Type: Component method
    Creates and initialises a new smt_socket_t object, returns a
    reference to the created object.
    Creates a new socket.  After creating a socket you should set any options
    that your application requires, and then call smt_socket_connect or
    smt_socket_passive as appropriate.
    -------------------------------------------------------------------------
 */

smt_socket_t *
    smt_socket_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
apr_status_t
    status;
    smt_socket_t *
        self = NULL;                    //  Object reference

    self = smt_socket_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_SOCKET_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_socket_show_);
#endif

status = apr_pool_create (&self->pollfd.p, icl_global_pool);
if (status == APR_SUCCESS) {
    apr_pool_tag    (self->pollfd.p, "smt_socket_new");
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
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_socket_annihilate

    Type: Component method
    Closes an open socket and immediately cancels any requests waiting on
    that socket.

    This method must be used to destroy sockets that are passively listening.

    This method may be used to rapidly close a connected socket.  If a tidy
    close is required, use smt_socket_request_close instead.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_annihilate (
    smt_socket_t * ( * self_p )         //  Reference to object reference
)
{

smt_socket_request_t
    *socket_request;
smt_thread_t
    *thread;
    smt_socket_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

SMT_SOCKET_ASSERT_SANE (self);

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

}
/*  -------------------------------------------------------------------------
    smt_socket_connect

    Type: Component method
    Accepts a smt_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Connect to a remote service asychronously.

    Returns SMT_OK immediately if the request was correctly processed; returns
    SMT_SOCKET_ERROR on error.  Note that the socket is not connected
    when the function call returns, but only after the event has been
    delivered.
    -------------------------------------------------------------------------
 */

int
    smt_socket_connect (
    smt_socket_t * self,                //  Reference to object
    smt_thread_t * thread,              //  Thread to deliver reply to
    qbyte msecs,                        //  Timeout for connection
    const char * host,                  //  Host to connect to
    const char * service,               //  Service to connect to
    smt_event_t event                   //  Event to deliver on connection; SMT_NULL_EVENT for simple thread-blocking
)
{
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
    int
        rc = 0;                         //  Return code

SMT_SOCKET_ASSERT_SANE (self);

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


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_passive

    Type: Component method
    Accepts a smt_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Setup a socket for passive listening. The service should be a port number
    (as a string) or a well-known TCP/IP service.  The ipaddress can be NULL
    (for "0.0.0.0", meaning "all addresses that this server supports"), or a
    specific IP address, if wanted.  The queue length should be at least 3 and
    as much as 20-30 for very heavily used services.

    Returns SMT_OK immediately if the request was correctly processed;
    returns SMT_SOCKET_ERROR on error.
    -------------------------------------------------------------------------
 */

int
    smt_socket_passive (
    smt_socket_t * self,                //  Reference to object
    smt_thread_t * thread,              //  Thread to deliver reply to
    const char * service,               //  Service to listen on
    const char * ipaddress,             //  IP address to bind to
    int queue_length                    //  Length of connect queue
)
{
apr_port_t
    port;
apr_sockaddr_t
    *sockaddr;
apr_status_t
    status;
char
    *endptr;
    int
        rc = 0;                         //  Return code

SMT_SOCKET_ASSERT_SANE (self);

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


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_set_nodelay

    Type: Component method
    Accepts a smt_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the value of the TCP NODELAY option on the socket, which controls
    whether or not the Nagle algorithm is used for this socket.
    -------------------------------------------------------------------------
 */

int
    smt_socket_set_nodelay (
    smt_socket_t * self,                //  Reference to object
    int value                           //  Option value
)
{
apr_status_t
    status;
    int
        rc = 0;                         //  Return code

SMT_SOCKET_ASSERT_SANE (self);

status = apr_socket_opt_set (self->pollfd.desc.s, APR_TCP_NODELAY, value);
if (status != APR_SUCCESS)
    rc = SMT_SOCKET_ERROR;


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_set_rcvbuf

    Type: Component method
    Accepts a smt_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the TCP receive buffer size for the socket. Returns the old buffer 
    size, if known. Should be called before calling smt_socket_connect or
    smt_socket_passive on the socket.
    -------------------------------------------------------------------------
 */

int
    smt_socket_set_rcvbuf (
    smt_socket_t * self,                //  Reference to object
    int new_size,                       //  New buffer size
    int * old_size                      //  Old buffer size
)
{
apr_status_t
    status = APR_SUCCESS;
    int
        rc = 0;                         //  Return code

SMT_SOCKET_ASSERT_SANE (self);

if (old_size)
    status = apr_socket_opt_get (self->pollfd.desc.s, APR_SO_RCVBUF, old_size);
if (status == APR_SUCCESS)
    status = apr_socket_opt_set (self->pollfd.desc.s, APR_SO_RCVBUF, new_size);
if (status != APR_SUCCESS)
    rc = SMT_SOCKET_ERROR;


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_set_sndbuf

    Type: Component method
    Accepts a smt_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the TCP send buffer size for the socket. Returns the old buffer 
    size, if known. Should be called before calling smt_socket_connect or
    smt_socket_passive on the socket.
    -------------------------------------------------------------------------
 */

int
    smt_socket_set_sndbuf (
    smt_socket_t * self,                //  Reference to object
    int new_size,                       //  New buffer size
    int * old_size                      //  Old buffer size
)
{
apr_status_t
    status = APR_SUCCESS;
    int
        rc = 0;                         //  Return code

SMT_SOCKET_ASSERT_SANE (self);

if (old_size)
    status = apr_socket_opt_get (self->pollfd.desc.s, APR_SO_SNDBUF, old_size);
if (status == APR_SUCCESS)
    status = apr_socket_opt_set (self->pollfd.desc.s, APR_SO_SNDBUF, new_size);
if (status != APR_SUCCESS)
    rc = SMT_SOCKET_ERROR;


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_peeraddr

    Type: Component method
    Returns the peer IP address of a connected socket, if any.
    -------------------------------------------------------------------------
 */

char *
    smt_socket_peeraddr (
    smt_socket_t * self                 //  Not documented
)
{
apr_sockaddr_t
    *sockaddr;
    char *
        addr;                           //  Not documented

apr_socket_addr_get (&sockaddr, APR_REMOTE, self->pollfd.desc.s);
apr_sockaddr_ip_get (&addr, sockaddr);

    return (addr);
}
/*  -------------------------------------------------------------------------
    smt_socket_trace

    Type: Component method
    Accepts a smt_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the trace flag for this socket.
    -------------------------------------------------------------------------
 */

int
    smt_socket_trace (
    smt_socket_t * self,                //  Reference to object
    Bool enabled                        //  Not documented
)
{
    int
        rc = 0;                         //  Return code

SMT_SOCKET_ASSERT_SANE (self);

self->trace = enabled;


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_socket_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_socket_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
smt_socket_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <smt_socket file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    smt_socket_destroy

    Type: Component method
    Destroys a smt_socket_t object. Takes the address of a
    smt_socket_t reference (a pointer to a pointer) and nullifies the
    reference after use.  Does nothing if the reference is already
    null.
    Performs an agressive destroy of an object.  This involves:
    1. Removing the object from any containers it is in.
    2. Making the object an zombie
    3. Decrementing the object's link count
    4. If the link count is zero then freeing the object.
    -------------------------------------------------------------------------
 */

void
    smt_socket_destroy_ (
    smt_socket_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_socket_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    smt_socket_annihilate (self_p);
    smt_socket_free ((smt_socket_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_socket_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_socket_t *
    smt_socket_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_socket_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_socket_cache_initialise ();

self = (smt_socket_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_socket_free

    Type: Component method
    Freess a smt_socket_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_free (
    smt_socket_t * self                 //  Object reference
)
{


if (self) {

    self->object_tag = SMT_SOCKET_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_socket_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_socket_t));
icl_system_register (smt_socket_cache_purge, smt_socket_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_socket_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_socket_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_socket_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_socket_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_socket_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_socket_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_socket_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_socket_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_new_in_scope_ (
    smt_socket_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_socket_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_socket_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_socket_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_socket_component         = "smt_socket ";
char *EMBED__smt_socket_version           = "1.0 ";
char *EMBED__smt_socket_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_socket_filename          = "smt_socket.icl ";
char *EMBED__smt_socket_builddate         = "2011/03/01 ";
char *EMBED__smt_socket_version_end       = "VeRsIoNeNd:ipc";

