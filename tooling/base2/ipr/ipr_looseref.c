/*---------------------------------------------------------------------------
    ipr_looseref.c - ipr_looseref component

    Allows classes to maintain lists of references to other classes without
    creating explicitly-typed classes for each case.  Uses a void * to refer
    to the target class.
    Generated from ipr_looseref.icl by icl_gen using GSL/4.
    
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
#include "ipr_looseref.h"               //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_looseref_t

//  Shorthands for class methods

#define self_new            ipr_looseref_new
#define self_annihilate     ipr_looseref_annihilate
#define self_queue          ipr_looseref_queue
#define self_push           ipr_looseref_push
#define self_pop            ipr_looseref_pop
#define self_selftest       ipr_looseref_selftest
#define self_remove_from_all_containers  ipr_looseref_remove_from_all_containers
#define self_show           ipr_looseref_show
#define self_terminate      ipr_looseref_terminate
#define self_destroy        ipr_looseref_destroy
#define self_alloc          ipr_looseref_alloc
#define self_free           ipr_looseref_free
#define self_cache_initialise  ipr_looseref_cache_initialise
#define self_cache_purge    ipr_looseref_cache_purge
#define self_cache_terminate  ipr_looseref_cache_terminate
#define self_show_animation  ipr_looseref_show_animation
#define self_new_in_scope   ipr_looseref_new_in_scope

static void
    ipr_looseref_annihilate (
ipr_looseref_t * ( * self_p )           //  Reference to object reference
);

#define ipr_looseref_alloc()            ipr_looseref_alloc_ (__FILE__, __LINE__)
static ipr_looseref_t *
    ipr_looseref_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_looseref_free (
ipr_looseref_t * self                   //  Object reference
);

static void
    ipr_looseref_cache_initialise (
void);

static void
    ipr_looseref_cache_purge (
void);

static void
    ipr_looseref_cache_terminate (
void);

Bool
    ipr_looseref_animating = TRUE;      //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_looseref_new

    Type: Component method
    Creates and initialises a new ipr_looseref_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

ipr_looseref_t *
    ipr_looseref_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_looseref_t *
        self = NULL;                    //  Object reference

    self = ipr_looseref_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_LOOSEREF_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_looseref_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_looseref_annihilate (
    ipr_looseref_t * ( * self_p )       //  Reference to object reference
)
{

    ipr_looseref_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_LOOSEREF_ASSERT_SANE (self);
ipr_looseref_remove_from_all_containers (self);


}
/*  -------------------------------------------------------------------------
    ipr_looseref_queue

    Type: Component method
    Adds an object to the tail of the list.
    -------------------------------------------------------------------------
 */

ipr_looseref_t *
    ipr_looseref_queue (
    ipr_looseref_list_t * list,         //  List to attach to
    void * object                       //  Target object reference
)
{
    ipr_looseref_t *
        self;                           //  The item

self = ipr_looseref_new ();
if (self) {
    self->object = object;
    ipr_looseref_list_queue (list, self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_push

    Type: Component method
    Adds an object to the head of the list - a pop will remove this object.
    -------------------------------------------------------------------------
 */

ipr_looseref_t *
    ipr_looseref_push (
    ipr_looseref_list_t * list,         //  List to attach to
    void * object                       //  Target object reference
)
{
    ipr_looseref_t *
        self;                           //  The item

self = ipr_looseref_new ();
if (self) {
    self->object = object;
    ipr_looseref_list_push (list, self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_pop

    Type: Component method
    Removes an object from the head of the list.
    -------------------------------------------------------------------------
 */

void *
    ipr_looseref_pop (
    ipr_looseref_list_t * list          //  List to attach to
)
{
ipr_looseref_t *self;
    void *
        object = NULL;                  //  Target object reference

self = ipr_looseref_list_pop (list);
if (self) {
    object = self->object;
    ipr_looseref_destroy (&self);
}

    return (object);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_looseref_selftest (
void)
{
ipr_looseref_list_t
    *list;

list = ipr_looseref_list_new ();
ipr_looseref_queue (list, (void *) list);
ipr_looseref_queue (list, (void *) list);
ipr_looseref_queue (list, (void *) list);
assert (ipr_looseref_pop (list) == (void *) list);
ipr_looseref_list_destroy (&list);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_looseref_remove_from_all_containers (
    ipr_looseref_t * self               //  The item
)
{

ipr_looseref_list_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_looseref_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_looseref_t
    *self;
int
    container_links;


self = item;
container_links = 0;
if (self->list_head)
   container_links++;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_looseref file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_looseref_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_looseref_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_looseref_destroy

    Type: Component method
    Destroys a ipr_looseref_t object. Takes the address of a
    ipr_looseref_t reference (a pointer to a pointer) and nullifies the
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
    ipr_looseref_destroy_ (
    ipr_looseref_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_looseref_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_looseref_annihilate (self_p);
    ipr_looseref_free ((ipr_looseref_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_looseref_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_looseref_t *
    ipr_looseref_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_looseref_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_looseref_cache_initialise ();

self = (ipr_looseref_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_looseref_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_free

    Type: Component method
    Freess a ipr_looseref_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_looseref_free (
    ipr_looseref_t * self               //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_looseref_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_looseref_t));
        self->object_tag = IPR_LOOSEREF_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_looseref_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_looseref_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_looseref_t));
icl_system_register (ipr_looseref_cache_purge, ipr_looseref_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_looseref_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_looseref_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_looseref_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_looseref_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_looseref_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_looseref_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_looseref_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_looseref_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_looseref_new_in_scope_ (
    ipr_looseref_t * * self_p,          //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_looseref_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_looseref_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_looseref_version_start   = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_looseref_component       = "ipr_looseref ";
char *EMBED__ipr_looseref_version         = "1.0 ";
char *EMBED__ipr_looseref_copyright       = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_looseref_filename        = "ipr_looseref.icl ";
char *EMBED__ipr_looseref_builddate       = "2011/03/01 ";
char *EMBED__ipr_looseref_version_end     = "VeRsIoNeNd:ipc";

