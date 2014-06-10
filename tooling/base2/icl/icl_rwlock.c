/*---------------------------------------------------------------------------
    icl_rwlock.c - icl_rwlock component

    Defines wrappers to managed read-write lock objects.
    Generated from icl_rwlock.icl by icl_gen using GSL/4.
    
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
#include "icl_rwlock.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_rwlock_t

//  Shorthands for class methods

#define self_alloc          icl_rwlock_alloc
#define self_free           icl_rwlock_free
#define self_cache_initialise  icl_rwlock_cache_initialise
#define self_read_lock      icl_rwlock_read_lock
#define self_write_lock     icl_rwlock_write_lock
#define self_unlock         icl_rwlock_unlock
#define self_selftest       icl_rwlock_selftest
#define self_terminate      icl_rwlock_terminate
#define self_new            icl_rwlock_new
#define self_show           icl_rwlock_show
#define self_destroy        icl_rwlock_destroy
#define self_annihilate     icl_rwlock_annihilate
#define self_cache_purge    icl_rwlock_cache_purge
#define self_cache_terminate  icl_rwlock_cache_terminate
#define self_show_animation  icl_rwlock_show_animation
#define self_new_in_scope   icl_rwlock_new_in_scope

#define icl_rwlock_alloc()              icl_rwlock_alloc_ (__FILE__, __LINE__)
static icl_rwlock_t *
    icl_rwlock_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_rwlock_free (
icl_rwlock_t * self                     //  Object reference
);

static void
    icl_rwlock_cache_initialise (
void);

static void
    icl_rwlock_annihilate (
icl_rwlock_t * ( * self_p )             //  Reference to object reference
);

static void
    icl_rwlock_cache_purge (
void);

static void
    icl_rwlock_cache_terminate (
void);

Bool
    icl_rwlock_animating = TRUE;        //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    icl_rwlock_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_rwlock_t *
    icl_rwlock_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_rwlock_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    icl_rwlock_cache_initialise ();

self = (icl_rwlock_t *) icl_mem_cache_pop_ (s_cache, file, line);
if (!self) {
    self = (icl_rwlock_t *) icl_mem_cache_new_ (s_cache, file, line);

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag   (self->pool, "icl_rwlock_alloc");
    icl_apr_assert (apr_thread_rwlock_create (&self->rwlock, self->pool));
#endif
}


    return (self);
}
/*  -------------------------------------------------------------------------
    icl_rwlock_free

    Type: Component method
    Freess a icl_rwlock_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_rwlock_free (
    icl_rwlock_t * self                 //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (icl_mem_allocator == ICL_MEM_DIRECT) {
        icl_apr_assert (apr_thread_rwlock_destroy (self->rwlock));
        apr_pool_destroy (self->pool);
    }
#endif
    self->object_tag = ICL_RWLOCK_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_rwlock_cache_initialise

    Type: Component method
    We need a private cache since we're going to re-use the apr_rwlock fields.
    -------------------------------------------------------------------------
 */

static void
    icl_rwlock_cache_initialise (
void)
{

s_cache = icl_cache_create (sizeof (icl_rwlock_t));
icl_system_register (icl_rwlock_cache_purge, icl_rwlock_cache_terminate);
}
/*  -------------------------------------------------------------------------
    icl_rwlock_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_rwlock_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_rwlock_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_rwlock_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_rwlock_new

    Type: Component method
    Creates and initialises a new icl_rwlock_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_rwlock_t *
    icl_rwlock_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_rwlock_t *
        self = NULL;                    //  Object reference

    self = icl_rwlock_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_RWLOCK_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_rwlock_show_);
#endif

}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_rwlock_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_rwlock_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_rwlock_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_rwlock file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_rwlock_destroy

    Type: Component method
    Destroys a icl_rwlock_t object. Takes the address of a
    icl_rwlock_t reference (a pointer to a pointer) and nullifies the
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
    icl_rwlock_destroy_ (
    icl_rwlock_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_rwlock_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_rwlock_annihilate (self_p);
    icl_rwlock_free ((icl_rwlock_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_rwlock_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_rwlock_annihilate (
    icl_rwlock_t * ( * self_p )         //  Reference to object reference
)
{

    icl_rwlock_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_RWLOCK_ASSERT_SANE (self);


}
/*  -------------------------------------------------------------------------
    icl_rwlock_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_rwlock_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    icl_rwlock_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_rwlock_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    icl_rwlock_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_rwlock_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_rwlock_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_rwlock_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_rwlock_new_in_scope_ (
    icl_rwlock_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_rwlock_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_rwlock_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_rwlock_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_rwlock_component         = "icl_rwlock ";
char *EMBED__icl_rwlock_version           = "1.2 ";
char *EMBED__icl_rwlock_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_rwlock_filename          = "icl_rwlock.icl ";
char *EMBED__icl_rwlock_builddate         = "2011/03/01 ";
char *EMBED__icl_rwlock_version_end       = "VeRsIoNeNd:ipc";

