/*---------------------------------------------------------------------------
    http_driver_module.c - http_driver_module component

    This class enables the creation of plug-in modules that handle specific
    URI paths for the server.  The plug-in accepts HTTP methods and an
    http_request object and returns a http_response object.
    Generated from http_driver_module.icl by icl_gen using GSL/4.
    
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
#include "http_driver_module.h"         //  Definitions for our class

//  Shorthand for class type

#define self_t              http_driver_module_t

//  Shorthands for class methods

#define self_new            http_driver_module_new
#define self_factory        http_driver_module_factory
#define self_request_announce  http_driver_module_request_announce
#define self_request_get    http_driver_module_request_get
#define self_request_head   http_driver_module_request_head
#define self_request_post   http_driver_module_request_post
#define self_request_put    http_driver_module_request_put
#define self_request_delete  http_driver_module_request_delete
#define self_request_move   http_driver_module_request_move
#define self_request_copy   http_driver_module_request_copy
#define self_annihilate     http_driver_module_annihilate
#define self_ready          http_driver_module_ready
#define self_selftest       http_driver_module_selftest
#define self_terminate      http_driver_module_terminate
#define self_show           http_driver_module_show
#define self_destroy        http_driver_module_destroy
#define self_alloc          http_driver_module_alloc
#define self_free           http_driver_module_free
#define self_read_lock      http_driver_module_read_lock
#define self_write_lock     http_driver_module_write_lock
#define self_unlock         http_driver_module_unlock
#define self_link           http_driver_module_link
#define self_unlink         http_driver_module_unlink
#define self_cache_initialise  http_driver_module_cache_initialise
#define self_cache_purge    http_driver_module_cache_purge
#define self_cache_terminate  http_driver_module_cache_terminate
#define self_show_animation  http_driver_module_show_animation
#define self_new_in_scope   http_driver_module_new_in_scope

#define http_driver_module_annihilate(self)  http_driver_module_annihilate_ (self, __FILE__, __LINE__)
static void
    http_driver_module_annihilate_ (
http_driver_module_t * ( * self_p ),    //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define http_driver_module_alloc()      http_driver_module_alloc_ (__FILE__, __LINE__)
static http_driver_module_t *
    http_driver_module_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define http_driver_module_free(self)   http_driver_module_free_ (self, __FILE__, __LINE__)
static void
    http_driver_module_free_ (
http_driver_module_t * self,            //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    http_driver_module_cache_initialise (
void);

static void
    http_driver_module_cache_purge (
void);

static void
    http_driver_module_cache_terminate (
void);

Bool
    http_driver_module_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    http_driver_module_new

    Type: Component method
    Creates and initialises a new http_driver_module_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

http_driver_module_t *
    http_driver_module_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    void * server_object,               //  Not documented
    untyped_link_fn * server_link,      //  Not documented
    untyped_unlink_fn * server_unlink,   //  Not documented
    char * path                         //  Not documented
)
{
    http_driver_module_t *
        self = NULL;                    //  Object reference

    self = http_driver_module_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_DRIVER_MODULE_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_driver_module_show_);
#endif

//
self->server_object = (server_link) (server_object);
self->server_unlink = server_unlink;

icl_shortstr_cpy (self->path, path);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_driver_module_factory

    Type: Component method
    Asks portal factory to create new portal with implicit server object. The
    server object is owned only by its portal; when you stop the owning portal
    the implicit server object is also stopped.
    -------------------------------------------------------------------------
 */

http_driver_module_t *
    http_driver_module_factory (
    http_driver_module_t * self,        //  Portal factory
    char * path                         //  Not documented
)
{
    http_driver_module_t *
        portal;                         //  New portal object

//
//  Check this is a valid factory
if (self->server_object)
    icl_console_print ("E: factory method is only allowed on portal factories");
assert (!self->server_object);
assert (self->server_new);
//
portal = (self->server_new) (NULL, path);

    return (portal);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_announce

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_announce (
    http_driver_module_t * self,        //  Not documented
    smt_log_t * log                     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_announce) (self->server_object, self, log);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_get

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_get (
    http_driver_module_t * self,        //  Not documented
    http_driver_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_get) (self->server_object, self, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_head

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_head (
    http_driver_module_t * self,        //  Not documented
    http_driver_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_head) (self->server_object, self, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_post

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_post (
    http_driver_module_t * self,        //  Not documented
    http_driver_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_post) (self->server_object, self, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_put

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_put (
    http_driver_module_t * self,        //  Not documented
    http_driver_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_put) (self->server_object, self, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_delete

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_delete (
    http_driver_module_t * self,        //  Not documented
    http_driver_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_delete) (self->server_object, self, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_move

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_move (
    http_driver_module_t * self,        //  Not documented
    http_driver_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_move) (self->server_object, self, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_request_copy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    http_driver_module_request_copy (
    http_driver_module_t * self,        //  Not documented
    http_driver_context_t * context     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

if (!self->bound)
    icl_console_print ("E: portal was not properly bound to client");
assert (self->bound);
rc = (self->request_copy) (self->server_object, self, context);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_driver_module_annihilate_ (
    http_driver_module_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
    int
        history_last;
#endif

    http_driver_module_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = file;
    self->history_line  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = line;
    self->history_type  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = self->links;
#endif

    HTTP_DRIVER_MODULE_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

if (icl_atomic_cas32 ((volatile qbyte *) &self->stopped, TRUE, FALSE) == FALSE) {
    self->bound = FALSE;
    (self->client_unbind) (self->client_object, self);
    if (self->server_object) {
        (self->server_unbind) (self->server_object, self);
        (self->server_unlink) (&self->server_object);
    }
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    http_driver_module_ready

    Type: Component method
    Accepts a http_driver_module_t reference and returns zero in case of success,
    1 in case of errors.
    Signals to the server that the portal has been bound by the client and
    is now ready for use.
    -------------------------------------------------------------------------
 */

int
    http_driver_module_ready (
    http_driver_module_t * self         //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_MODULE_ASSERT_SANE (self);
if (!self->zombie) {

//
self->bound = TRUE;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_module_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_driver_module_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_module_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    http_driver_module_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_module_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    http_driver_module_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <http_driver_module zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
    if (self->history_last > HTTP_DRIVER_MODULE_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % HTTP_DRIVER_MODULE_HISTORY_LENGTH;
        self->history_last %= HTTP_DRIVER_MODULE_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % HTTP_DRIVER_MODULE_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </http_driver_module>\n");
#endif

}
/*  -------------------------------------------------------------------------
    http_driver_module_destroy

    Type: Component method
    Destroys a http_driver_module_t object. Takes the address of a
    http_driver_module_t reference (a pointer to a pointer) and nullifies the
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
    http_driver_module_destroy_ (
    http_driver_module_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    http_driver_module_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        http_driver_module_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("http_driver_module", "E: missing link on http_driver_module object");
        http_driver_module_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        http_driver_module_free_ ((http_driver_module_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_driver_module_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_driver_module_t *
    http_driver_module_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_driver_module_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_driver_module_cache_initialise ();

self = (http_driver_module_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_driver_module_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_driver_module_free

    Type: Component method
    Freess a http_driver_module_t object.
    -------------------------------------------------------------------------
 */

static void
    http_driver_module_free_ (
    http_driver_module_t * self,        //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = "free";
        self->history_links [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = self->links;
#endif

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (http_driver_module_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_driver_module_t));
        self->object_tag = HTTP_DRIVER_MODULE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_driver_module_read_lock

    Type: Component method
    Accepts a http_driver_module_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_driver_module_read_lock (
    http_driver_module_t * self         //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_MODULE_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_write_lock

    Type: Component method
    Accepts a http_driver_module_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_driver_module_write_lock (
    http_driver_module_t * self         //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_MODULE_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_unlock

    Type: Component method
    Accepts a http_driver_module_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_driver_module_unlock (
    http_driver_module_t * self         //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_DRIVER_MODULE_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif

}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_driver_module_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

http_driver_module_t *
    http_driver_module_link_ (
    http_driver_module_t * self,        //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
    int
        history_last;
#endif

    if (self) {
        HTTP_DRIVER_MODULE_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = "link";
        self->history_links [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    http_driver_module_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    http_driver_module_unlink_ (
    http_driver_module_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
    int
        history_last;
#endif

    http_driver_module_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        HTTP_DRIVER_MODULE_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("http_driver_module", "E: missing link on http_driver_module object");
            http_driver_module_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_HTTP_DRIVER_MODULE)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % HTTP_DRIVER_MODULE_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            http_driver_module_annihilate_ (self_p, file, line);
        http_driver_module_free_ ((http_driver_module_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_driver_module_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_driver_module_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_driver_module_t));
icl_system_register (http_driver_module_cache_purge, http_driver_module_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_driver_module_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_driver_module_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_driver_module_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_driver_module_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_driver_module_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_driver_module_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_driver_module_animating = enabled;
}
/*  -------------------------------------------------------------------------
    http_driver_module_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_driver_module_new_in_scope_ (
    http_driver_module_t * * self_p,    //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    void * server_object,               //  Not documented
    untyped_link_fn * server_link,      //  Not documented
    untyped_unlink_fn * server_unlink,   //  Not documented
    char * path                         //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_driver_module_new_ (file,line,server_object,server_link,server_unlink,path);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_driver_module_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__http_driver_module_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__http_driver_module_component  = "http_driver_module ";
char *EMBED__http_driver_module_version   = "1.1 ";
char *EMBED__http_driver_module_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_driver_module_filename  = "http_driver_module.icl ";
char *EMBED__http_driver_module_builddate  = "2011/03/01 ";
char *EMBED__http_driver_module_version_end  = "VeRsIoNeNd:ipc";

