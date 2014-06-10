/*---------------------------------------------------------------------------
    http_server.c - http_server component

    This implements the HTTP server object, which is the top-level object
    holding server-wide values and owning server-wide structures.
    Generated from http_server.icl by smt_object_gen using GSL/4.
    
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
#include "http_server.h"                //  Definitions for our class

//  Shorthand for class type

#define self_t              http_server_t

//  Shorthands for class methods

#define self_new            http_server_new
#define self_annihilate     http_server_annihilate
#define self_start          http_server_start
#define self_ban_address    http_server_ban_address
#define self_sync_hashes    http_server_sync_hashes
#define self_new_password   http_server_new_password
#define self_selftest       http_server_selftest
#define self__http_access_module_bind  http_server__http_access_module_bind
#define self__http_access_module_unbind_shim  http_server__http_access_module_unbind_shim
#define self__http_access_module_unbind  http_server__http_access_module_unbind
#define self_new_password_shim  http_server_new_password_shim
#define self__http_driver_module_bind  http_server__http_driver_module_bind
#define self__http_driver_module_unbind_shim  http_server__http_driver_module_unbind_shim
#define self__http_driver_module_unbind  http_server__http_driver_module_unbind
#define self_destroy        http_server_destroy
#define self_unlink         http_server_unlink
#define self_show_animation  http_server_show_animation
#define self_free           http_server_free
#define self_initialise     http_server_initialise
#define self_terminate      http_server_terminate
#define self_show           http_server_show
#define self_alloc          http_server_alloc
#define self_link           http_server_link
#define self_cache_initialise  http_server_cache_initialise
#define self_cache_purge    http_server_cache_purge
#define self_cache_terminate  http_server_cache_terminate
#define self_new_in_scope   http_server_new_in_scope

#define http_server_annihilate(self)    http_server_annihilate_ (self, __FILE__, __LINE__)
static int
    http_server_annihilate_ (
http_server_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    http_server__http_access_module_unbind_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal           //  Portal reference
);

static int
    http_server_new_password_shim (
void * self_v,                          //  Self as void pointer
http_access_module_t * portal,          //  Portal back to server
char * key,                             //  Not documented
char * hash,                            //  Not documented
Bool digest                             //  Not documented
);

static void
    http_server__http_driver_module_unbind_shim (
void * self_v,                          //  Self as void pointer
http_driver_module_t * portal           //  Portal reference
);

static void
    http_server_initialise (
void);

#define http_server_alloc()             http_server_alloc_ (__FILE__, __LINE__)
static http_server_t *
    http_server_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    http_server_cache_initialise (
void);

static void
    http_server_cache_purge (
void);

static void
    http_server_cache_terminate (
void);

Bool
    http_server_animating = TRUE;       //  Animation enabled by default
static Bool
    s_http_server_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_http_server_mutex      = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static void
    s_setup_log_files (http_server_t *self);
static void
    s_load_content_types (http_server_t *self);
static void
    s_format_logfile (char *logfile, char *configured);
http_server_t
    *http_server = NULL;
/*  -------------------------------------------------------------------------
    http_server_new

    Type: Component method
    Creates and initialises a new http_server_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

http_server_t *
    http_server_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    http_server_t *
        self = NULL;                    //  Object reference

if (!s_http_server_active)
    self_initialise ();
    self = http_server_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_SERVER_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_server_show_);
#endif

self->thread = http_server_agent_class_thread_new (self);
self->thread->animate = TRUE;

self->http_driver_module_list = ipr_looseref_list_new ();
self->http_driver_module_factory_list = ipr_looseref_list_new ();

self->http_access_module_list = ipr_looseref_list_new ();
self->http_access_module_factory_list = ipr_looseref_list_new ();

//
self->monitor_timer  = http_config_monitor (http_config);
self->security_timer = http_config_nervosity (http_config);
self->password_guard = http_config_password_guard (http_config);
self->imeter         = ipr_meter_new ();
self->ometer         = ipr_meter_new ();
self->content_types  = ipr_dict_table_new ();
self->access_log     = smt_log_new ();
self->debug_log      = smt_log_new ();
self->alert_log      = smt_log_new ();

s_setup_log_files (self);
s_load_content_types (self);
http_server_sync_hashes (self);

//  Start protocol server agent
http_agent_init (self);
self_start (self);
self->started = apr_time_now ();

smt_coremark_calculate (self->alert_log, http_config_coremark_cycles (http_config));
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_server_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_server_annihilate_ (
    http_server_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    int
        history_last;
#endif

    http_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % HTTP_SERVER_HISTORY_LENGTH] = file;
    self->history_line  [history_last % HTTP_SERVER_HISTORY_LENGTH] = line;
    self->history_type  [history_last % HTTP_SERVER_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % HTTP_SERVER_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (http_server_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to http_server agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_server_start

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    -------------------------------------------------------------------------
 */

int
    http_server_start (
    http_server_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_server_agent_start (self->thread)) {
        //icl_console_print ("Error sending 'start' method to http_server agent");
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
    http_server_ban_address

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Adds one IP address to the blacklist.  We do the work here to avoid more
    than one thread trying to write to the blacklist at once.  This assumes
    we only have one http_server instance, which is the current design.
    -------------------------------------------------------------------------
 */

int
    http_server_ban_address (
    http_server_t * self,               //  Reference to object
    char * address                      //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_server_agent_ban_address (self->thread,address)) {
        //icl_console_print ("Error sending 'ban address' method to http_server agent");
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
    http_server_sync_hashes

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Loads, or reloads the server hashes.  These are the blacklist, the basic
    password, and the digest password file.
    -------------------------------------------------------------------------
 */

int
    http_server_sync_hashes (
    http_server_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_server_agent_sync_hashes (self->thread)) {
        //icl_console_print ("Error sending 'sync hashes' method to http_server agent");
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
    http_server_new_password

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    This is the new password incoming response method, which the client class should
    inherit and provide the body for.  This method is invoked by the portal when
    the server makes the response; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

int
    http_server_new_password (
    http_server_t * self,               //  Reference to object
    http_access_module_t * portal,      //  Portal back to server
    char * key,                         //  Not documented
    char * hash,                        //  Not documented
    Bool digest                         //  Not documented
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_server_agent_new_password (self->thread,portal,key,hash,digest)) {
        //icl_console_print ("Error sending 'new password' method to http_server agent");
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
    http_server_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_server_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_server__http_access_module_bind

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    Binds a http_access_module portal, ready for use.
    -------------------------------------------------------------------------
 */

int
    http_server__http_access_module_bind (
    http_server_t * self,               //  Reference to object
    http_access_module_t * http_access_module  //  Portal reference
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

//
http_access_module->client_object = self;
if (http_access_module->server_object)
    http_access_module->client_looseref = ipr_looseref_queue (
        self->http_access_module_list, http_access_module_link (http_access_module));
else                                //  server_object is null for factories
    http_access_module->client_looseref = ipr_looseref_queue (
        self->http_access_module_factory_list, http_access_module_link (http_access_module));

//  Build virtual function table for client-side methods
http_access_module->client_unbind = self__http_access_module_unbind_shim;
http_access_module->response_new_password = self_new_password_shim;

//  Signal that portal is ready for use
http_access_module_ready (http_access_module);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_server__http_access_module_unbind_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_server__http_access_module_unbind_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal       //  Portal reference
)
{

self__http_access_module_unbind ((http_server_t *) self_v, portal);
}
/*  -------------------------------------------------------------------------
    http_server__http_access_module_unbind

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Client-to-portal unbind.
    -------------------------------------------------------------------------
 */

int
    http_server__http_access_module_unbind (
    http_server_t * self,               //  Reference to object
    http_access_module_t * portal       //  Portal reference
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_server_agent__http_access_module_unbind (self->thread,portal)) {
        //icl_console_print ("Error sending '_http_access_module unbind' method to http_server agent");
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
    http_server_new_password_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    http_server_new_password_shim (
    void * self_v,                      //  Self as void pointer
    http_access_module_t * portal,      //  Portal back to server
    char * key,                         //  Not documented
    char * hash,                        //  Not documented
    Bool digest                         //  Not documented
)
{
    int
        rc = 0;                         //  Return code

rc = self_new_password ((http_server_t *) self_v, portal, key, hash, digest);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_server__http_driver_module_bind

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    Binds a http_driver_module portal, ready for use.
    -------------------------------------------------------------------------
 */

int
    http_server__http_driver_module_bind (
    http_server_t * self,               //  Reference to object
    http_driver_module_t * http_driver_module  //  Portal reference
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

//
http_driver_module->client_object = self;
if (http_driver_module->server_object)
    http_driver_module->client_looseref = ipr_looseref_queue (
        self->http_driver_module_list, http_driver_module_link (http_driver_module));
else                                //  server_object is null for factories
    http_driver_module->client_looseref = ipr_looseref_queue (
        self->http_driver_module_factory_list, http_driver_module_link (http_driver_module));

//  Build virtual function table for client-side methods
http_driver_module->client_unbind = self__http_driver_module_unbind_shim;

//  Signal that portal is ready for use
http_driver_module_ready (http_driver_module);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_server__http_driver_module_unbind_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_server__http_driver_module_unbind_shim (
    void * self_v,                      //  Self as void pointer
    http_driver_module_t * portal       //  Portal reference
)
{

self__http_driver_module_unbind ((http_server_t *) self_v, portal);
}
/*  -------------------------------------------------------------------------
    http_server__http_driver_module_unbind

    Type: Component method
    Accepts a http_server_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Client-to-portal unbind.
    -------------------------------------------------------------------------
 */

int
    http_server__http_driver_module_unbind (
    http_server_t * self,               //  Reference to object
    http_driver_module_t * portal       //  Portal reference
)
{
    int
        rc = 0;                         //  Return code

HTTP_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (http_server_agent__http_driver_module_unbind (self->thread,portal)) {
        //icl_console_print ("Error sending '_http_driver_module unbind' method to http_server agent");
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
    http_server_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_server_destroy_ (
    http_server_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        rc = http_server_annihilate_ (self_p, file, line);
    else
    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        http_server_free (self);
    *self_p = NULL;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_server_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    http_server_unlink_ (
    http_server_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    int
        history_last;
#endif
    http_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on http_server object");
            http_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_SERVER_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % HTTP_SERVER_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                http_server_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                http_server_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    http_server_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_server_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_server_animating = enabled;

http_server_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    http_server_free

    Type: Component method
    Freess a http_server_t object.
    -------------------------------------------------------------------------
 */

void
    http_server_free_ (
    http_server_t * self,               //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_SERVER_HISTORY_LENGTH] = "free";
        self->history_links [history_last % HTTP_SERVER_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);
        memset (&self->object_tag, 0, sizeof (http_server_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_server_t));
        self->object_tag = HTTP_SERVER_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_server_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_server_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_http_server_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_http_server_mutex)
            s_http_server_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_http_server_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_http_server_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

http_server_agent_init ();
            s_http_server_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_http_server_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    http_server_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_server_terminate (
void)
{

if (s_http_server_active) {

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_http_server_mutex);
#endif
        s_http_server_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    http_server_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_server_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_server_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <http_server zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    if (self->history_last > HTTP_SERVER_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % HTTP_SERVER_HISTORY_LENGTH;
        self->history_last %= HTTP_SERVER_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % HTTP_SERVER_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </http_server>\n");
#endif

}
/*  -------------------------------------------------------------------------
    http_server_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_server_t *
    http_server_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_server_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_server_cache_initialise ();

self = (http_server_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_server_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_server_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

http_server_t *
    http_server_link_ (
    http_server_t * self,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
    int
        history_last;
#endif

    if (self) {
        HTTP_SERVER_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_SERVER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_SERVER_HISTORY_LENGTH] = "link";
        self->history_links [history_last % HTTP_SERVER_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    http_server_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_server_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_server_t));
icl_system_register (http_server_cache_purge, http_server_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_server_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_server_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_server_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_server_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_server_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_server_new_in_scope_ (
    http_server_t * * self_p,           //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_server_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_server_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
static void
s_setup_log_files (http_server_t *self)
{
    char
        *log_path,                      //  Log directory
        *archive_cmd;                   //  Archive command
    icl_shortstr_t
        log_name,                       //  Full logfile name
        stats_file;                     //  Statistics file name

    log_path    = http_config_log_path    (http_config);
    archive_cmd = http_config_archive_cmd (http_config);

    if (http_config_syslog (http_config)) {
        if (ipr_log_sysopen ("http"))
            icl_console_print (
                "W: syslog not available, using direct-to-file logging");
        smt_log_set_syslog (self->access_log, IPR_LOG_SYSINFO);
        smt_log_set_syslog (self->debug_log,  IPR_LOG_SYSDEBUG);
        smt_log_set_syslog (self->alert_log,  IPR_LOG_SYSWARNING);
    }
    else
    if (http_config_keep_logs (http_config)) {
        s_format_logfile (log_name, http_config_access_log (http_config));
        smt_log_open (self->access_log, log_name, log_path, archive_cmd);
        s_format_logfile (log_name, http_config_debug_log (http_config));
        smt_log_open (self->debug_log, log_name, log_path, archive_cmd);
        s_format_logfile (log_name, http_config_alert_log (http_config));
        smt_log_open (self->alert_log, log_name, log_path, archive_cmd);
    }
    //  Dump environment and configuration to debug log, silently
    smt_log_dump_env (self->debug_log);
    http_config_dump_log (http_config, self->debug_log);

    //  Set up log file associations and configuration
    smt_log_set_raw    (self->access_log, TRUE);
    smt_log_set_stderr (self->debug_log, TRUE);
    smt_log_set_stderr (self->alert_log, TRUE);

    if (http_config_record_stats (http_config)) {
        icl_shortstr_fmt (stats_file, "http_stats_%s.del", http_config_port (http_config));
        self->stats = fopen (stats_file, "w");
        //  We use a tab-delimited form that pastes easily into spreadsheets
        fprintf (self->stats, "BuffersKB\tCurIn\tCurOut\tAvgIn\tAvgOut\tTotReq\tTotResp\tClients\n");
    }
}

//  Format a configured value into a log name including the port
//
static void
s_format_logfile (char *log_name, char *configured)
{
    char
        *point;

    point = strchr (configured, '.');
    if (point) {
        //  Insert port number before extension
        icl_shortstr_ncpy (log_name, configured, point - configured);
        icl_shortstr_cat  (log_name, "_");
        icl_shortstr_cat  (log_name, http_config_port (http_config));
        icl_shortstr_cat  (log_name, point);
    }
    else {
        //  Add port number at end
        icl_shortstr_cpy  (log_name, configured);
        icl_shortstr_cat  (log_name, "_");
        icl_shortstr_cat  (log_name, http_config_port (http_config));
        icl_shortstr_cat  (log_name, ".log");
    }
}

//  Loads content types from configuration file
static void
s_load_content_types (http_server_t *self)
{
    ipr_config_t
        *config;                        //  Current server config file
    char
        *name,                          //  File extension
        *value;                         //  Content type

    config = ipr_config_dup (http_config_table (http_config));
    ipr_config_locate (config, "/config/content/type", NULL);
    while (config->located) {
        name  = ipr_config_get (config, "name", NULL);
        value = ipr_config_get (config, "value", NULL);
        if (name && value)
            ipr_dict_assume (self->content_types, name, value);
        else
            smt_log_print (self->alert_log, "W: config file has bad content-type definition");

        ipr_config_next (config);
    }
    ipr_config_destroy (&config);
}

//  Embed the version information in the resulting binary

char *EMBED__http_server_version_start    = "VeRsIoNsTaRt:ipc";
char *EMBED__http_server_component        = "http_server ";
char *EMBED__http_server_version          = "1.0 ";
char *EMBED__http_server_copyright        = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_server_filename         = "http_server.icl ";
char *EMBED__http_server_builddate        = "2011/03/01 ";
char *EMBED__http_server_version_end      = "VeRsIoNeNd:ipc";

