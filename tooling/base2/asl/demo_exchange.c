/*---------------------------------------------------------------------------
    demo_exchange.c - demo_exchange component

    This class implements the ASL demo server exchange class (this is a
    fanout exchange).
    Generated from demo_exchange.icl by smt_object_gen using GSL/4.
    
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
#include "demo_exchange.h"              //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_exchange_t

//  Shorthands for class methods

#define self_new            demo_exchange_new
#define self_annihilate     demo_exchange_annihilate
#define self_search         demo_exchange_search
#define self_bind           demo_exchange_bind
#define self_publish        demo_exchange_publish
#define self_initialise     demo_exchange_initialise
#define self_terminate      demo_exchange_terminate
#define self_selftest       demo_exchange_selftest
#define self_remove_from_all_containers  demo_exchange_remove_from_all_containers
#define self_show           demo_exchange_show
#define self_destroy        demo_exchange_destroy
#define self_unlink         demo_exchange_unlink
#define self_show_animation  demo_exchange_show_animation
#define self_free           demo_exchange_free
#define self_alloc          demo_exchange_alloc
#define self_link           demo_exchange_link
#define self_cache_initialise  demo_exchange_cache_initialise
#define self_cache_purge    demo_exchange_cache_purge
#define self_cache_terminate  demo_exchange_cache_terminate
#define self_new_in_scope   demo_exchange_new_in_scope

#define demo_exchange_annihilate(self)  demo_exchange_annihilate_ (self, __FILE__, __LINE__)
static int
    demo_exchange_annihilate_ (
demo_exchange_t * ( * self_p ),         //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    demo_exchange_initialise (
void);

#define demo_exchange_alloc()           demo_exchange_alloc_ (__FILE__, __LINE__)
static demo_exchange_t *
    demo_exchange_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_exchange_cache_initialise (
void);

static void
    demo_exchange_cache_purge (
void);

static void
    demo_exchange_cache_terminate (
void);

Bool
    demo_exchange_animating = TRUE;     //  Animation enabled by default
static Bool
    s_demo_exchange_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_demo_exchange_mutex    = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static demo_exchange_table_t
    *s_demo_exchange_table;             //  The table of existing items
/*  -------------------------------------------------------------------------
    demo_exchange_new

    Type: Component method
    Creates and initialises a new demo_exchange_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

demo_exchange_t *
    demo_exchange_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Exchange name
    Bool durable,                       //  Is exchange durable?
    Bool auto_delete                    //  Auto-delete unused exchange?
)
{
#define table s_demo_exchange_table
#define key name
    demo_exchange_t *
        self = NULL;                    //  Object reference

if (!s_demo_exchange_active)
    self_initialise ();
    self = demo_exchange_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_EXCHANGE_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_exchange_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->thread = demo_exchange_agent_class_thread_new (self);
self->thread->animate = TRUE;

self->name        = icl_mem_strdup (name);
self->durable     = durable;
self->auto_delete = auto_delete;
self->queue_list  = demo_queue_list_new ();
if (table && self && demo_exchange_table_insert (table, key, self))
    demo_exchange_destroy (&self);
}

    return (self);
}
#undef table
#undef key
/*  -------------------------------------------------------------------------
    demo_exchange_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    demo_exchange_annihilate_ (
    demo_exchange_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
    int
        history_last;
#endif

    demo_exchange_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = file;
    self->history_line  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = line;
    self->history_type  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (demo_exchange_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to demo_exchange agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_exchange_search

    Type: Component method
    -------------------------------------------------------------------------
 */

demo_exchange_t *
    demo_exchange_search (
    char * name                         //  Exchange name
)
{
    demo_exchange_t *
        self;                           //  The found object

//
if (!s_demo_exchange_active)
    demo_exchange_initialise ();

self = demo_exchange_table_search (s_demo_exchange_table, name);

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_exchange_bind

    Type: Component method
    Accepts a demo_exchange_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    -------------------------------------------------------------------------
 */

int
    demo_exchange_bind (
    demo_exchange_t * self,             //  Reference to object
    demo_server_channel_t * channel,    //  The channel in question
    demo_queue_t * queue,               //  The queue to bind
    icl_longstr_t * arguments           //  Bind arguments
)
{
    int
        rc = 0;                         //  Return code

DEMO_EXCHANGE_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (demo_exchange_agent_bind (self->thread,channel,queue,arguments)) {
        //icl_console_print ("Error sending 'bind' method to demo_exchange agent");
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
    demo_exchange_publish

    Type: Component method
    Accepts a demo_exchange_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Sends the content to all registered queues.
    -------------------------------------------------------------------------
 */

int
    demo_exchange_publish (
    demo_exchange_t * self,             //  Reference to object
    demo_server_channel_t * channel,    //  The channel in question
    demo_content_basic_t * content      //  The message content
)
{
    int
        rc = 0;                         //  Return code

DEMO_EXCHANGE_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (demo_exchange_agent_publish (self->thread,channel,content)) {
        //icl_console_print ("Error sending 'publish' method to demo_exchange agent");
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
    demo_exchange_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_exchange_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_demo_exchange_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_demo_exchange_mutex)
            s_demo_exchange_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_demo_exchange_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_demo_exchange_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

demo_exchange_agent_init ();

s_demo_exchange_table = demo_exchange_table_new ();
            s_demo_exchange_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_demo_exchange_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    demo_exchange_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_exchange_terminate (
void)
{

if (s_demo_exchange_active) {

demo_exchange_table_destroy (&s_demo_exchange_table);
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_demo_exchange_mutex);
#endif
        s_demo_exchange_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    demo_exchange_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_exchange_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_exchange_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    demo_exchange_remove_from_all_containers (
    demo_exchange_t * self              //  The item
)
{

DEMO_EXCHANGE_ASSERT_SANE (self);
demo_exchange_table_remove (self);
}
/*  -------------------------------------------------------------------------
    demo_exchange_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_exchange_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_exchange_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <demo_exchange zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
    if (self->history_last > DEMO_EXCHANGE_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % DEMO_EXCHANGE_HISTORY_LENGTH;
        self->history_last %= DEMO_EXCHANGE_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % DEMO_EXCHANGE_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </demo_exchange>\n");
#endif

}
/*  -------------------------------------------------------------------------
    demo_exchange_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_exchange_destroy_ (
    demo_exchange_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_exchange_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

    if (self) {
demo_exchange_remove_from_all_containers (self);
        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            rc = demo_exchange_annihilate_ (self_p, file, line);
        else
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
            demo_exchange_free (self);
        *self_p = NULL;
    }

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_exchange_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    demo_exchange_unlink_ (
    demo_exchange_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
    int
        history_last;
#endif
    demo_exchange_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on demo_exchange object");
            demo_exchange_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                demo_exchange_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                demo_exchange_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    demo_exchange_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_exchange_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_exchange_animating = enabled;

demo_exchange_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    demo_exchange_free

    Type: Component method
    Freess a demo_exchange_t object.
    -------------------------------------------------------------------------
 */

void
    demo_exchange_free_ (
    demo_exchange_t * self,             //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = "free";
        self->history_links [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);
        memset (&self->object_tag, 0, sizeof (demo_exchange_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_exchange_t));
        self->object_tag = DEMO_EXCHANGE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_exchange_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_exchange_t *
    demo_exchange_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_exchange_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_exchange_cache_initialise ();

self = (demo_exchange_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_exchange_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_exchange_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

demo_exchange_t *
    demo_exchange_link_ (
    demo_exchange_t * self,             //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
    int
        history_last;
#endif

    if (self) {
        DEMO_EXCHANGE_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_EXCHANGE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = "link";
        self->history_links [history_last % DEMO_EXCHANGE_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_exchange_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_exchange_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_exchange_t));
icl_system_register (demo_exchange_cache_purge, demo_exchange_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_exchange_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_exchange_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_exchange_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_exchange_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_exchange_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_exchange_new_in_scope_ (
    demo_exchange_t * * self_p,         //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Exchange name
    Bool durable,                       //  Is exchange durable?
    Bool auto_delete                    //  Auto-delete unused exchange?
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_exchange_new_ (file,line,name,durable,auto_delete);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_exchange_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__demo_exchange_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_exchange_component      = "demo_exchange ";
char *EMBED__demo_exchange_version        = "1.0 ";
char *EMBED__demo_exchange_copyright      = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_exchange_filename       = "demo_exchange.icl ";
char *EMBED__demo_exchange_builddate      = "2011/03/01 ";
char *EMBED__demo_exchange_version_end    = "VeRsIoNeNd:ipc";

