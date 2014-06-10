/*---------------------------------------------------------------------------
    demo_queue.c - demo_queue component

    This class implements the server queue class, an asynchronous object
    that acts as a envelope for the separate queue managers for each
    class.
    Generated from demo_queue.icl by smt_object_gen using GSL/4.
    
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
#include "demo_queue.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_queue_t

//  Shorthands for class methods

#define self_new            demo_queue_new
#define self_annihilate     demo_queue_annihilate
#define self_free           demo_queue_free
#define self_search         demo_queue_search
#define self_publish        demo_queue_publish
#define self_get            demo_queue_get
#define self_initialise     demo_queue_initialise
#define self_terminate      demo_queue_terminate
#define self_selftest       demo_queue_selftest
#define self_remove_from_all_containers  demo_queue_remove_from_all_containers
#define self_show           demo_queue_show
#define self_destroy        demo_queue_destroy
#define self_unlink         demo_queue_unlink
#define self_show_animation  demo_queue_show_animation
#define self_alloc          demo_queue_alloc
#define self_link           demo_queue_link
#define self_cache_initialise  demo_queue_cache_initialise
#define self_cache_purge    demo_queue_cache_purge
#define self_cache_terminate  demo_queue_cache_terminate
#define self_new_in_scope   demo_queue_new_in_scope

#define demo_queue_annihilate(self)     demo_queue_annihilate_ (self, __FILE__, __LINE__)
static int
    demo_queue_annihilate_ (
demo_queue_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    demo_queue_initialise (
void);

#define demo_queue_alloc()              demo_queue_alloc_ (__FILE__, __LINE__)
static demo_queue_t *
    demo_queue_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_queue_cache_initialise (
void);

static void
    demo_queue_cache_purge (
void);

static void
    demo_queue_cache_terminate (
void);

Bool
    demo_queue_animating = TRUE;        //  Animation enabled by default
static Bool
    s_demo_queue_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_demo_queue_mutex       = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static demo_queue_table_t
    *s_demo_queue_table;                //  The table of existing items
/*  -------------------------------------------------------------------------
    demo_queue_new

    Type: Component method
    Creates and initialises a new demo_queue_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

demo_queue_t *
    demo_queue_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Queue name
    Bool durable,                       //  Is queue durable?
    Bool exclusive,                     //  Is queue exclusive?
    Bool auto_delete,                   //  Auto-delete unused queue?
    demo_server_channel_t * channel     //  Server channel
)
{
#define table s_demo_queue_table
#define key name
    demo_queue_t *
        self = NULL;                    //  Object reference

if (!s_demo_queue_active)
    self_initialise ();
    self = demo_queue_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_QUEUE_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_queue_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->thread = demo_queue_agent_class_thread_new (self);
self->thread->animate = TRUE;

self->name         = icl_mem_strdup (name);
self->durable      = durable;
self->exclusive    = exclusive;
self->auto_delete  = auto_delete;
self->content_list = demo_content_basic_list_new ();
if (exclusive)
    self->channel = demo_server_channel_link (channel);
if (table && self && demo_queue_table_insert (table, key, self))
    demo_queue_destroy (&self);
}

    return (self);
}
#undef table
#undef key
/*  -------------------------------------------------------------------------
    demo_queue_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    demo_queue_annihilate_ (
    demo_queue_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
    int
        history_last;
#endif

    demo_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = file;
    self->history_line  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = line;
    self->history_type  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % DEMO_QUEUE_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (demo_queue_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to demo_queue agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_queue_free

    Type: Component method
    Freess a demo_queue_t object.
    -------------------------------------------------------------------------
 */

void
    demo_queue_free_ (
    demo_queue_t * self,                //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = "free";
        self->history_links [history_last % DEMO_QUEUE_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);

//  Free structures that may still be used while object is zombied
icl_mem_strfree (&self->name);
        memset (&self->object_tag, 0, sizeof (demo_queue_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_queue_t));
        self->object_tag = DEMO_QUEUE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_queue_search

    Type: Component method
    -------------------------------------------------------------------------
 */

demo_queue_t *
    demo_queue_search (
    char * name                         //  Exchange name
)
{
    demo_queue_t *
        self;                           //  The found object

if (!s_demo_queue_active)
    demo_queue_initialise ();
self = demo_queue_table_search (s_demo_queue_table, name);

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_queue_publish

    Type: Component method
    Accepts a demo_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Publish message content onto queue.
    -------------------------------------------------------------------------
 */

int
    demo_queue_publish (
    demo_queue_t * self,                //  Reference to object
    demo_server_channel_t * channel,    //  Not documented
    demo_content_basic_t * content      //  Not documented
)
{
    int
        rc = 0;                         //  Return code

DEMO_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (demo_queue_agent_publish (self->thread,channel,content)) {
        //icl_console_print ("Error sending 'publish' method to demo_queue agent");
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
    demo_queue_get

    Type: Component method
    Accepts a demo_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Returns next message off queue, if any.
    -------------------------------------------------------------------------
 */

int
    demo_queue_get (
    demo_queue_t * self,                //  Reference to object
    demo_server_channel_t * channel     //  Not documented
)
{
    int
        rc = 0;                         //  Return code

DEMO_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (demo_queue_agent_get (self->thread,channel)) {
        //icl_console_print ("Error sending 'get' method to demo_queue agent");
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
    demo_queue_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_queue_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_demo_queue_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_demo_queue_mutex)
            s_demo_queue_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_demo_queue_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_demo_queue_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

demo_queue_agent_init ();

s_demo_queue_table = demo_queue_table_new ();
            s_demo_queue_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_demo_queue_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    demo_queue_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_terminate (
void)
{

if (s_demo_queue_active) {

demo_queue_table_destroy (&s_demo_queue_table);
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_demo_queue_mutex);
#endif
        s_demo_queue_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    demo_queue_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_queue_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    demo_queue_remove_from_all_containers (
    demo_queue_t * self                 //  The item
)
{

DEMO_QUEUE_ASSERT_SANE (self);
demo_queue_table_remove (self);
}
/*  -------------------------------------------------------------------------
    demo_queue_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_queue_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <demo_queue zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
    if (self->history_last > DEMO_QUEUE_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % DEMO_QUEUE_HISTORY_LENGTH;
        self->history_last %= DEMO_QUEUE_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % DEMO_QUEUE_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </demo_queue>\n");
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_queue_destroy_ (
    demo_queue_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

    if (self) {
demo_queue_remove_from_all_containers (self);
        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            rc = demo_queue_annihilate_ (self_p, file, line);
        else
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
            demo_queue_free (self);
        *self_p = NULL;
    }

    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_queue_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    demo_queue_unlink_ (
    demo_queue_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
    int
        history_last;
#endif
    demo_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on demo_queue object");
            demo_queue_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % DEMO_QUEUE_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                demo_queue_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                demo_queue_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    demo_queue_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_queue_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_queue_animating = enabled;

demo_queue_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    demo_queue_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_queue_t *
    demo_queue_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_queue_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_queue_cache_initialise ();

self = (demo_queue_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_queue_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_queue_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

demo_queue_t *
    demo_queue_link_ (
    demo_queue_t * self,                //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
    int
        history_last;
#endif

    if (self) {
        DEMO_QUEUE_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_QUEUE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_QUEUE_HISTORY_LENGTH] = "link";
        self->history_links [history_last % DEMO_QUEUE_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_queue_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_queue_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_queue_t));
icl_system_register (demo_queue_cache_purge, demo_queue_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_queue_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_queue_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_queue_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_queue_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_queue_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_new_in_scope_ (
    demo_queue_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Queue name
    Bool durable,                       //  Is queue durable?
    Bool exclusive,                     //  Is queue exclusive?
    Bool auto_delete,                   //  Auto-delete unused queue?
    demo_server_channel_t * channel     //  Server channel
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_queue_new_ (file,line,name,durable,exclusive,auto_delete,channel);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_queue_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__demo_queue_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_queue_component         = "demo_queue ";
char *EMBED__demo_queue_version           = "1.0 ";
char *EMBED__demo_queue_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_queue_filename          = "demo_queue.icl ";
char *EMBED__demo_queue_builddate         = "2011/03/01 ";
char *EMBED__demo_queue_version_end       = "VeRsIoNeNd:ipc";

