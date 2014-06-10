/*---------------------------------------------------------------------------
    icl_demo_scope.c - icl_demo_scope component

    Generated from icl_demo_scope.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_scope.h"             //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_scope_t

//  Shorthands for class methods

#define self_new            icl_demo_scope_new
#define self_annihilate     icl_demo_scope_annihilate
#define self_selftest       icl_demo_scope_selftest
#define self_terminate      icl_demo_scope_terminate
#define self_show           icl_demo_scope_show
#define self_destroy        icl_demo_scope_destroy
#define self_alloc          icl_demo_scope_alloc
#define self_free           icl_demo_scope_free
#define self_cache_initialise  icl_demo_scope_cache_initialise
#define self_cache_purge    icl_demo_scope_cache_purge
#define self_cache_terminate  icl_demo_scope_cache_terminate
#define self_show_animation  icl_demo_scope_show_animation
#define self_new_in_scope   icl_demo_scope_new_in_scope

static void
    icl_demo_scope_annihilate (
icl_demo_scope_t * ( * self_p )         //  Reference to object reference
);

#define icl_demo_scope_alloc()          icl_demo_scope_alloc_ (__FILE__, __LINE__)
static icl_demo_scope_t *
    icl_demo_scope_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_demo_scope_free (
icl_demo_scope_t * self                 //  Object reference
);

static void
    icl_demo_scope_cache_initialise (
void);

static void
    icl_demo_scope_cache_purge (
void);

static void
    icl_demo_scope_cache_terminate (
void);

Bool
    icl_demo_scope_animating = TRUE;    //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    icl_demo_scope_new

    Type: Component method
    Creates and initialises a new icl_demo_scope_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_scope_t *
    icl_demo_scope_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Not documented
    char * address                      //  Not documented
)
{
    icl_demo_scope_t *
        self = NULL;                    //  Object reference

    self = icl_demo_scope_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_DEMO_SCOPE_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_demo_scope_show_);
#endif

self->name    = icl_mem_strdup (name);
self->address = icl_mem_strdup (address);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_scope_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_scope_annihilate (
    icl_demo_scope_t * ( * self_p )     //  Reference to object reference
)
{

    icl_demo_scope_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_DEMO_SCOPE_ASSERT_SANE (self);

icl_mem_free (self->name);
icl_mem_free (self->address);

}
/*  -------------------------------------------------------------------------
    icl_demo_scope_selftest

    Type: Component method
    This abstract method can be inherited into a method.  When this is done,
    a scope called 'scope' is opened at the start of the method.  Objects
    within the class may then be allocated inside the scope with their
    'new in scope' method.  The scope, and thus all such objects, are destroyed
    at the end of this method.
    -------------------------------------------------------------------------
 */

void
    icl_demo_scope_selftest (
void)
{
icl_scope_t
    *scope;
icl_demo_scope_t
    *self;

scope = icl_scope_new ();
icl_demo_scope_new_in_scope (&self, scope, "Santorini", "0x012201");
icl_scope_destroy (&scope);
}
/*  -------------------------------------------------------------------------
    icl_demo_scope_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_scope_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_demo_scope_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_scope_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_demo_scope_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_demo_scope file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_demo_scope_destroy

    Type: Component method
    Destroys a icl_demo_scope_t object. Takes the address of a
    icl_demo_scope_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_scope_destroy_ (
    icl_demo_scope_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_scope_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_demo_scope_annihilate (self_p);
    icl_demo_scope_free ((icl_demo_scope_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_scope_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_scope_t *
    icl_demo_scope_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_demo_scope_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    icl_demo_scope_cache_initialise ();

self = (icl_demo_scope_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (icl_demo_scope_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_scope_free

    Type: Component method
    Freess a icl_demo_scope_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_scope_free (
    icl_demo_scope_t * self             //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (icl_demo_scope_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_scope_t));
        self->object_tag = ICL_DEMO_SCOPE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_scope_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_scope_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (icl_demo_scope_t));
icl_system_register (icl_demo_scope_cache_purge, icl_demo_scope_cache_terminate);
}
/*  -------------------------------------------------------------------------
    icl_demo_scope_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_scope_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    icl_demo_scope_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_scope_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    icl_demo_scope_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_scope_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_scope_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_scope_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_scope_new_in_scope_ (
    icl_demo_scope_t * * self_p,        //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Not documented
    char * address                      //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_scope_new_ (file,line,name,address);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_scope_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_demo_scope_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_scope_component     = "icl_demo_scope ";
char *EMBED__icl_demo_scope_version       = "1.0 ";
char *EMBED__icl_demo_scope_copyright     = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_scope_filename      = "icl_demo_scope.icl ";
char *EMBED__icl_demo_scope_builddate     = "2011/03/01 ";
char *EMBED__icl_demo_scope_version_end   = "VeRsIoNeNd:ipc";

