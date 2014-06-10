/*---------------------------------------------------------------------------
    demo_client_connection.c - demo_client_connection component

A demo_client_connection object establishes and manages a single
connection to a demo server. The connection can hold
many sessions - each is a demo_client_session object. To use this
API, first create a connection to the server, then create one or
more sessions and do work on the sessions. Each session represents
a serial stream of work. In simple applications, a single session
is usually sufficient. In multithreaded applications, each thread
will want its own session.
    Generated from demo_client_connection.icl by icl_gen using GSL/4.
    
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
#include "demo_client_connection.h"     //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_client_connection_t

//  Shorthands for class methods

#define self_new            demo_client_connection_new
#define self_annihilate     demo_client_connection_annihilate
#define self_auth_plain     demo_client_connection_auth_plain
#define self_wait           demo_client_connection_wait
#define self_set_silent     demo_client_connection_set_silent
#define self_set_timeout    demo_client_connection_set_timeout
#define self_get_silent     demo_client_connection_get_silent
#define self_get_alive      demo_client_connection_get_alive
#define self_get_error_text  demo_client_connection_get_error_text
#define self_get_version_major  demo_client_connection_get_version_major
#define self_get_version_minor  demo_client_connection_get_version_minor
#define self_get_server_properties  demo_client_connection_get_server_properties
#define self_get_mechanisms  demo_client_connection_get_mechanisms
#define self_get_locales    demo_client_connection_get_locales
#define self_get_challenge  demo_client_connection_get_challenge
#define self_get_channel_max  demo_client_connection_get_channel_max
#define self_get_frame_max  demo_client_connection_get_frame_max
#define self_get_heartbeat  demo_client_connection_get_heartbeat
#define self_get_known_hosts  demo_client_connection_get_known_hosts
#define self_get_reply_code  demo_client_connection_get_reply_code
#define self_get_reply_text  demo_client_connection_get_reply_text
#define self_get_class_id   demo_client_connection_get_class_id
#define self_get_method_id  demo_client_connection_get_method_id
#define self_initialise     demo_client_connection_initialise
#define self_terminate      demo_client_connection_terminate
#define self_selftest       demo_client_connection_selftest
#define self_show           demo_client_connection_show
#define self_destroy        demo_client_connection_destroy
#define self_alloc          demo_client_connection_alloc
#define self_free           demo_client_connection_free
#define self_cache_initialise  demo_client_connection_cache_initialise
#define self_cache_purge    demo_client_connection_cache_purge
#define self_cache_terminate  demo_client_connection_cache_terminate
#define self_show_animation  demo_client_connection_show_animation
#define self_new_in_scope   demo_client_connection_new_in_scope

static void
    demo_client_connection_annihilate (
demo_client_connection_t * ( * self_p )  //  Reference to object reference
);

static void
    demo_client_connection_initialise (
void);

static void
    demo_client_connection_terminate (
void);

#define demo_client_connection_alloc()  demo_client_connection_alloc_ (__FILE__, __LINE__)
static demo_client_connection_t *
    demo_client_connection_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_client_connection_free (
demo_client_connection_t * self         //  Object reference
);

static void
    demo_client_connection_cache_initialise (
void);

static void
    demo_client_connection_cache_purge (
void);

static void
    demo_client_connection_cache_terminate (
void);

Bool
    demo_client_connection_animating = TRUE;  //  Animation enabled by default
static Bool
    s_demo_client_connection_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_demo_client_connection_mutex  = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static int
    s_wait_for_methods (demo_client_connection_t *self, int timeout, Bool blocking);
/*  -------------------------------------------------------------------------
    demo_client_connection_new

    Type: Component method
    Creates and initialises a new demo_client_connection_t object, returns a
    reference to the created object.
    Create new connection to server.

    The host argument specifies a server name or IP address, optionally
    ending in ':' plus a port number.

    The virtual_host name specifies the virtual host to which the connection
    will connect.  The default virtual host is "/".

    The auth_data provides an authentication block, used to login to the
    server.  To create a plain-text authentication block, use the auth_plain
    method.

    The trace argument sets the trace level for the client API.

    The timeout argument governs all synchronous exchanges with the server
    - if the server does not respond within this time, the connection treats
    it as a fatal error.  A timeout of zero means "infinite".  A good value
    for fast networks is five to ten seconds; for a slower network, a value
    of 30 seconds or more is reasonable.
    -------------------------------------------------------------------------
 */

demo_client_connection_t *
    demo_client_connection_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * host,                        //  Host to connect to
    char * virtual_host,                //  Virtual host
    icl_longstr_t * auth_data,          //  Authentication data
    char* instance,                     //  Client instance name
    int trace,                          //  Trace level, 0 - 3
    int timeout                         //  Connection timeout, msecs
)
{
char
    *env_value;                 //  Get environment setting
    demo_client_connection_t *
        self = NULL;                    //  Object reference

if (!s_demo_client_connection_active)
    self_initialise ();
    self = demo_client_connection_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_CLIENT_CONNECTION_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_client_connection_show_);
#endif

//
self->method_queue = smt_method_queue_sync_new ();
self->timeout = timeout;
self->trace = trace;
self->error_text = "no error";
icl_shortstr_cpy (self->host, host);

env_value = getenv ("WIREAPI_SILENT");
if (env_value && streq (env_value, "1"))
    self->silent = TRUE;

self->thread = demo_client_agent_connection_thread_new (
    self->method_queue,
    &self->alive,
    &self->interrupt,
    &self->reply_code,
    self->reply_text,
    host,
    virtual_host,
    auth_data,
    instance,
    trace,
    timeout);

//  Let the SMT engine take over until we get a reply on our queue
if (self->thread) {
    if (s_wait_for_methods (self, self->timeout, TRUE) == 0) {
        self->alive = TRUE;         //  Connection is ready for use
        //  Decide whether to use Direct Mode or not
        if (demo_client_config_direct (demo_client_config) || self->server_direct)
            self->direct = TRUE;
    }
    else
        self_destroy (&self);
}
}

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_annihilate

    Type: Component method
    Close the connection and all sessions on it.  Call this when your
    application ends, or if you want to break the connection to the
    server.  This performs a clean shut-down of the connection, so any
    server-side resources are freed.
    -------------------------------------------------------------------------
 */

static void
    demo_client_connection_annihilate (
    demo_client_connection_t * ( * self_p )  //  Reference to object reference
)
{

    demo_client_connection_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

DEMO_CLIENT_CONNECTION_ASSERT_SANE (self);

//
if (self->alive) {
    demo_client_agent_connection_close (self->thread);
    while (self->alive) {
        //  Wait until we get back an CLOSED method, or timeout
        s_wait_for_methods (self, self->timeout, FALSE);
    }
}
icl_longstr_destroy (&self->server_properties);
icl_longstr_destroy (&self->mechanisms);
icl_longstr_destroy (&self->locales);
icl_longstr_destroy (&self->challenge);
smt_method_queue_destroy (&self->method_queue);
smt_thread_unlink (&self->thread);

}
/*  -------------------------------------------------------------------------
    demo_client_connection_auth_plain

    Type: Component method
    Formats a new authentication data block for a plain login.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    demo_client_connection_auth_plain (
    char * login,                       //  User login name
    char * password                     //  User password
)
{
    icl_longstr_t *
        auth_data;                      //  Authentication data

//
auth_data = ipr_sasl_plain_encode (login, password);

    return (auth_data);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_wait

    Type: Component method
    Accepts a demo_client_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Waits for any activity on the connection. Returns -1 if the connection
    ended abnormally. If the timeout is zero, waits forever.
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_wait (
    demo_client_connection_t * self,    //  Reference to object
    int timeout                         //  Timeout, in milliseconds
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_CONNECTION_ASSERT_SANE (self);

//
if (self->alive) {
    if (s_wait_for_methods (self, timeout, FALSE))
        rc = -1;
}
else
    rc = -1;


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_set_silent

    Type: Component method
    Accepts a demo_client_connection_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_set_silent (
    demo_client_connection_t * self,    //  Reference to object
    Bool silent                         //  Not documented
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_CONNECTION_ASSERT_SANE (self);

//
self->silent = silent;


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_set_timeout

    Type: Component method
    Accepts a demo_client_connection_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_set_timeout (
    demo_client_connection_t * self,    //  Reference to object
    Bool timeout                        //  Not documented
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_CONNECTION_ASSERT_SANE (self);

//
self->timeout = timeout;


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_silent

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    demo_client_connection_get_silent (
    demo_client_connection_t * self     //  Object reference
)
{
    Bool
        silent;                         //  Not documented

//
silent = self->silent;

    return (silent);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_alive

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    demo_client_connection_get_alive (
    demo_client_connection_t * self     //  Object reference
)
{
    Bool
        alive;                          //  Not documented

//
alive = self->alive;

    return (alive);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_error_text

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_connection_get_error_text (
    demo_client_connection_t * self     //  Object reference
)
{
    char *
        error_text;                     //  Not documented

//
error_text = self->error_text;

    return (error_text);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_version_major

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_get_version_major (
    demo_client_connection_t * self     //  Object reference
)
{
    int
        version_major;                  //  Not documented

//
version_major = self->version_major;

    return (version_major);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_version_minor

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_get_version_minor (
    demo_client_connection_t * self     //  Object reference
)
{
    int
        version_minor;                  //  Not documented

//
version_minor = self->version_minor;

    return (version_minor);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_server_properties

    Type: Component method
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    demo_client_connection_get_server_properties (
    demo_client_connection_t * self     //  Object reference
)
{
    icl_longstr_t *
        server_properties;              //  Not documented

//
server_properties = self->server_properties;

    return (server_properties);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_mechanisms

    Type: Component method
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    demo_client_connection_get_mechanisms (
    demo_client_connection_t * self     //  Object reference
)
{
    icl_longstr_t *
        mechanisms;                     //  Not documented

//
mechanisms = self->mechanisms;

    return (mechanisms);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_locales

    Type: Component method
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    demo_client_connection_get_locales (
    demo_client_connection_t * self     //  Object reference
)
{
    icl_longstr_t *
        locales;                        //  Not documented

//
locales = self->locales;

    return (locales);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_challenge

    Type: Component method
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    demo_client_connection_get_challenge (
    demo_client_connection_t * self     //  Object reference
)
{
    icl_longstr_t *
        challenge;                      //  Not documented

//
challenge = self->challenge;

    return (challenge);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_channel_max

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_get_channel_max (
    demo_client_connection_t * self     //  Object reference
)
{
    int
        channel_max;                    //  Not documented

//
channel_max = self->channel_max;

    return (channel_max);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_frame_max

    Type: Component method
    -------------------------------------------------------------------------
 */

qbyte
    demo_client_connection_get_frame_max (
    demo_client_connection_t * self     //  Object reference
)
{
    qbyte
        frame_max;                      //  Not documented

//
frame_max = self->frame_max;

    return (frame_max);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_heartbeat

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_get_heartbeat (
    demo_client_connection_t * self     //  Object reference
)
{
    int
        heartbeat;                      //  Not documented

//
heartbeat = self->heartbeat;

    return (heartbeat);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_known_hosts

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_connection_get_known_hosts (
    demo_client_connection_t * self     //  Object reference
)
{
    char *
        known_hosts;                    //  Not documented

//
known_hosts = self->known_hosts;

    return (known_hosts);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_reply_code

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_get_reply_code (
    demo_client_connection_t * self     //  Object reference
)
{
    int
        reply_code;                     //  Not documented

//
reply_code = self->reply_code;

    return (reply_code);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_reply_text

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_connection_get_reply_text (
    demo_client_connection_t * self     //  Object reference
)
{
    char *
        reply_text;                     //  Not documented

//
reply_text = self->reply_text;

    return (reply_text);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_class_id

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_get_class_id (
    demo_client_connection_t * self     //  Object reference
)
{
    int
        class_id;                       //  Not documented

//
class_id = self->class_id;

    return (class_id);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_get_method_id

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_connection_get_method_id (
    demo_client_connection_t * self     //  Object reference
)
{
    int
        method_id;                      //  Not documented

//
method_id = self->method_id;

    return (method_id);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_initialise

    Type: Component method
    Initialise the connection class.  If the application has not already
    loaded config data, loads this from wireapi.cfg.
    -------------------------------------------------------------------------
 */

static void
    demo_client_connection_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_demo_client_connection_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_demo_client_connection_mutex)
            s_demo_client_connection_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_demo_client_connection_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_demo_client_connection_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

if (!demo_client_config) {
    demo_client_config = demo_client_config_new ();
    demo_client_config_load_xmlfile (demo_client_config, "wireapi.cfg", FALSE);
}
smt_initialise ();
demo_client_agent_init ();
            s_demo_client_connection_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_demo_client_connection_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    demo_client_connection_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_connection_terminate (
void)
{

if (s_demo_client_connection_active) {

demo_client_config_destroy (&demo_client_config);
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_demo_client_connection_mutex);
#endif
        s_demo_client_connection_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    demo_client_connection_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_connection_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_client_connection_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_connection_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
demo_client_connection_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <demo_client_connection file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    demo_client_connection_destroy

    Type: Component method
    Destroys a demo_client_connection_t object. Takes the address of a
    demo_client_connection_t reference (a pointer to a pointer) and nullifies the
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
    demo_client_connection_destroy_ (
    demo_client_connection_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    demo_client_connection_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    demo_client_connection_annihilate (self_p);
    demo_client_connection_free ((demo_client_connection_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_client_connection_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_client_connection_t *
    demo_client_connection_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_client_connection_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_client_connection_cache_initialise ();

self = (demo_client_connection_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_client_connection_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_free

    Type: Component method
    Freess a demo_client_connection_t object.
    -------------------------------------------------------------------------
 */

static void
    demo_client_connection_free (
    demo_client_connection_t * self     //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (demo_client_connection_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_client_connection_t));
        self->object_tag = DEMO_CLIENT_CONNECTION_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_client_connection_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_client_connection_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_client_connection_t));
icl_system_register (demo_client_connection_cache_purge, demo_client_connection_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_client_connection_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_connection_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_client_connection_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_connection_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_client_connection_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_client_connection_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_client_connection_animating = enabled;
}
/*  -------------------------------------------------------------------------
    demo_client_connection_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_connection_new_in_scope_ (
    demo_client_connection_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * host,                        //  Host to connect to
    char * virtual_host,                //  Virtual host
    icl_longstr_t * auth_data,          //  Authentication data
    char* instance,                     //  Client instance name
    int trace,                          //  Trace level, 0 - 3
    int timeout                         //  Connection timeout, msecs
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_client_connection_new_ (file,line,host,virtual_host,auth_data,instance,trace,timeout);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_client_connection_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
/*  Waits for methods from the protocol agent.  If the blocking argument
    is true, returns only when receiving a synchronous reply method.  If not,
    returns when any incoming method is received.  Returns 0 if ok, -1 if
    the connection is closed or a blocking wait timed-out.
 */
static int
s_wait_for_methods (
    demo_client_connection_t *self,
    int timeout,
    Bool blocking)
{
    smt_method_t
        *smt_method = NULL;
    demo_client_method_t
        *proto_method;
    int
        rc = 0;                         //  OK by default

    FOREVER {
        smt_method_destroy (&smt_method);
        smt_method_queue_wait (self->method_queue, timeout);
        smt_method = smt_method_queue_pop (self->method_queue);
        //  If we don't have a method, we timed-out
        if (!smt_method) {
            if (blocking) {
                self->error_text = "No reply received - timeout";
                rc = -1;
            }
            break;
        }
        else
        //  ALMOST means we should wait for the next method...
        if (smt_method->result == DEMO_CLIENT_REPLY_ALMOST) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method)
                demo_client_method_execute (proto_method, self, NULL);
        }
        else
        //  INTERN is an internal synchronisation from the agent
        if (smt_method->result == DEMO_CLIENT_REPLY_INTERN)
            break;                      //  We can return to caller
        else
        //  METHOD means the agent is sending us either:
        //    - a sync method, which is a reply to a sync request
        //    - an async method, which we want to process and then continue
        //
        //  We always stop when we get a sync method. If we have an async
        //  method, we stop only if the queue is empty and we're not blocking.
        //  Blocking means we want a sync method within the timeout.
        //
        if (smt_method->result == DEMO_CLIENT_REPLY_METHOD) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method) {
                demo_client_method_execute (proto_method, self, NULL);
                if (proto_method->sync)
                    break;              //  Got synchronous method, end
                else
                if (smt_method_queue_isempty (self->method_queue) && !blocking)
                    break;              //  Got final async method but not blocking
            }
            else
                break;                  //  Synchronisation from agent
        }
        else
        if (smt_method->result == DEMO_CLIENT_REPLY_CLOSED) {
            self->alive = FALSE;        //  Connection is closed
            rc = -1;                    //  Signal connection is closed
            break;
        }
        else {
            self->error_text = "Internal error - invalid reply method from protocol agent";
            icl_console_print ("E: invalid reply result %d", smt_method->result);
        }
    }
    smt_method_destroy (&smt_method);
    return (rc);
}

//  Embed the version information in the resulting binary

char *EMBED__demo_client_connection_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_client_connection_component  = "demo_client_connection ";
char *EMBED__demo_client_connection_version  = "1.0 ";
char *EMBED__demo_client_connection_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_client_connection_filename  = "demo_client_connection.icl ";
char *EMBED__demo_client_connection_builddate  = "2011/03/01 ";
char *EMBED__demo_client_connection_version_end  = "VeRsIoNeNd:ipc";

