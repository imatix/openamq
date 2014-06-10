/*---------------------------------------------------------------------------
    icl_mutex.c - icl_mutex component

    Defines wrappers to manage mutex objects.
    Generated from icl_mutex.icl by icl_gen using GSL/4.
    
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
#include "icl_mutex.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_mutex_t

//  Shorthands for class methods

#define self_alloc          icl_mutex_alloc
#define self_free           icl_mutex_free
#define self_lock           icl_mutex_lock
#define self_unlock         icl_mutex_unlock
#define self_selftest       icl_mutex_selftest
#define self_terminate      icl_mutex_terminate
#define self_new            icl_mutex_new
#define self_show           icl_mutex_show
#define self_destroy        icl_mutex_destroy
#define self_annihilate     icl_mutex_annihilate
#define self_show_animation  icl_mutex_show_animation
#define self_new_in_scope   icl_mutex_new_in_scope

#define icl_mutex_alloc()               icl_mutex_alloc_ (__FILE__, __LINE__)
static icl_mutex_t *
    icl_mutex_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_mutex_free (
icl_mutex_t * self                      //  Object reference
);

static void
    icl_mutex_annihilate (
icl_mutex_t * ( * self_p )              //  Reference to object reference
);

Bool
    icl_mutex_animating = TRUE;         //  Animation enabled by default

/*  -------------------------------------------------------------------------
    icl_mutex_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_mutex_t *
    icl_mutex_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_mutex_t *
        self = NULL;                    //  Object reference

self = (icl_mutex_t *) malloc (sizeof (icl_mutex_t));
if (self)
    memset (self, 0, sizeof (icl_mutex_t));

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "icl_mutex_alloc");
    icl_apr_assert (apr_thread_mutex_create (&self->mutex, 
                    APR_THREAD_MUTEX_DEFAULT, self->pool));
#endif


    return (self);
}
/*  -------------------------------------------------------------------------
    icl_mutex_free

    Type: Component method
    Freess a icl_mutex_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_mutex_free (
    icl_mutex_t * self                  //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_mutex_destroy (self->mutex));
    self->mutex = NULL;
    apr_pool_destroy (self->pool);
    self->pool = NULL;
#endif
        memset (&self->object_tag, 0, sizeof (icl_mutex_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_mutex_t));
        self->object_tag = ICL_MUTEX_DEAD;
        free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_mutex_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mutex_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_mutex_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mutex_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_mutex_new

    Type: Component method
    Creates and initialises a new icl_mutex_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_mutex_t *
    icl_mutex_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_mutex_t *
        self = NULL;                    //  Object reference

self = icl_mutex_alloc_ (file, line);
if (self) {
    self->object_tag   = ICL_MUTEX_ALIVE;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_mutex_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mutex_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_mutex_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_mutex file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_mutex_destroy

    Type: Component method
    Destroys a icl_mutex_t object. Takes the address of a
    icl_mutex_t reference (a pointer to a pointer) and nullifies the
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
    icl_mutex_destroy_ (
    icl_mutex_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_mutex_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_mutex_annihilate (self_p);
    icl_mutex_free ((icl_mutex_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_mutex_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_mutex_annihilate (
    icl_mutex_t * ( * self_p )          //  Reference to object reference
)
{

    icl_mutex_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_MUTEX_ASSERT_SANE (self);


}
/*  -------------------------------------------------------------------------
    icl_mutex_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_mutex_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_mutex_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_mutex_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mutex_new_in_scope_ (
    icl_mutex_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_mutex_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_mutex_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_mutex_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_mutex_component          = "icl_mutex ";
char *EMBED__icl_mutex_version            = "1.0 ";
char *EMBED__icl_mutex_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_mutex_filename           = "icl_mutex.icl ";
char *EMBED__icl_mutex_builddate          = "2011/03/01 ";
char *EMBED__icl_mutex_version_end        = "VeRsIoNeNd:ipc";

