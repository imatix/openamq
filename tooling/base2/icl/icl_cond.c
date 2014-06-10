/*---------------------------------------------------------------------------
    icl_cond.c - icl_cond component

    Defines wrappers to manage condition variable objects.
    Generated from icl_cond.icl by icl_gen using GSL/4.
    
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
#include "icl_cond.h"                   //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_cond_t

//  Shorthands for class methods

#define self_alloc          icl_cond_alloc
#define self_free           icl_cond_free
#define self_signal         icl_cond_signal
#define self_broadcast      icl_cond_broadcast
#define self_wait           icl_cond_wait
#define self_timed_wait     icl_cond_timed_wait
#define self_selftest       icl_cond_selftest
#define self_terminate      icl_cond_terminate
#define self_new            icl_cond_new
#define self_show           icl_cond_show
#define self_destroy        icl_cond_destroy
#define self_annihilate     icl_cond_annihilate
#define self_show_animation  icl_cond_show_animation
#define self_new_in_scope   icl_cond_new_in_scope

#define icl_cond_alloc()                icl_cond_alloc_ (__FILE__, __LINE__)
static icl_cond_t *
    icl_cond_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_cond_free (
icl_cond_t * self                       //  Object reference
);

static void
    icl_cond_annihilate (
icl_cond_t * ( * self_p )               //  Reference to object reference
);

Bool
    icl_cond_animating = TRUE;          //  Animation enabled by default

/*  -------------------------------------------------------------------------
    icl_cond_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_cond_t *
    icl_cond_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_cond_t *
        self = NULL;                    //  Object reference

self = (icl_cond_t *) malloc (sizeof (icl_cond_t));
if (self)
    memset (self, 0, sizeof (icl_cond_t));

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "icl_cond_alloc");
    icl_apr_assert (apr_thread_cond_create (&self->cond, self->pool));
#endif


    return (self);
}
/*  -------------------------------------------------------------------------
    icl_cond_free

    Type: Component method
    Freess a icl_cond_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_cond_free (
    icl_cond_t * self                   //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_cond_destroy (self->cond));
    self->cond = NULL;
    apr_pool_destroy (self->pool);
    self->pool = NULL;
#endif
        memset (&self->object_tag, 0, sizeof (icl_cond_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_cond_t));
        self->object_tag = ICL_COND_DEAD;
        free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_cond_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cond_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_cond_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cond_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_cond_new

    Type: Component method
    Creates and initialises a new icl_cond_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_cond_t *
    icl_cond_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_cond_t *
        self = NULL;                    //  Object reference

self = icl_cond_alloc_ (file, line);
if (self) {
    self->object_tag   = ICL_COND_ALIVE;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_cond_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cond_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_cond_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_cond file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_cond_destroy

    Type: Component method
    Destroys a icl_cond_t object. Takes the address of a
    icl_cond_t reference (a pointer to a pointer) and nullifies the
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
    icl_cond_destroy_ (
    icl_cond_t * ( * self_p ),          //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_cond_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_cond_annihilate (self_p);
    icl_cond_free ((icl_cond_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_cond_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_cond_annihilate (
    icl_cond_t * ( * self_p )           //  Reference to object reference
)
{

    icl_cond_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_COND_ASSERT_SANE (self);


}
/*  -------------------------------------------------------------------------
    icl_cond_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_cond_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_cond_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_cond_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cond_new_in_scope_ (
    icl_cond_t * * self_p,              //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_cond_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_cond_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_cond_version_start       = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_cond_component           = "icl_cond ";
char *EMBED__icl_cond_version             = "1.0 ";
char *EMBED__icl_cond_copyright           = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_cond_filename            = "icl_cond.icl ";
char *EMBED__icl_cond_builddate           = "2011/03/01 ";
char *EMBED__icl_cond_version_end         = "VeRsIoNeNd:ipc";

