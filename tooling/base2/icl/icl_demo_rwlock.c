/*---------------------------------------------------------------------------
    icl_demo_rwlock.c - icl_demo_rwlock component

    Tests and demonstrates the iCL class framework.  This class creates
    a file object that you can open, close, and work with.
    Generated from icl_demo_rwlock.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_rwlock.h"            //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_rwlock_t

//  Shorthands for class methods

#define self_new            icl_demo_rwlock_new
#define self_annihilate     icl_demo_rwlock_annihilate
#define self_write          icl_demo_rwlock_write
#define self_read           icl_demo_rwlock_read
#define self_reset          icl_demo_rwlock_reset
#define self_selftest       icl_demo_rwlock_selftest
#define self_terminate      icl_demo_rwlock_terminate
#define self_show           icl_demo_rwlock_show
#define self_destroy        icl_demo_rwlock_destroy
#define self_alloc          icl_demo_rwlock_alloc
#define self_free           icl_demo_rwlock_free
#define self_read_lock      icl_demo_rwlock_read_lock
#define self_write_lock     icl_demo_rwlock_write_lock
#define self_unlock         icl_demo_rwlock_unlock
#define self_show_animation  icl_demo_rwlock_show_animation
#define self_new_in_scope   icl_demo_rwlock_new_in_scope

static void
    icl_demo_rwlock_annihilate (
icl_demo_rwlock_t * ( * self_p )        //  Reference to object reference
);

#define icl_demo_rwlock_alloc()         icl_demo_rwlock_alloc_ (__FILE__, __LINE__)
static icl_demo_rwlock_t *
    icl_demo_rwlock_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_demo_rwlock_free (
icl_demo_rwlock_t * self                //  Object reference
);

Bool
    icl_demo_rwlock_animating = TRUE;   //  Animation enabled by default

/*  -------------------------------------------------------------------------
    icl_demo_rwlock_new

    Type: Component method
    Creates and initialises a new icl_demo_rwlock_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_rwlock_t *
    icl_demo_rwlock_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * filename                     //  Name of file to open
)
{
    icl_demo_rwlock_t *
        self = NULL;                    //  Object reference

    self = icl_demo_rwlock_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_DEMO_RWLOCK_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_demo_rwlock_show_);
#endif

self->handle = fopen (filename, "w+");
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_rwlock_annihilate (
    icl_demo_rwlock_t * ( * self_p )    //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    icl_demo_rwlock_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    ICL_DEMO_RWLOCK_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

fclose (self->handle);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_write

    Type: Component method
    Accepts a icl_demo_rwlock_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_rwlock_write (
    icl_demo_rwlock_t * self,           //  Reference to object
    byte * data,                        //  Pointer to data to write
    size_t size                         //  Amount of data, in octets
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

ICL_DEMO_RWLOCK_ASSERT_SANE (self);

if (fwrite (data, 1, size, self->handle) < (size_t) size)
    rc = FALSE;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_read

    Type: Component method
    Accepts a icl_demo_rwlock_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_rwlock_read (
    icl_demo_rwlock_t * self,           //  Reference to object
    byte * data,                        //  Pointer to receive data
    size_t size                         //  Maximum amount to read, in octets
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

ICL_DEMO_RWLOCK_ASSERT_SANE (self);

if (fread (data, 1, size, self->handle) == 0)
    rc = FALSE;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_reset

    Type: Component method
    Accepts a icl_demo_rwlock_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_rwlock_reset (
    icl_demo_rwlock_t * self            //  Reference to object
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

ICL_DEMO_RWLOCK_ASSERT_SANE (self);

rewind (self->handle);

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_rwlock_selftest (
void)
{
icl_demo_rwlock_t
    *output;
byte
    buffer [100];

output = icl_demo_rwlock_new ("icl_demo.tst");
icl_demo_rwlock_write (output, (byte *) "this is a string", 17);
icl_demo_rwlock_reset (output);
icl_demo_rwlock_read  (output, buffer, sizeof (buffer));
icl_demo_rwlock_destroy (&output);
icl_demo_rwlock_destroy (&output);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_rwlock_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_rwlock_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_demo_rwlock_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_demo_rwlock file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_destroy

    Type: Component method
    Destroys a icl_demo_rwlock_t object. Takes the address of a
    icl_demo_rwlock_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_rwlock_destroy_ (
    icl_demo_rwlock_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_rwlock_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_demo_rwlock_annihilate (self_p);
    icl_demo_rwlock_free ((icl_demo_rwlock_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_rwlock_t *
    icl_demo_rwlock_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_demo_rwlock_t *
        self = NULL;                    //  Object reference

self = (icl_demo_rwlock_t *) icl_mem_alloc_ (sizeof (icl_demo_rwlock_t), file, line);
if (self)
    memset (self, 0, sizeof (icl_demo_rwlock_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_free

    Type: Component method
    Freess a icl_demo_rwlock_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_rwlock_free (
    icl_demo_rwlock_t * self            //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (icl_demo_rwlock_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_rwlock_t));
        self->object_tag = ICL_DEMO_RWLOCK_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_read_lock

    Type: Component method
    Accepts a icl_demo_rwlock_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_rwlock_read_lock (
    icl_demo_rwlock_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_RWLOCK_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_write_lock

    Type: Component method
    Accepts a icl_demo_rwlock_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_rwlock_write_lock (
    icl_demo_rwlock_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_RWLOCK_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_unlock

    Type: Component method
    Accepts a icl_demo_rwlock_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_rwlock_unlock (
    icl_demo_rwlock_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_RWLOCK_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_rwlock_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_rwlock_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_rwlock_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_rwlock_new_in_scope_ (
    icl_demo_rwlock_t * * self_p,       //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * filename                     //  Name of file to open
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_rwlock_new_ (file,line,filename);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_rwlock_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_demo_rwlock_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_rwlock_component    = "icl_demo_rwlock ";
char *EMBED__icl_demo_rwlock_version      = "1.1 ";
char *EMBED__icl_demo_rwlock_copyright    = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_rwlock_filename     = "icl_demo_rwlock.icl ";
char *EMBED__icl_demo_rwlock_builddate    = "2011/03/01 ";
char *EMBED__icl_demo_rwlock_version_end  = "VeRsIoNeNd:ipc";

