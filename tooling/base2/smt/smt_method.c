/*---------------------------------------------------------------------------
    smt_method.c - smt_method component

    The smt_method class is used to send and receive 'replies' (queued events
    with result code and data) for smt threads.
    Generated from smt_method.icl by icl_gen using GSL/4.
    
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
#include "smt_method.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_method_t

//  Shorthands for class methods

#define self_annihilate     smt_method_annihilate
#define self_send           smt_method_send
#define self_selftest       smt_method_selftest
#define self_new            smt_method_new
#define self_remove_from_all_containers  smt_method_remove_from_all_containers
#define self_show           smt_method_show
#define self_terminate      smt_method_terminate
#define self_destroy        smt_method_destroy
#define self_alloc          smt_method_alloc
#define self_free           smt_method_free
#define self_link           smt_method_link
#define self_unlink         smt_method_unlink
#define self_cache_initialise  smt_method_cache_initialise
#define self_cache_purge    smt_method_cache_purge
#define self_cache_terminate  smt_method_cache_terminate
#define self_show_animation  smt_method_show_animation
#define self_new_in_scope   smt_method_new_in_scope

#define smt_method_annihilate(self)     smt_method_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_method_annihilate_ (
smt_method_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_method_alloc()              smt_method_alloc_ (__FILE__, __LINE__)
static smt_method_t *
    smt_method_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_method_free(self)           smt_method_free_ (self, __FILE__, __LINE__)
static void
    smt_method_free_ (
smt_method_t * self,                    //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_method_cache_initialise (
void);

static void
    smt_method_cache_purge (
void);

static void
    smt_method_cache_terminate (
void);

Bool
    smt_method_animating = TRUE;        //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_method_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_method_annihilate_ (
    smt_method_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    int
        history_last;
#endif

    smt_method_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_METHOD_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_METHOD_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_METHOD_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_METHOD_HISTORY_LENGTH] = self->links;
#endif

    SMT_METHOD_ASSERT_SANE (self);
    smt_method_remove_from_all_containers (self);

if (self->destructor)
    (self->destructor) (self->data);
self->data = NULL;

}
/*  -------------------------------------------------------------------------
    smt_method_send

    Type: Component method
    Sends an event with accompanying result code and data pointer to a  method
    queue and call the queue's 'trigger' method.
    -------------------------------------------------------------------------
 */

int
    smt_method_send (
    smt_method_queue_t * queue,         //  Queue to send reply to
    smt_event_t event,                  //  Internal event number
    smt_priority_t priority,            //  The priority for the event
    int result,                         //  Result to return
    void * data,                        //  Data pointer
    smt_method_destructor_fn * destructor  //  Data destructor callbak
)
{
smt_method_t  *self;
    int
        rc = 0;                         //  Return value

//  Need to create the method even if the queue is null, so that destructor
//  can be called.
self = smt_method_new ();
self->event      = event;
self->priority   = priority;
self->result     = result;
self->data       = data;
self->destructor = destructor;

queue = smt_method_queue_link (queue);  //  So can't be freed
if (queue) {                            //  Might already be zombie
    if (smt_method_queue_queue (queue, self) == 0) {
        smt_method_unlink (&self);
        smt_method_queue_trigger (queue, SMT_METHOD_QUEUE_INSERT);
    }
    else {
        smt_method_destroy (&self);
        rc = -1;
    }
    smt_method_queue_unlink (&queue);
}
else {
    smt_method_destroy (&self);
    rc = -1;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_method_new

    Type: Component method
    Creates and initialises a new smt_method_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_method_t *
        self = NULL;                    //  Object reference

    self = smt_method_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_METHOD_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_method_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
self->queue_head = NULL;
self->queue_next = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_remove_from_all_containers

    Type: Component method
    This is just a stub.  Objects cannot be removed from a queue except by
    being popped.
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    smt_method_remove_from_all_containers (
    smt_method_t * self                 //  The itemThe item
)
{

smt_method_list_remove (self);
}
/*  -------------------------------------------------------------------------
    smt_method_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_method_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
#if !defined (BASE_THREADSAFE)
     if (self->queue_head)
        container_links++;
#endif
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_method zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    if (self->history_last > SMT_METHOD_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_METHOD_HISTORY_LENGTH;
        self->history_last %= SMT_METHOD_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_METHOD_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_method>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_method_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_method_destroy

    Type: Component method
    Destroys a smt_method_t object. Takes the address of a
    smt_method_t reference (a pointer to a pointer) and nullifies the
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
    smt_method_destroy_ (
    smt_method_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_method_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_method_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_method", "E: missing link on smt_method object");
        smt_method_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_method_free_ ((smt_method_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_method_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_method_t *
    smt_method_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_method_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_method_cache_initialise ();

self = (smt_method_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_free

    Type: Component method
    Freess a smt_method_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_method_free_ (
    smt_method_t * self,                //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_METHOD_HISTORY_LENGTH] = self->links;
#endif

    self->object_tag = SMT_METHOD_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_method_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_link_ (
    smt_method_t * self,                //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    int
        history_last;
#endif

    if (self) {
        SMT_METHOD_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_METHOD_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_method_unlink_ (
    smt_method_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
    int
        history_last;
#endif

    smt_method_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        SMT_METHOD_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_method", "E: missing link on smt_method object");
            smt_method_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_METHOD_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_method_annihilate_ (self_p, file, line);
        smt_method_free_ ((smt_method_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_method_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_method_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_method_t));
icl_system_register (smt_method_cache_purge, smt_method_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_method_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_method_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_method_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_method_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_method_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_method_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_method_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_method_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_new_in_scope_ (
    smt_method_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_method_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_method_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_method_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_method_component         = "smt_method ";
char *EMBED__smt_method_version           = "1.0 ";
char *EMBED__smt_method_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_method_filename          = "smt_method.icl ";
char *EMBED__smt_method_builddate         = "2011/03/01 ";
char *EMBED__smt_method_version_end       = "VeRsIoNeNd:ipc";

