/*---------------------------------------------------------------------------
    icl_demo_queue_queue.c - icl_demo_queue_queue component

    This class implements the queue container for icl_demo_queue
    Generated from icl_demo_queue_queue.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_queue_queue.h"       //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_queue_queue_t

//  Shorthands for class methods

#define self_new            icl_demo_queue_queue_new
#define self_annihilate     icl_demo_queue_queue_annihilate
#define self_selftest       icl_demo_queue_queue_selftest
#define self_free           icl_demo_queue_queue_free
#define self_pop            icl_demo_queue_queue_pop
#define self_queue          icl_demo_queue_queue_queue
#define self_terminate      icl_demo_queue_queue_terminate
#define self_show           icl_demo_queue_queue_show
#define self_destroy        icl_demo_queue_queue_destroy
#define self_alloc          icl_demo_queue_queue_alloc
#define self_cache_initialise  icl_demo_queue_queue_cache_initialise
#define self_cache_purge    icl_demo_queue_queue_cache_purge
#define self_cache_terminate  icl_demo_queue_queue_cache_terminate
#define self_show_animation  icl_demo_queue_queue_show_animation
#define self_new_in_scope   icl_demo_queue_queue_new_in_scope

static void
    icl_demo_queue_queue_annihilate (
icl_demo_queue_queue_t * ( * self_p )   //  Reference to object reference
);

static void
    icl_demo_queue_queue_free (
icl_demo_queue_queue_t * self           //  Object reference
);

#define icl_demo_queue_queue_alloc()    icl_demo_queue_queue_alloc_ (__FILE__, __LINE__)
static icl_demo_queue_queue_t *
    icl_demo_queue_queue_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_demo_queue_queue_cache_initialise (
void);

static void
    icl_demo_queue_queue_cache_purge (
void);

static void
    icl_demo_queue_queue_cache_terminate (
void);

Bool
    icl_demo_queue_queue_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_new

    Type: Component method
    Creates and initialises a new icl_demo_queue_queue_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_queue_queue_t *
    icl_demo_queue_queue_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_demo_queue_queue_t *
        self = NULL;                    //  Object reference

    self = icl_demo_queue_queue_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_DEMO_QUEUE_QUEUE_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_demo_queue_queue_show_);
#endif

#if (defined (BASE_THREADSAFE))
    self->mutex = icl_mutex_new ();
#endif

    self->head = NULL;
    self->tail = NULL;


}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_annihilate

    Type: Component method
    Destroys the queue and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_queue_queue_annihilate (
    icl_demo_queue_queue_t * ( * self_p )  //  Reference to object reference
)
{

    icl_demo_queue_t
        *item;
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    icl_demo_queue_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_DEMO_QUEUE_QUEUE_ASSERT_SANE (self);

#if defined (BASE_THREADSAFE)
    mutex = self->mutex;
    if (mutex)
         icl_mutex_lock (mutex);
#endif


    item = (icl_demo_queue_t *) self->head;
    while (item != NULL) {
        //  Unlink child without lock (we already have the lock)
        self->head = item->queue_next;

        icl_demo_queue_unlink (&item);
        item = (icl_demo_queue_t *) self->head;
    }
#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_queue_queue_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_free

    Type: Component method
    Freess a icl_demo_queue_queue_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_queue_queue_free (
    icl_demo_queue_queue_t * self       //  Object reference
)
{


if (self) {

#if defined (BASE_THREADSAFE)
    if (self->mutex)
        icl_mutex_destroy (&self->mutex);
#endif
        memset (&self->object_tag, 0, sizeof (icl_demo_queue_queue_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_queue_queue_t));
        self->object_tag = ICL_DEMO_QUEUE_QUEUE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_pop

    Type: Component method
    Removes the next item in the queue, if any, and returns it.  If
    the queue was empty, returns NULL.

    The number of links is unchanged; removing from the queue and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

icl_demo_queue_t *
    icl_demo_queue_queue_pop (
    icl_demo_queue_queue_t * self       //  The queue
)
{
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    icl_demo_queue_t *
        item = NULL;                    //  Not documented

    ICL_DEMO_QUEUE_QUEUE_ASSERT_SANE (self);
#if defined (BASE_THREADSAFE)
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

        item = (icl_demo_queue_t *) self->head;
        if (item) {
            self->head = item->queue_next;
            if (self->head == NULL)
                self->tail = NULL;
            item->queue_next = NULL;
            item->queue_head = NULL;
        }

        if (item) {
        }

#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_queue

    Type: Component method
    Attaches the item to the end of the specified queue.  The item may not
    be in a queue of the same class.

    The number of links is automatically adjusted - if the item was already
    in the queue, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    icl_demo_queue_queue_queue (
    icl_demo_queue_queue_t * self,      //  Reference to object
    icl_demo_queue_t * item             //  Not documented
)
{
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    int
        rc = 0;                         //  Return code

#if defined (BASE_THREADSAFE)
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

    ICL_DEMO_QUEUE_QUEUE_ASSERT_SANE (self);

        if (! item->queue_head) {
            icl_demo_queue_link (item);

            if (self->tail)
                self->tail->queue_next = item;
            else
                self->head = item;
            self->tail = item;
            item->queue_next = NULL;
            item->queue_head = self;
        }
        else {
            //  Requeuing into the same queue isn't allowed.
            icl_console_print ("E: icl_demo_queue item inserted into multiple queue containers");
            assert (item->queue_head == NULL);
        }



#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_queue_queue_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_queue_queue_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_demo_queue_queue_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_demo_queue_queue file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_destroy

    Type: Component method
    Destroys a icl_demo_queue_queue_t object. Takes the address of a
    icl_demo_queue_queue_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_queue_queue_destroy_ (
    icl_demo_queue_queue_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_queue_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_demo_queue_queue_annihilate (self_p);
    icl_demo_queue_queue_free ((icl_demo_queue_queue_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_queue_queue_t *
    icl_demo_queue_queue_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_demo_queue_queue_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    icl_demo_queue_queue_cache_initialise ();

self = (icl_demo_queue_queue_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (icl_demo_queue_queue_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_queue_queue_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (icl_demo_queue_queue_t));
icl_system_register (icl_demo_queue_queue_cache_purge, icl_demo_queue_queue_cache_terminate);
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_queue_queue_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_queue_queue_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_queue_queue_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_queue_queue_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_queue_queue_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_queue_queue_new_in_scope_ (
    icl_demo_queue_queue_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_queue_queue_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_queue_queue_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_demo_queue_queue_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_queue_queue_component  = "icl_demo_queue_queue ";
char *EMBED__icl_demo_queue_queue_version  = "1.0 ";
char *EMBED__icl_demo_queue_queue_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_queue_queue_filename  = "icl_demo_queue_queue.icl ";
char *EMBED__icl_demo_queue_queue_builddate  = "2011/03/01 ";
char *EMBED__icl_demo_queue_queue_version_end  = "VeRsIoNeNd:ipc";

