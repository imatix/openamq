/*---------------------------------------------------------------------------
    http_access_refresh.c - http_access_refresh component

    Password authentication refresh module.  If this module is loaded, it
    will attempt to recover from certain authentication failures by calling
    a shell command to update the password file(s).  The shell command can
    do whatever is needed - e.g. query a credentials database - to get new
    credentials for the specified user.

    The module issues a system command with this syntax:

        command-name mechanism username realm

    E.g.

        http_refresh Digest mufasa testrealm@host.com

    The module does not wait for a response.  The shell command should update
    the web server password file appropriately; Xitami will reload that data
    and use it.  The user may get a message asking them to retry later.
    Generated from http_access_refresh.icl by smt_object_gen using GSL/4.
    
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
#include "http_access_refresh.h"        //  Definitions for our class

//  Shorthand for class type

#define self_t              http_access_refresh_t

//  Shorthands for class methods

#define self_announce       http_access_refresh_announce
#define self_before         http_access_refresh_before
#define self_new            http_access_refresh_new
#define self_annihilate     http_access_refresh_annihilate
#define self_initialise     http_access_refresh_initialise
#define self_terminate      http_access_refresh_terminate
#define self__http_access_module_new  http_access_refresh__http_access_module_new
#define self__http_access_module_factory  http_access_refresh__http_access_module_factory
#define self__http_access_module_unbind_shim  http_access_refresh__http_access_module_unbind_shim
#define self__http_access_module_unbind  http_access_refresh__http_access_module_unbind
#define self_announce_shim  http_access_refresh_announce_shim
#define self_open_shim      http_access_refresh_open_shim
#define self_open           http_access_refresh_open
#define self_before_shim    http_access_refresh_before_shim
#define self_after_shim     http_access_refresh_after_shim
#define self_after          http_access_refresh_after
#define self_close_shim     http_access_refresh_close_shim
#define self_close          http_access_refresh_close
#define self__http_access_module_link  http_access_refresh__http_access_module_link
#define self__http_access_module_unlink  http_access_refresh__http_access_module_unlink
#define self_selftest       http_access_refresh_selftest
#define self_destroy        http_access_refresh_destroy
#define self_unlink         http_access_refresh_unlink
#define self_show_animation  http_access_refresh_show_animation
#define self_free           http_access_refresh_free
#define self_show           http_access_refresh_show
#define self_alloc          http_access_refresh_alloc
#define self_link           http_access_refresh_link
#define self_cache_initialise  http_access_refresh_cache_initialise
#define self_cache_purge    http_access_refresh_cache_purge
#define self_cache_terminate  http_access_refresh_cache_terminate
#define self_new_in_scope   http_access_refresh_new_in_scope

#define http_access_refresh_annihilate(self)  http_access_refresh_annihilate_ (self, __FILE__, __LINE__)
static int
    http_access_refresh_annihilate_ (
http_access_refresh_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    http_access_refresh_initialise (
void);

static void
    http_access_refresh__http_access_module_unbind_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal           //  Portal reference
);

static int
    http_access_refresh_announce_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal,          //  Portal back to server
smt_log_t * log                         //  Not documented
);

static int
    http_access_refresh_open_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal,          //  Portal back to server
http_access_context_t * context         //  Not documented
);

static int
    http_access_refresh_before_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal,          //  Portal back to server
http_access_context_t * context         //  Not documented
);

static int
    http_access_refresh_after_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal,          //  Portal back to server
http_access_context_t * context         //  Not documented
);

static int
    http_access_refresh_close_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal,          //  Portal back to server
http_access_context_t * context         //  Not documented
);

static void *
    http_access_refresh__http_access_module_link (
void * object                           //  Reference cast as a void *
);

static void
    http_access_refresh__http_access_module_unlink (
void * object_p                         //  Reference pointer cast as a void *
);

#define http_access_refresh_alloc()     http_access_refresh_alloc_ (__FILE__, __LINE__)
static http_access_refresh_t *
    http_access_refresh_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    http_access_refresh_cache_initialise (
void);

static void
    http_access_refresh_cache_purge (
void);

static void
    http_access_refresh_cache_terminate (
void);

Bool
    http_access_refresh_animating = TRUE;  //  Animation enabled by default
static Bool
    s_http_access_refresh_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_http_access_refresh_mutex  = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static ipr_looseref_list_t
    *s_http_access_module_list;         //  List of portal factories
static Bool
    s_http_access_module_stopped;       //  Is this portal factory stopped?
/*  -------------------------------------------------------------------------
    http_access_refresh_announce

    Type: Component method
    Accepts a http_access_refresh_t reference and returns zero in case of success,
    1 in case of errors.
    This is the announce incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

int
    http_access_refresh_announce (
    http_access_refresh_t * self,       //  Reference to object
    http_access_module_t * portal,      //  Portal back to client
    smt_log_t * log                     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_REFRESH_ASSERT_SANE (self);
if (!self->zombie) {

    smt_log_print (log, "I: X5 password refresh module loaded");

if (self) {
    assert (self->thread);
    if (http_access_refresh_agent_announce (self->thread,portal,log)) {
        //icl_console_print ("Error sending 'announce' method to http_access_refresh agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_before

    Type: Component method
    Accepts a http_access_refresh_t reference and returns zero in case of success,
    1 in case of errors.
    This is the before incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    Check if authentication failed, and if so, send shell command.
    -------------------------------------------------------------------------
 */

int
    http_access_refresh_before (
    http_access_refresh_t * self,       //  Reference to object
    http_access_module_t * portal,      //  Portal back to client
    http_access_context_t * context     //  Not documented
)
{
char
    *refresh_by = http_config_refresh_by (http_config);
icl_shortstr_t
    refresh_cmd;
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_REFRESH_ASSERT_SANE (self);
if (!self->zombie) {

    //
    if (strnull (refresh_by))
        ;   //  Do nothing
    else
    if (context->process) {
        //  If there is already a refresh process running, check whether it's
        //  completed successfully or not, and kill it if we think it's blocked.
        if (ipr_process_wait (context->process, FALSE)) {
            //  Allow ten seconds for the process to finish, else kill the
            //  process and signal an internal server error
            if (++context->retries < 5)
                rc = HTTP_ACCESS_RETRY;
            else {
                smt_log_print (context->alert_log,
                    "E: killing unresponsive '%s'", refresh_by);
                ipr_process_destroy (&context->process);
                http_response_set_error (context->response,
                    HTTP_REPLY_INTERNALERROR, "Authentication system did not respond");
            }
        }
        else {
            //  Process finished, so check its exit code
            if (ipr_process_exitcode (context->process)) {
                smt_log_print (context->alert_log,
                    "E: non-zero exit value for '%s' (%d)", refresh_by, ipr_process_exitcode (context->process));
                http_response_set_error (context->response,
                    HTTP_REPLY_INTERNALERROR, "Authentication system failed");
            }
            ipr_process_destroy (&context->process);
        }
    }
    else
    if (context->fail_code == IPR_PASSWORD_NOTFOUND
    ||  context->fail_code == IPR_PASSWORD_EXPIRED) {
        //  Launch a new refresh process
        if (context->digest)
            icl_shortstr_fmt (refresh_cmd, "%s Digest %s %s %s",
                refresh_by, context->fail_user, context->fail_realm, http_config_digest_auth (http_config));
        else
            icl_shortstr_fmt (refresh_cmd, "%s Basic %s %s %s",
                refresh_by, context->fail_user, context->fail_realm, http_config_basic_auth (http_config));

        smt_log_print (context->alert_log, "I: running password refresh '%s'", refresh_cmd);
        context->process = ipr_process_new (
            refresh_cmd,
            NULL,
            http_config_refresh_log (http_config),
            http_config_refresh_log (http_config));
        ipr_process_start (context->process, ".");
        rc = HTTP_ACCESS_RETRY;
    }

if (self) {
    assert (self->thread);
    if (http_access_refresh_agent_before (self->thread,portal,context)) {
        //icl_console_print ("Error sending 'before' method to http_access_refresh agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_new

    Type: Component method
    Creates and initialises a new http_access_refresh_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

http_access_refresh_t *
    http_access_refresh_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    http_access_refresh_t *
        self = NULL;                    //  Object reference

if (!s_http_access_refresh_active)
    self_initialise ();
    self = http_access_refresh_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_ACCESS_REFRESH_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_access_refresh_show_);
#endif

self->thread = http_access_refresh_agent_class_thread_new (self);
self->thread->animate = TRUE;

self->http_access_module_list = ipr_looseref_list_new ();
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_access_refresh_annihilate_ (
    http_access_refresh_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    int
        history_last;
#endif

    http_access_refresh_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = file;
    self->history_line  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = line;
    self->history_type  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (http_access_refresh_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to http_access_refresh agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_refresh_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_http_access_refresh_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_http_access_refresh_mutex)
            s_http_access_refresh_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_http_access_refresh_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_http_access_refresh_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

http_access_refresh_agent_init ();

s_http_access_module_list = ipr_looseref_list_new ();
            s_http_access_refresh_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_http_access_refresh_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    http_access_refresh_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_refresh_terminate (
void)
{
http_access_module_t
    *http_access_module;            //  Portal factory object

if (s_http_access_refresh_active) {

//
if (icl_atomic_cas32 ((volatile qbyte *) &s_http_access_module_stopped, TRUE, FALSE) == FALSE) {
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (s_http_access_module_list))) {
        http_access_module->server_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
}
ipr_looseref_list_destroy (&s_http_access_module_list);
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_http_access_refresh_mutex);
#endif
        s_http_access_refresh_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    http_access_refresh__http_access_module_new

    Type: Component method
    Creates and initialises a new portal instance.  If the server argument is
    null, creates a hidden server object that can only be referenced through
    the portal.  This is recommended for most purposes.
    -------------------------------------------------------------------------
 */

http_access_module_t *
    http_access_refresh__http_access_module_new (
    void * self_v                       //  Untyped server reference
)
{
http_access_refresh_t
    *self;
    http_access_module_t *
        http_access_module;             //  Not documented

//
//  Grab a link to a new or the provided server object
if (self_v)
    self = self_link ((http_access_refresh_t *) self_v);
else
    self = self_new ();

http_access_module = http_access_module_new (self, self__http_access_module_link, self__http_access_module_unlink);
http_access_module->server_looseref = ipr_looseref_queue (
    self->http_access_module_list, http_access_module_link (http_access_module));

//  Build virtual methods for server-side methods
http_access_module->server_unbind = self__http_access_module_unbind_shim;
http_access_module->request_announce = self_announce_shim;
http_access_module->request_open = self_open_shim;
http_access_module->request_before = self_before_shim;
http_access_module->request_after = self_after_shim;
http_access_module->request_close = self_close_shim;

//  Drop link to server object
self_unlink (&self);

    return (http_access_module);
}
/*  -------------------------------------------------------------------------
    http_access_refresh__http_access_module_factory

    Type: Component method
    Creates and initialises a new portal factory instance.
    -------------------------------------------------------------------------
 */

http_access_module_t *
    http_access_refresh__http_access_module_factory (
void)
{
    http_access_module_t *
        http_access_module;             //  Not documented

//
if (!s_http_access_refresh_active)
    self_initialise ();             //  Since we don't call http_access_refresh_new
http_access_module = http_access_module_new (NULL, self__http_access_module_link, self__http_access_module_unlink);
http_access_module->server_looseref = ipr_looseref_queue (
    s_http_access_module_list, http_access_module_link (http_access_module));

//  Build virtual methods for server-side methods
http_access_module->server_new = self__http_access_module_new;

    return (http_access_module);
}
/*  -------------------------------------------------------------------------
    http_access_refresh__http_access_module_unbind_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_refresh__http_access_module_unbind_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal       //  Portal reference
)
{

self__http_access_module_unbind ((http_access_refresh_t *) self_v, portal);
}
/*  -------------------------------------------------------------------------
    http_access_refresh__http_access_module_unbind

    Type: Component method
    Accepts a http_access_refresh_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Server-to-portal unbind.
    -------------------------------------------------------------------------
 */

int
    http_access_refresh__http_access_module_unbind (
    http_access_refresh_t * self,       //  Reference to object
    http_access_module_t * portal       //  Portal reference
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_REFRESH_ASSERT_SANE (self);
if (!self->zombie) {

    //

if (self) {
    assert (self->thread);
    if (http_access_refresh_agent__http_access_module_unbind (self->thread,portal)) {
        //icl_console_print ("Error sending '_http_access_module unbind' method to http_access_refresh agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_announce_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_access_refresh_announce_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal,      //  Portal back to server
    smt_log_t * log                     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

rc = self_announce ((http_access_refresh_t *) self_v, portal, log);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_open_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_access_refresh_open_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal,      //  Portal back to server
    http_access_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

rc = self_open ((http_access_refresh_t *) self_v, portal, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_open

    Type: Component method
    Accepts a http_access_refresh_t reference and returns zero in case of success,
    1 in case of errors.
    This is the open incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

int
    http_access_refresh_open (
    http_access_refresh_t * self,       //  Reference to object
    http_access_module_t * portal,      //  Portal back to client
    http_access_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_REFRESH_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_access_refresh_agent_open (self->thread,portal,context)) {
        //icl_console_print ("Error sending 'open' method to http_access_refresh agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_before_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_access_refresh_before_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal,      //  Portal back to server
    http_access_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

rc = self_before ((http_access_refresh_t *) self_v, portal, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_after_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_access_refresh_after_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal,      //  Portal back to server
    http_access_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

rc = self_after ((http_access_refresh_t *) self_v, portal, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_after

    Type: Component method
    Accepts a http_access_refresh_t reference and returns zero in case of success,
    1 in case of errors.
    This is the after incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

int
    http_access_refresh_after (
    http_access_refresh_t * self,       //  Reference to object
    http_access_module_t * portal,      //  Portal back to client
    http_access_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_REFRESH_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_access_refresh_agent_after (self->thread,portal,context)) {
        //icl_console_print ("Error sending 'after' method to http_access_refresh agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_close_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_access_refresh_close_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal,      //  Portal back to server
    http_access_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

rc = self_close ((http_access_refresh_t *) self_v, portal, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_close

    Type: Component method
    Accepts a http_access_refresh_t reference and returns zero in case of success,
    1 in case of errors.
    This is the close incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

int
    http_access_refresh_close (
    http_access_refresh_t * self,       //  Reference to object
    http_access_module_t * portal,      //  Portal back to client
    http_access_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_ACCESS_REFRESH_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_access_refresh_agent_close (self->thread,portal,context)) {
        //icl_console_print ("Error sending 'close' method to http_access_refresh agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh__http_access_module_link

    Type: Component method
    -------------------------------------------------------------------------
 */

static void *
    http_access_refresh__http_access_module_link (
    void * object                       //  Reference cast as a void *
)
{

//
object = (void *) http_access_refresh_link (((http_access_refresh_t *) object));

    return (object);
}
/*  -------------------------------------------------------------------------
    http_access_refresh__http_access_module_unlink

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_refresh__http_access_module_unlink (
    void * object_p                     //  Reference pointer cast as a void *
)
{

//
http_access_refresh_unlink (((http_access_refresh_t **) object_p));
}
/*  -------------------------------------------------------------------------
    http_access_refresh_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_refresh_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_access_refresh_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_access_refresh_destroy_ (
    http_access_refresh_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_access_refresh_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        rc = http_access_refresh_annihilate_ (self_p, file, line);
    else
    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        http_access_refresh_free (self);
    *self_p = NULL;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    http_access_refresh_unlink_ (
    http_access_refresh_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    int
        history_last;
#endif
    http_access_refresh_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on http_access_refresh object");
            http_access_refresh_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                http_access_refresh_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                http_access_refresh_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    http_access_refresh_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_access_refresh_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_access_refresh_animating = enabled;

http_access_refresh_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_free

    Type: Component method
    Freess a http_access_refresh_t object.
    -------------------------------------------------------------------------
 */

void
    http_access_refresh_free_ (
    http_access_refresh_t * self,       //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = "free";
        self->history_links [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);
        memset (&self->object_tag, 0, sizeof (http_access_refresh_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_access_refresh_t));
        self->object_tag = HTTP_ACCESS_REFRESH_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_access_refresh_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_refresh_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_access_refresh_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <http_access_refresh zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    if (self->history_last > HTTP_ACCESS_REFRESH_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % HTTP_ACCESS_REFRESH_HISTORY_LENGTH;
        self->history_last %= HTTP_ACCESS_REFRESH_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % HTTP_ACCESS_REFRESH_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </http_access_refresh>\n");
#endif

}
/*  -------------------------------------------------------------------------
    http_access_refresh_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_access_refresh_t *
    http_access_refresh_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_access_refresh_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_access_refresh_cache_initialise ();

self = (http_access_refresh_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_access_refresh_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

http_access_refresh_t *
    http_access_refresh_link_ (
    http_access_refresh_t * self,       //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
    int
        history_last;
#endif

    if (self) {
        HTTP_ACCESS_REFRESH_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_ACCESS_REFRESH)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = "link";
        self->history_links [history_last % HTTP_ACCESS_REFRESH_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_access_refresh_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_access_refresh_t));
icl_system_register (http_access_refresh_cache_purge, http_access_refresh_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_access_refresh_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_refresh_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_access_refresh_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_access_refresh_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_access_refresh_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_access_refresh_new_in_scope_ (
    http_access_refresh_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_access_refresh_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_access_refresh_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__http_access_refresh_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__http_access_refresh_component  = "http_access_refresh ";
char *EMBED__http_access_refresh_version  = "0.0 ";
char *EMBED__http_access_refresh_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_access_refresh_filename  = "http_access_refresh.icl ";
char *EMBED__http_access_refresh_builddate  = "2011/03/01 ";
char *EMBED__http_access_refresh_version_end  = "VeRsIoNeNd:ipc";

