/*---------------------------------------------------------------------------
    smt_demo_portal_server.c - smt_demo_portal_server component

    Generated from smt_demo_portal_server.icl by smt_object_gen using GSL/4.
    
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
#include "smt_demo_portal_server.h"     //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_demo_portal_server_t

//  Shorthands for class methods

#define self_selftest       smt_demo_portal_server_selftest
#define self_print          smt_demo_portal_server_print
#define self_new            smt_demo_portal_server_new
#define self_annihilate     smt_demo_portal_server_annihilate
#define self_initialise     smt_demo_portal_server_initialise
#define self_terminate      smt_demo_portal_server_terminate
#define self__smt_demo_portal_new  smt_demo_portal_server__smt_demo_portal_new
#define self__smt_demo_portal_factory  smt_demo_portal_server__smt_demo_portal_factory
#define self__smt_demo_portal_unbind_shim  smt_demo_portal_server__smt_demo_portal_unbind_shim
#define self__smt_demo_portal_unbind  smt_demo_portal_server__smt_demo_portal_unbind
#define self_hello_shim     smt_demo_portal_server_hello_shim
#define self_hello          smt_demo_portal_server_hello
#define self_print_shim     smt_demo_portal_server_print_shim
#define self__smt_demo_portal_link  smt_demo_portal_server__smt_demo_portal_link
#define self__smt_demo_portal_unlink  smt_demo_portal_server__smt_demo_portal_unlink
#define self_destroy        smt_demo_portal_server_destroy
#define self_unlink         smt_demo_portal_server_unlink
#define self_show_animation  smt_demo_portal_server_show_animation
#define self_free           smt_demo_portal_server_free
#define self_show           smt_demo_portal_server_show
#define self_alloc          smt_demo_portal_server_alloc
#define self_link           smt_demo_portal_server_link
#define self_cache_initialise  smt_demo_portal_server_cache_initialise
#define self_cache_purge    smt_demo_portal_server_cache_purge
#define self_cache_terminate  smt_demo_portal_server_cache_terminate
#define self_new_in_scope   smt_demo_portal_server_new_in_scope

#define smt_demo_portal_server_annihilate(self)  smt_demo_portal_server_annihilate_ (self, __FILE__, __LINE__)
static int
    smt_demo_portal_server_annihilate_ (
smt_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_demo_portal_server_initialise (
void);

static void
    smt_demo_portal_server__smt_demo_portal_unbind_shim (
void * self_v,                          //  Self as void pointer
smt_demo_portal_t * portal              //  Portal reference
);

static int
    smt_demo_portal_server_hello_shim (
void * self_v,                          //  Self as void pointer
smt_demo_portal_t * portal              //  Portal back to server
);

static int
    smt_demo_portal_server_print_shim (
void * self_v,                          //  Self as void pointer
smt_demo_portal_t * portal,             //  Portal back to server
char * string                           //  Not documented
);

static void *
    smt_demo_portal_server__smt_demo_portal_link (
void * object                           //  Reference cast as a void *
);

static void
    smt_demo_portal_server__smt_demo_portal_unlink (
void * object_p                         //  Reference pointer cast as a void *
);

#define smt_demo_portal_server_alloc()  smt_demo_portal_server_alloc_ (__FILE__, __LINE__)
static smt_demo_portal_server_t *
    smt_demo_portal_server_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_demo_portal_server_cache_initialise (
void);

static void
    smt_demo_portal_server_cache_purge (
void);

static void
    smt_demo_portal_server_cache_terminate (
void);

Bool
    smt_demo_portal_server_animating = TRUE;  //  Animation enabled by default
static Bool
    s_smt_demo_portal_server_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_smt_demo_portal_server_mutex  = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static ipr_looseref_list_t
    *s_smt_demo_portal_list;            //  List of portal factories
static Bool
    s_smt_demo_portal_stopped;          //  Is this portal factory stopped?
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_demo_portal_server_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_print

    Type: Component method
    Accepts a smt_demo_portal_server_t reference and returns zero in case of success,
    1 in case of errors.
    This is the print incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

int
    smt_demo_portal_server_print (
    smt_demo_portal_server_t * self,    //  Reference to object
    smt_demo_portal_t * portal,         //  Portal back to client
    char * string                       //  Not documented
)
{
    int
        rc = 0;                         //  Return code

SMT_DEMO_PORTAL_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (smt_demo_portal_server_agent_print (self->thread,portal,string)) {
        //icl_console_print ("Error sending 'print' method to smt_demo_portal_server agent");
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
    smt_demo_portal_server_new

    Type: Component method
    Creates and initialises a new smt_demo_portal_server_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_demo_portal_server_t *
    smt_demo_portal_server_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_demo_portal_server_t *
        self = NULL;                    //  Object reference

if (!s_smt_demo_portal_server_active)
    self_initialise ();
    self = smt_demo_portal_server_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_DEMO_PORTAL_SERVER_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_demo_portal_server_show_);
#endif

self->thread = smt_demo_portal_server_agent_class_thread_new (self);
self->thread->animate = TRUE;

self->smt_demo_portal_list = ipr_looseref_list_new ();
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    smt_demo_portal_server_annihilate_ (
    smt_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif

    smt_demo_portal_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (smt_demo_portal_server_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to smt_demo_portal_server agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_demo_portal_server_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_smt_demo_portal_server_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_smt_demo_portal_server_mutex)
            s_smt_demo_portal_server_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_smt_demo_portal_server_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_smt_demo_portal_server_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

smt_demo_portal_server_agent_init ();

s_smt_demo_portal_list = ipr_looseref_list_new ();
            s_smt_demo_portal_server_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_smt_demo_portal_server_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_demo_portal_server_terminate (
void)
{
smt_demo_portal_t
    *smt_demo_portal;               //  Portal factory object

if (s_smt_demo_portal_server_active) {

//
if (icl_atomic_cas32 ((volatile qbyte *) &s_smt_demo_portal_stopped, TRUE, FALSE) == FALSE) {
    while ((smt_demo_portal = (smt_demo_portal_t *) ipr_looseref_pop (s_smt_demo_portal_list))) {
        smt_demo_portal->server_looseref = NULL;
        smt_demo_portal_destroy (&smt_demo_portal);
    }
}
ipr_looseref_list_destroy (&s_smt_demo_portal_list);
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_smt_demo_portal_server_mutex);
#endif
        s_smt_demo_portal_server_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server__smt_demo_portal_new

    Type: Component method
    Creates and initialises a new portal instance.  If the server argument is
    null, creates a hidden server object that can only be referenced through
    the portal.  This is recommended for most purposes.
    -------------------------------------------------------------------------
 */

smt_demo_portal_t *
    smt_demo_portal_server__smt_demo_portal_new (
    void * self_v,                      //  Untyped server reference
    char * name                         //  Not documented
)
{
smt_demo_portal_server_t
    *self;
    smt_demo_portal_t *
        smt_demo_portal;                //  Not documented

//
//  Grab a link to a new or the provided server object
if (self_v)
    self = self_link ((smt_demo_portal_server_t *) self_v);
else
    self = self_new ();

smt_demo_portal = smt_demo_portal_new (self, self__smt_demo_portal_link, self__smt_demo_portal_unlink, name);
smt_demo_portal->server_looseref = ipr_looseref_queue (
    self->smt_demo_portal_list, smt_demo_portal_link (smt_demo_portal));

//  Build virtual methods for server-side methods
smt_demo_portal->server_unbind = self__smt_demo_portal_unbind_shim;
smt_demo_portal->request_hello = self_hello_shim;
smt_demo_portal->request_print = self_print_shim;

//  Drop link to server object
self_unlink (&self);

    return (smt_demo_portal);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server__smt_demo_portal_factory

    Type: Component method
    Creates and initialises a new portal factory instance.
    -------------------------------------------------------------------------
 */

smt_demo_portal_t *
    smt_demo_portal_server__smt_demo_portal_factory (
    char * name                         //  Not documented
)
{
    smt_demo_portal_t *
        smt_demo_portal;                //  Not documented

//
if (!s_smt_demo_portal_server_active)
    self_initialise ();             //  Since we don't call smt_demo_portal_server_new
smt_demo_portal = smt_demo_portal_new (NULL, self__smt_demo_portal_link, self__smt_demo_portal_unlink, name);
smt_demo_portal->server_looseref = ipr_looseref_queue (
    s_smt_demo_portal_list, smt_demo_portal_link (smt_demo_portal));

//  Build virtual methods for server-side methods
smt_demo_portal->server_new = self__smt_demo_portal_new;

    return (smt_demo_portal);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server__smt_demo_portal_unbind_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_demo_portal_server__smt_demo_portal_unbind_shim (
    void * self_v,                      //  Self as void pointer
    smt_demo_portal_t * portal          //  Portal reference
)
{

self__smt_demo_portal_unbind ((smt_demo_portal_server_t *) self_v, portal);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server__smt_demo_portal_unbind

    Type: Component method
    Accepts a smt_demo_portal_server_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Server-to-portal unbind.
    -------------------------------------------------------------------------
 */

int
    smt_demo_portal_server__smt_demo_portal_unbind (
    smt_demo_portal_server_t * self,    //  Reference to object
    smt_demo_portal_t * portal          //  Portal reference
)
{
    int
        rc = 0;                         //  Return code

SMT_DEMO_PORTAL_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

    //

if (self) {
    assert (self->thread);
    if (smt_demo_portal_server_agent__smt_demo_portal_unbind (self->thread,portal)) {
        //icl_console_print ("Error sending '_smt_demo_portal unbind' method to smt_demo_portal_server agent");
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
    smt_demo_portal_server_hello_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    smt_demo_portal_server_hello_shim (
    void * self_v,                      //  Self as void pointer
    smt_demo_portal_t * portal          //  Portal back to server
)
{
    int
        rc = 0;                         //  Return code

rc = self_hello ((smt_demo_portal_server_t *) self_v, portal);

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_hello

    Type: Component method
    Accepts a smt_demo_portal_server_t reference and returns zero in case of success,
    1 in case of errors.
    This is the hello incoming request method, which the server class should
    inherit and provide the body for.  This method is invoked by the portal when
    the client makes the request; it is a private method and not accessible as
    part of the normal class API for this object.
    -------------------------------------------------------------------------
 */

int
    smt_demo_portal_server_hello (
    smt_demo_portal_server_t * self,    //  Reference to object
    smt_demo_portal_t * portal          //  Portal back to client
)
{
    int
        rc = 0;                         //  Return code

SMT_DEMO_PORTAL_SERVER_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (smt_demo_portal_server_agent_hello (self->thread,portal)) {
        //icl_console_print ("Error sending 'hello' method to smt_demo_portal_server agent");
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
    smt_demo_portal_server_print_shim

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    smt_demo_portal_server_print_shim (
    void * self_v,                      //  Self as void pointer
    smt_demo_portal_t * portal,         //  Portal back to server
    char * string                       //  Not documented
)
{
    int
        rc = 0;                         //  Return code

rc = self_print ((smt_demo_portal_server_t *) self_v, portal, string);

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server__smt_demo_portal_link

    Type: Component method
    -------------------------------------------------------------------------
 */

static void *
    smt_demo_portal_server__smt_demo_portal_link (
    void * object                       //  Reference cast as a void *
)
{

//
object = (void *) smt_demo_portal_server_link (((smt_demo_portal_server_t *) object));

    return (object);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server__smt_demo_portal_unlink

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_demo_portal_server__smt_demo_portal_unlink (
    void * object_p                     //  Reference pointer cast as a void *
)
{

//
smt_demo_portal_server_unlink (((smt_demo_portal_server_t **) object_p));
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    smt_demo_portal_server_destroy_ (
    smt_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_demo_portal_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        rc = smt_demo_portal_server_annihilate_ (self_p, file, line);
    else
    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_demo_portal_server_free (self);
    *self_p = NULL;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_demo_portal_server_unlink_ (
    smt_demo_portal_server_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif
    smt_demo_portal_server_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on smt_demo_portal_server object");
            smt_demo_portal_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                smt_demo_portal_server_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                smt_demo_portal_server_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_demo_portal_server_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_demo_portal_server_animating = enabled;

smt_demo_portal_server_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_free

    Type: Component method
    Freess a smt_demo_portal_server_t object.
    -------------------------------------------------------------------------
 */

void
    smt_demo_portal_server_free_ (
    smt_demo_portal_server_t * self,    //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);
        memset (&self->object_tag, 0, sizeof (smt_demo_portal_server_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_demo_portal_server_t));
        self->object_tag = SMT_DEMO_PORTAL_SERVER_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_demo_portal_server_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_demo_portal_server_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_demo_portal_server zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
    if (self->history_last > SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH;
        self->history_last %= SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_demo_portal_server>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_demo_portal_server_t *
    smt_demo_portal_server_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_demo_portal_server_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_demo_portal_server_cache_initialise ();

self = (smt_demo_portal_server_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_demo_portal_server_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_demo_portal_server_t *
    smt_demo_portal_server_link_ (
    smt_demo_portal_server_t * self,    //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
    int
        history_last;
#endif

    if (self) {
        SMT_DEMO_PORTAL_SERVER_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_DEMO_PORTAL_SERVER)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_DEMO_PORTAL_SERVER_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_demo_portal_server_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_demo_portal_server_t));
icl_system_register (smt_demo_portal_server_cache_purge, smt_demo_portal_server_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_demo_portal_server_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_demo_portal_server_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_demo_portal_server_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_demo_portal_server_new_in_scope_ (
    smt_demo_portal_server_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_demo_portal_server_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_demo_portal_server_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_demo_portal_server_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_demo_portal_server_component  = "smt_demo_portal_server ";
char *EMBED__smt_demo_portal_server_version  = "0.0 ";
char *EMBED__smt_demo_portal_server_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_demo_portal_server_filename  = "smt_demo_portal_server.icl ";
char *EMBED__smt_demo_portal_server_builddate  = "2011/03/01 ";
char *EMBED__smt_demo_portal_server_version_end  = "VeRsIoNeNd:ipc";

