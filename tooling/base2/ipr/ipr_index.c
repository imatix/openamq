/*---------------------------------------------------------------------------
    ipr_index.c - ipr_index component

This class provides a mechanism for indexing objects.  An index is a number
from 0 to IPR_INDEX_MAX - 1. The class keeps indexes low by hashing across
a variable part of the index rather than across its full size.  Does not
support the retrieval of objects by their reference, only by index.
    Generated from ipr_index.icl by icl_gen using GSL/4.
    
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
#include "ipr_index.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_index_t

//  Shorthands for class methods

#define self_insert         ipr_index_insert
#define self_delete         ipr_index_delete
#define self_selftest       ipr_index_selftest
#define self_new            ipr_index_new
#define self_annihilate     ipr_index_annihilate
#define self_count          ipr_index_count
#define self_terminate      ipr_index_terminate
#define self_show           ipr_index_show
#define self_destroy        ipr_index_destroy
#define self_alloc          ipr_index_alloc
#define self_free           ipr_index_free
#define self_read_lock      ipr_index_read_lock
#define self_write_lock     ipr_index_write_lock
#define self_unlock         ipr_index_unlock
#define self_cache_initialise  ipr_index_cache_initialise
#define self_cache_purge    ipr_index_cache_purge
#define self_cache_terminate  ipr_index_cache_terminate
#define self_show_animation  ipr_index_show_animation
#define self_new_in_scope   ipr_index_new_in_scope

static void
    ipr_index_annihilate (
ipr_index_t * ( * self_p )              //  Reference to object reference
);

#define ipr_index_alloc()               ipr_index_alloc_ (__FILE__, __LINE__)
static ipr_index_t *
    ipr_index_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_index_free (
ipr_index_t * self                      //  Object reference
);

static void
    ipr_index_cache_initialise (
void);

static void
    ipr_index_cache_purge (
void);

static void
    ipr_index_cache_terminate (
void);

Bool
    ipr_index_animating = TRUE;         //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static qbyte
    s_ipr_index_count = 0;
/*  -------------------------------------------------------------------------
    ipr_index_insert

    Type: Component method
    Accepts a ipr_index_t reference and returns zero in case of success,
    1 in case of errors.
    Inserts a new object into the index.  Returns the resulting index
    value, 1 and upwards.  If the same object is inserted twice, will
    create two index entries.  A return value of zero indicates that the
    table was full.
    -------------------------------------------------------------------------
 */

int
    ipr_index_insert (
    ipr_index_t * self,                 //  Reference to object
    void * object                       //  Object to index
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

unsigned int
    hash_range,                     //  Size of hash range
    hash_index;                     //  Calculated index
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

IPR_INDEX_ASSERT_SANE (self);

assert (self->size < IPR_INDEX_MAX);
hash_range = self->size * 2;
if (hash_range > IPR_INDEX_MAX)
    hash_range = IPR_INDEX_MAX;
else
if (hash_range < 16)
    hash_range = 16;                //  Minimum hash range

hash_index = (((unsigned long) object) % (hash_range - 1)) + 1;
while (self->data [hash_index]) {
    hash_index++;
    if (hash_index == IPR_INDEX_MAX)
        hash_index = 0;
}
if (hash_index) {
    self->data [hash_index] = object;
    self->size++;
}
rc = hash_index;

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_index_delete

    Type: Component method
    Accepts a ipr_index_t reference and returns zero in case of success,
    1 in case of errors.
    Deletes the specified index entry.  Note that you must specify
    the index value, not the object address.
    -------------------------------------------------------------------------
 */

int
    ipr_index_delete (
    ipr_index_t * self,                 //  Reference to object
    int index                           //  Index entry to delete
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

IPR_INDEX_ASSERT_SANE (self);

assert (index > 0 && index < IPR_INDEX_MAX);
if (self->size > 0) {
    self->data [index] = 0;
    self->size--;
}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_index_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_index_selftest (
void)
{
ipr_index_t
    *index_table;
char
    *value1 = "abc",
    *value2 = "abc",
    *value3 = "abc";
int
    index;

index_table = ipr_index_new ();
ipr_index_insert (index_table, value1);
ipr_index_insert (index_table, value2);
ipr_index_insert (index_table, value3);
ipr_index_insert (index_table, value1);
ipr_index_insert (index_table, value2);
ipr_index_insert (index_table, value3);

//  Fill the table half-full
for (index = 0; index < IPR_INDEX_MAX / 2; index++)
    ipr_index_insert (index_table, value3);
//  Insert a limit pointer
index = ipr_index_insert (index_table, (void*) ((uint) (IPR_INDEX_MAX - 1)));
ipr_index_delete (index_table, index);

//  Clean-up
ipr_index_destroy (&index_table);
}
/*  -------------------------------------------------------------------------
    ipr_index_new

    Type: Component method
    Creates and initialises a new ipr_index_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_index_t *
    ipr_index_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_index_t *
        self = NULL;                    //  Object reference

    self = ipr_index_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_INDEX_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_index_show_);
#endif

icl_atomic_inc32 ((volatile qbyte *) &s_ipr_index_count);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_index_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_index_annihilate (
    ipr_index_t * ( * self_p )          //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    ipr_index_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    IPR_INDEX_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

icl_atomic_dec32 ((volatile qbyte *) &s_ipr_index_count);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    ipr_index_count

    Type: Component method
    -------------------------------------------------------------------------
 */

qbyte
    ipr_index_count (
void)
{
    qbyte
        count;                          //  Number of instances

count = (int) s_ipr_index_count;

    return (count);
}
/*  -------------------------------------------------------------------------
    ipr_index_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_index_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_index_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_index_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_index_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_index file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_index_destroy

    Type: Component method
    Destroys a ipr_index_t object. Takes the address of a
    ipr_index_t reference (a pointer to a pointer) and nullifies the
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
    ipr_index_destroy_ (
    ipr_index_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_index_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_index_annihilate (self_p);
    ipr_index_free ((ipr_index_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_index_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_index_t *
    ipr_index_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_index_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_index_cache_initialise ();

self = (ipr_index_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_index_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_index_free

    Type: Component method
    Freess a ipr_index_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_index_free (
    ipr_index_t * self                  //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (ipr_index_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_index_t));
        self->object_tag = IPR_INDEX_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_index_read_lock

    Type: Component method
    Accepts a ipr_index_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_index_read_lock (
    ipr_index_t * self                  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_INDEX_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_index_write_lock

    Type: Component method
    Accepts a ipr_index_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_index_write_lock (
    ipr_index_t * self                  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_INDEX_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_index_unlock

    Type: Component method
    Accepts a ipr_index_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_index_unlock (
    ipr_index_t * self                  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_INDEX_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_index_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_index_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_index_t));
icl_system_register (ipr_index_cache_purge, ipr_index_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_index_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_index_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_index_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_index_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_index_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_index_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_index_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_index_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_index_new_in_scope_ (
    ipr_index_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_index_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_index_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_index_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_index_component          = "ipr_index ";
char *EMBED__ipr_index_version            = "1.0 ";
char *EMBED__ipr_index_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_index_filename           = "ipr_index.icl ";
char *EMBED__ipr_index_builddate          = "2011/03/01 ";
char *EMBED__ipr_index_version_end        = "VeRsIoNeNd:ipc";

