/*---------------------------------------------------------------------------
    icl_demo_array_array.c - icl_demo_array_array component

    This class implements the array container for icl_demo_array
    Generated from icl_demo_array_array.icl by icl_gen using GSL/4.
    
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
#include "icl_demo_array_array.h"       //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_demo_array_array_t

//  Shorthands for class methods

#define self_new            icl_demo_array_array_new
#define self_annihilate     icl_demo_array_array_annihilate
#define self_selftest       icl_demo_array_array_selftest
#define self_reset          icl_demo_array_array_reset
#define self_insert         icl_demo_array_array_insert
#define self_remove         icl_demo_array_array_remove
#define self_fetch          icl_demo_array_array_fetch
#define self_ubound         icl_demo_array_array_ubound
#define self_terminate      icl_demo_array_array_terminate
#define self_show           icl_demo_array_array_show
#define self_destroy        icl_demo_array_array_destroy
#define self_alloc          icl_demo_array_array_alloc
#define self_free           icl_demo_array_array_free
#define self_read_lock      icl_demo_array_array_read_lock
#define self_write_lock     icl_demo_array_array_write_lock
#define self_unlock         icl_demo_array_array_unlock
#define self_link           icl_demo_array_array_link
#define self_unlink         icl_demo_array_array_unlink
#define self_cache_initialise  icl_demo_array_array_cache_initialise
#define self_cache_purge    icl_demo_array_array_cache_purge
#define self_cache_terminate  icl_demo_array_array_cache_terminate
#define self_show_animation  icl_demo_array_array_show_animation
#define self_new_in_scope   icl_demo_array_array_new_in_scope

#define icl_demo_array_array_annihilate(self)  icl_demo_array_array_annihilate_ (self, __FILE__, __LINE__)
static void
    icl_demo_array_array_annihilate_ (
icl_demo_array_array_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_demo_array_array_alloc()    icl_demo_array_array_alloc_ (__FILE__, __LINE__)
static icl_demo_array_array_t *
    icl_demo_array_array_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_demo_array_array_free(self)  icl_demo_array_array_free_ (self, __FILE__, __LINE__)
static void
    icl_demo_array_array_free_ (
icl_demo_array_array_t * self,          //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    icl_demo_array_array_cache_initialise (
void);

static void
    icl_demo_array_array_cache_purge (
void);

static void
    icl_demo_array_array_cache_terminate (
void);

Bool
    icl_demo_array_array_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static void
    s_icl_demo_array_array_reset  (icl_demo_array_array_t *self);
static void
    s_icl_demo_array_array_insert (icl_demo_array_array_t *self, icl_demo_array_t *item, qbyte index);
static void
    s_icl_demo_array_array_remove (icl_demo_array_t *item);
//  We increase the array in blocks of this many entries
#define ARRAY_EXTENT        1024
//  This is the largest array we allow (for sanity)
#define ARRAY_MAX_INDEX     1024*1024*128
/*  -------------------------------------------------------------------------
    icl_demo_array_array_new

    Type: Component method
    Creates and initialises a new icl_demo_array_array_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_demo_array_array_t *
    icl_demo_array_array_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_demo_array_array_t *
        self = NULL;                    //  Object reference

    self = icl_demo_array_array_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_DEMO_ARRAY_ARRAY_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, icl_demo_array_array_show_);
#endif

}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_annihilate

    Type: Component method
    Destroys the array and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_array_array_annihilate_ (
    icl_demo_array_array_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
    int
        history_last;
#endif

    icl_demo_array_array_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = file;
    self->history_line  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = line;
    self->history_type  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = self->links;
#endif

    ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

s_icl_demo_array_array_reset (self);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_array_array_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_reset

    Type: Component method
    Accepts a icl_demo_array_array_t reference and returns zero in case of success,
    1 in case of errors.
    Clears the array and frees all allocated memory.
    -------------------------------------------------------------------------
 */

int
    icl_demo_array_array_reset (
    icl_demo_array_array_t * self       //  Reference to object
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

ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
if (!self->zombie) {

s_icl_demo_array_array_reset (self);
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_insert

    Type: Component method
    Accepts a icl_demo_array_array_t reference and returns zero in case of success,
    1 in case of errors.
    Insert a new item into the array or replace the existing item at
    same index.  The first valid index is zero.

    The number of links is automatically adjusted - if the item was previously
    in an array, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    icl_demo_array_array_insert (
    icl_demo_array_array_t * self,      //  Reference to object
    qbyte index,                        //  Array index
    icl_demo_array_t * item             //  Item to insert
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

ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
if (!self->zombie) {

//
if (item->array_head) {
    if (item->array_head != self) {
        icl_console_print ("E: icl_demo_array item inserted into multiple array containers");
        assert (item->array_head == self);
    }
    s_icl_demo_array_array_remove (item);
    icl_demo_array_unlink (&item);
}
s_icl_demo_array_array_insert (self, item, index);
if (item->array_head)
    icl_demo_array_link (item);

}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_remove

    Type: Component method
    Remove a specified item from an array.  If the item is not in any array,
    this function does nothing.

    If the item was in an array, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    icl_demo_array_array_remove (
    icl_demo_array_t * item             //  Item to remove
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    icl_demo_array_array_t *
        self = item->array_head;        //  The list
    int
        rc = 0;                         //  Not documented

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

if (self
&&  self == item->array_head) { //      Catch case where item moved
    s_icl_demo_array_array_remove (item);
    icl_demo_array_unlink (&item);
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_fetch

    Type: Component method
    Fetch the item at specified index, if any.  If no item is defined at
    the specified index, returns NULL.

    If an item is found, its link count is incremented.
    -------------------------------------------------------------------------
 */

icl_demo_array_t *
    icl_demo_array_array_fetch (
    icl_demo_array_array_t * self,      //  The array
    qbyte index                         //  Array index
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    icl_demo_array_t *
        item = NULL;                    //  Item fetched

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

//
ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
if (index < self->bound) {
    item = self->data [index];
    icl_demo_array_link (item);
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_ubound

    Type: Component method
    Returns the upper bound of the array, that is one more than the index
    of the last item in the array.
    -------------------------------------------------------------------------
 */

qbyte
    icl_demo_array_array_ubound (
    icl_demo_array_array_t * self       //  The array
)
{
    qbyte
        index;                          //  The result

//
ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
index = icl_atomic_get32 (&self->bound);

    return (index);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_array_array_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_array_array_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    icl_demo_array_array_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <icl_demo_array_array zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
    if (self->history_last > ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH;
        self->history_last %= ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </icl_demo_array_array>\n");
#endif

}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_destroy

    Type: Component method
    Destroys a icl_demo_array_array_t object. Takes the address of a
    icl_demo_array_array_t reference (a pointer to a pointer) and nullifies the
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
    icl_demo_array_array_destroy_ (
    icl_demo_array_array_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_demo_array_array_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        icl_demo_array_array_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("icl_demo_array_array", "E: missing link on icl_demo_array_array object");
        icl_demo_array_array_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        icl_demo_array_array_free_ ((icl_demo_array_array_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_demo_array_array_t *
    icl_demo_array_array_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_demo_array_array_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    icl_demo_array_array_cache_initialise ();

self = (icl_demo_array_array_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (icl_demo_array_array_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_free

    Type: Component method
    Freess a icl_demo_array_array_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_array_array_free_ (
    icl_demo_array_array_t * self,      //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = "free";
        self->history_links [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = self->links;
#endif

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (icl_demo_array_array_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_demo_array_array_t));
        self->object_tag = ICL_DEMO_ARRAY_ARRAY_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_read_lock

    Type: Component method
    Accepts a icl_demo_array_array_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_array_array_read_lock (
    icl_demo_array_array_t * self       //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_write_lock

    Type: Component method
    Accepts a icl_demo_array_array_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_array_array_write_lock (
    icl_demo_array_array_t * self       //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_unlock

    Type: Component method
    Accepts a icl_demo_array_array_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_demo_array_array_unlock (
    icl_demo_array_array_t * self       //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif

}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

icl_demo_array_array_t *
    icl_demo_array_array_link_ (
    icl_demo_array_array_t * self,      //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
    int
        history_last;
#endif

    if (self) {
        ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = "link";
        self->history_links [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    icl_demo_array_array_unlink_ (
    icl_demo_array_array_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
    int
        history_last;
#endif

    icl_demo_array_array_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        ICL_DEMO_ARRAY_ARRAY_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("icl_demo_array_array", "E: missing link on icl_demo_array_array object");
            icl_demo_array_array_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ICL_DEMO_ARRAY_ARRAY)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % ICL_DEMO_ARRAY_ARRAY_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            icl_demo_array_array_annihilate_ (self_p, file, line);
        icl_demo_array_array_free_ ((icl_demo_array_array_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    icl_demo_array_array_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (icl_demo_array_array_t));
icl_system_register (icl_demo_array_array_cache_purge, icl_demo_array_array_cache_terminate);
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_array_array_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_demo_array_array_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_demo_array_array_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_demo_array_array_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_demo_array_array_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_demo_array_array_new_in_scope_ (
    icl_demo_array_array_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_demo_array_array_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_demo_array_array_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
static void
s_icl_demo_array_array_reset (icl_demo_array_array_t *self)
{
    qbyte
        index;
    icl_demo_array_t
        *item;

    if (self->data) {
        for (index = 0; index < self->bound; index++) {
            item = self->data [index];
            if (item) {
                item->array_head = NULL;
                item->array_index = 0;
                icl_demo_array_unlink (&item);
            }
        }
        icl_mem_free (self->data);
        self->data = NULL;
    }
    self->bound = 0;
    self->size  = 0;
}

static void
s_icl_demo_array_array_insert (icl_demo_array_array_t *self, icl_demo_array_t *item, qbyte index)
{
    size_t
        old_size,
        extent;
    icl_demo_array_t
        *old_item;

    assert (item);
    assert (index < ARRAY_MAX_INDEX);

    if (index >= self->size) {
        old_size = sizeof (icl_demo_array_t *) * self->size;
        extent   = sizeof (icl_demo_array_t *) * ARRAY_EXTENT;
        self->size += ARRAY_EXTENT;
        self->data = icl_mem_realloc (self->data, old_size + extent);
        memset ((byte *) self->data + old_size, 0, extent);
    }
    //  Kick-out any previous occupant of this slot
    old_item = self->data [index];
    if (old_item) {
        old_item->array_head = NULL;
        old_item->array_index = 0;
        icl_demo_array_destroy (&old_item);
    }
    self->data [index] = item;
    if (self->bound <= index)
        self->bound = index + 1;

    item->array_head = self;
    item->array_index = index;
}

static void
s_icl_demo_array_array_remove (icl_demo_array_t *item)
{
    icl_demo_array_array_t
        *self = item->array_head;
    qbyte
        index = item->array_index;

    if (self) {
        assert (index < self->size);
        assert (self-> data [index] == item);

        if (self->data) {
            item->array_head = NULL;
            item->array_index = 0;
            self->data [index] = NULL;
            while (self->bound > 0 && self->data [self->bound - 1] == NULL)
                self->bound--;
        }
    }
}

//  Embed the version information in the resulting binary

char *EMBED__icl_demo_array_array_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_demo_array_array_component  = "icl_demo_array_array ";
char *EMBED__icl_demo_array_array_version  = "1.0 ";
char *EMBED__icl_demo_array_array_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_demo_array_array_filename  = "icl_demo_array_array.icl ";
char *EMBED__icl_demo_array_array_builddate  = "2011/03/01 ";
char *EMBED__icl_demo_array_array_version_end  = "VeRsIoNeNd:ipc";

