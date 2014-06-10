/*---------------------------------------------------------------------------
    icl_demo_plain.c - icl_demo_plain component

    Generated from icl_demo_plain.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_plain.h"             //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_plain_t

//  Structure of the icl_demo_plain object

struct _icl_demo_plain_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
    char * name;                        //  
    char * address;                     //  
};

//  Shorthands for class methods

#define self_new            icl_demo_plain_new
#define self_annihilate     icl_demo_plain_annihilate
#define self_selftest       icl_demo_plain_selftest
#define self_name           icl_demo_plain_name
#define self_set_name       icl_demo_plain_set_name
#define self_address        icl_demo_plain_address
#define self_set_address    icl_demo_plain_set_address
#define self_terminate      icl_demo_plain_terminate
#define self_show           icl_demo_plain_show
#define self_destroy        icl_demo_plain_destroy
#define self_alloc          icl_demo_plain_alloc
#define self_free           icl_demo_plain_free
#define self_read_lock      icl_demo_plain_read_lock
#define self_write_lock     icl_demo_plain_write_lock
#define self_unlock         icl_demo_plain_unlock
#define self_show_animation  icl_demo_plain_show_animation
#define self_new_in_scope   icl_demo_plain_new_in_scope

static void
    icl_demo_plain_annihilate (
icl_demo_plain_t * ( * self_p )         //  Reference to object reference
);

#define icl_demo_plain_alloc()          icl_demo_plain_alloc_ (__FILE__, __LINE__)
static icl_demo_plain_t *
    icl_demo_plain_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_demo_plain_free (
icl_demo_plain_t * self                 //  Object reference
);

Bool
    icl_demo_plain_animating = TRUE;    //  Animation enabled by default

/*  -------------------------------------------------------------------------
    icl_demo_plain_new

    Type: Component method
    Creates and initialises a new icl_demo_plain_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_plain_t *
    icl_demo_plain_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Not documented
    char * address                      //  Not documented
)
{
    icl_demo_plain_t *
        self = NULL;                    //  Object reference

    self = icl_demo_plain_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_DEMO_PLAIN_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_demo_plain_show_);
#endif

self->name    = icl_mem_strdup (name);
self->address = icl_mem_strdup (address);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_plain_annihilate (
    icl_demo_plain_t * ( * self_p )     //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    icl_demo_plain_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    ICL_DEMO_PLAIN_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

icl_mem_free (self->name);
icl_mem_free (self->address);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    icl_demo_plain_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_plain_selftest (
void)
{
icl_demo_plain_t *
    item;

item = icl_demo_plain_new ("John", "Main street");
icl_demo_plain_destroy (&item);
item = icl_demo_plain_new ("Janet", "Side avenue");
icl_demo_plain_destroy (&item);
icl_demo_plain_destroy (&item);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_name

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    icl_demo_plain_name (
    icl_demo_plain_t * self             //  Not documented
)
{
    char *
        name;                           //  Not documented

name = self->name;

    return (name);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_set_name

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_plain_set_name (
    icl_demo_plain_t * self,            //  Not documented
    char * name                         //  Not documented
)
{

icl_mem_free (self->name);
self->name = icl_mem_strdup (name);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_address

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    icl_demo_plain_address (
    icl_demo_plain_t * self             //  Not documented
)
{
    char *
        address;                        //  Not documented

address = self->address;

    return (address);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_set_address

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_plain_set_address (
    icl_demo_plain_t * self,            //  Not documented
    char * address                      //  Not documented
)
{

icl_mem_free (self->address);
self->address = icl_mem_strdup (address);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_plain_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_demo_plain_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_plain_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_demo_plain_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_demo_plain file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_demo_plain_destroy

    Type: Component method
    Destroys a icl_demo_plain_t object. Takes the address of a
    icl_demo_plain_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_plain_destroy_ (
    icl_demo_plain_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_plain_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_demo_plain_annihilate (self_p);
    icl_demo_plain_free ((icl_demo_plain_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_plain_t *
    icl_demo_plain_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_demo_plain_t *
        self = NULL;                    //  Object reference

self = (icl_demo_plain_t *) icl_mem_alloc_ (sizeof (icl_demo_plain_t), file, line);
if (self)
    memset (self, 0, sizeof (icl_demo_plain_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_free

    Type: Component method
    Freess a icl_demo_plain_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_plain_free (
    icl_demo_plain_t * self             //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (icl_demo_plain_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_plain_t));
        self->object_tag = ICL_DEMO_PLAIN_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_read_lock

    Type: Component method
    Accepts a icl_demo_plain_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_plain_read_lock (
    icl_demo_plain_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_PLAIN_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_write_lock

    Type: Component method
    Accepts a icl_demo_plain_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_plain_write_lock (
    icl_demo_plain_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_PLAIN_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_unlock

    Type: Component method
    Accepts a icl_demo_plain_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_plain_unlock (
    icl_demo_plain_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_PLAIN_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_plain_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_plain_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_plain_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_plain_new_in_scope_ (
    icl_demo_plain_t * * self_p,        //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name,                        //  Not documented
    char * address                      //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_plain_new_ (file,line,name,address);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_plain_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_demo_plain_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_plain_component     = "icl_demo_plain ";
char *EMBED__icl_demo_plain_version       = "1.0 ";
char *EMBED__icl_demo_plain_copyright     = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_plain_filename      = "icl_demo_plain.icl ";
char *EMBED__icl_demo_plain_builddate     = "2011/03/01 ";
char *EMBED__icl_demo_plain_version_end   = "VeRsIoNeNd:ipc";

