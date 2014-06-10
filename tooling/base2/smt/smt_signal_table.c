/*---------------------------------------------------------------------------
    smt_signal_table.c - smt_signal_table component

    This class implements the hash table container for smt_signal
    Generated from smt_signal_table.icl by icl_gen using GSL/4.
    
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
#include "smt_signal_table.h"           //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_signal_table_t

//  Shorthands for class methods

#define self_new            smt_signal_table_new
#define self_annihilate     smt_signal_table_annihilate
#define self_selftest       smt_signal_table_selftest
#define self_insert         smt_signal_table_insert
#define self_remove         smt_signal_table_remove
#define self_search         smt_signal_table_search
#define self_apply          smt_signal_table_apply
#define self_terminate      smt_signal_table_terminate
#define self_show           smt_signal_table_show
#define self_destroy        smt_signal_table_destroy
#define self_alloc          smt_signal_table_alloc
#define self_free           smt_signal_table_free
#define self_read_lock      smt_signal_table_read_lock
#define self_write_lock     smt_signal_table_write_lock
#define self_unlock         smt_signal_table_unlock
#define self_cache_initialise  smt_signal_table_cache_initialise
#define self_cache_purge    smt_signal_table_cache_purge
#define self_cache_terminate  smt_signal_table_cache_terminate
#define self_show_animation  smt_signal_table_show_animation
#define self_new_in_scope   smt_signal_table_new_in_scope

static void
    smt_signal_table_annihilate (
smt_signal_table_t * ( * self_p )       //  Reference to object reference
);

#define smt_signal_table_alloc()        smt_signal_table_alloc_ (__FILE__, __LINE__)
static smt_signal_table_t *
    smt_signal_table_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_signal_table_free (
smt_signal_table_t * self               //  Object reference
);

static void
    smt_signal_table_cache_initialise (
void);

static void
    smt_signal_table_cache_purge (
void);

static void
    smt_signal_table_cache_terminate (
void);

Bool
    smt_signal_table_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


//  Hash support functions
static inline unsigned int
    s_key_hash (int key, size_t hash_size);
static inline Bool
    s_key_equal (int key1, int key2);

//  Table functions to be called once locks are in place
static int
    s_smt_signal_table_insert (smt_signal_table_t *self, smt_signal_t *item, int key);
static smt_signal_t *
    s_smt_signal_table_lookup (smt_signal_table_t *self, int key);
static void
    s_smt_signal_table_remove (smt_signal_t *item);
static void
    s_smt_signal_table_purge (smt_signal_table_t *self);
/*  -------------------------------------------------------------------------
    smt_signal_table_new

    Type: Component method
    Creates and initialises a new smt_signal_table_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_signal_table_t *
    smt_signal_table_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_signal_table_t *
        self = NULL;                    //  Object reference

    self = smt_signal_table_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_SIGNAL_TABLE_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_signal_table_show_);
#endif

self->max_items = SMT_SIGNAL_TABLE_INITIAL_SIZE;
self->table_items = icl_mem_alloc (sizeof (smt_signal_t*) *
    SMT_SIGNAL_TABLE_INITIAL_SIZE);
assert (self->table_items);
memset (self->table_items, 0, sizeof (smt_signal_t*) *
    SMT_SIGNAL_TABLE_INITIAL_SIZE);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_table_annihilate (
    smt_signal_table_t * ( * self_p )   //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_signal_table_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    SMT_SIGNAL_TABLE_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

s_smt_signal_table_purge (self);
if (self->table_items)
    icl_mem_free (self->table_items);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    smt_signal_table_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_signal_table_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_signal_table_insert

    Type: Component method
    Accepts a smt_signal_table_t reference and returns zero in case of success,
    1 in case of errors.
    Insert a new item into the hash table if possible. If successful,
    the number of links to the item is automatically incremented.
    Trying to insert a duplicate key will result in nothing being inserted.
    -------------------------------------------------------------------------
 */

int
    smt_signal_table_insert (
    smt_signal_table_t * self,          //  Reference to object
    int key,                            //  Hash key
    smt_signal_t * item                 //  Item to insert
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

SMT_SIGNAL_TABLE_ASSERT_SANE (self);

//
if (item->table_head) {
    if (item->table_head != self) {
        icl_console_print ("E: smt_signal item inserted into multiple table containers");
        assert (item->table_head == self);
    }
    s_smt_signal_table_remove (item);
}
rc = s_smt_signal_table_insert (self, item, key);


#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_remove

    Type: Component method
    Remove an item from a hash table.  If the item was not in any hash
    table, this function does nothing.
    -------------------------------------------------------------------------
 */

int
    smt_signal_table_remove (
    smt_signal_t * item                 //  Item to remove
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_signal_table_t *
        self = item->table_head;        //  The list
    int
        rc = 0;                         //  Not documented

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

if (self
&&  self == item->table_head) { //      Catch case where item moved
    s_smt_signal_table_remove (item);
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_search

    Type: Component method
    Find object by key in the hash table. Returns reference to object
    if found, else null.
    -------------------------------------------------------------------------
 */

smt_signal_t *
    smt_signal_table_search (
    smt_signal_table_t * self,          //  Table to search
    int key                             //  Hash key
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_signal_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

//
SMT_SIGNAL_TABLE_ASSERT_SANE (self);
if (key)
    item = s_smt_signal_table_lookup (self, key);

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_apply

    Type: Component method
    This method iterates through all the items in the hash and calls the
    callback function for each of them.
    -------------------------------------------------------------------------
 */

void
    smt_signal_table_apply (
    smt_signal_table_t * self,          //  Table to iterate
    smt_signal_table_callback_fn * callback,   //  Not documented
    void * argument                     //  Arbitrary argument pointer
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

size_t
    pos;
smt_signal_t
    *item;

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

//
SMT_SIGNAL_TABLE_ASSERT_SANE (self);
for (pos = 0; pos != self->max_items; pos ++) {
    item = self->table_items [pos];
    while (item) {
        callback (item, argument);
        item = item->table_next;
    }
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    smt_signal_table_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_signal_table_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_signal_table_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_signal_table_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
smt_signal_table_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <smt_signal_table file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    smt_signal_table_destroy

    Type: Component method
    Destroys a smt_signal_table_t object. Takes the address of a
    smt_signal_table_t reference (a pointer to a pointer) and nullifies the
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
    smt_signal_table_destroy_ (
    smt_signal_table_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_signal_table_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    smt_signal_table_annihilate (self_p);
    smt_signal_table_free ((smt_signal_table_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_signal_table_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_signal_table_t *
    smt_signal_table_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_signal_table_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_signal_table_cache_initialise ();

self = (smt_signal_table_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_signal_table_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_free

    Type: Component method
    Freess a smt_signal_table_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_signal_table_free (
    smt_signal_table_t * self           //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (smt_signal_table_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_signal_table_t));
        self->object_tag = SMT_SIGNAL_TABLE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_signal_table_read_lock

    Type: Component method
    Accepts a smt_signal_table_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_signal_table_read_lock (
    smt_signal_table_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_SIGNAL_TABLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_write_lock

    Type: Component method
    Accepts a smt_signal_table_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_signal_table_write_lock (
    smt_signal_table_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_SIGNAL_TABLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_unlock

    Type: Component method
    Accepts a smt_signal_table_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_signal_table_unlock (
    smt_signal_table_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_SIGNAL_TABLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_signal_table_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_signal_table_t));
icl_system_register (smt_signal_table_cache_purge, smt_signal_table_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_signal_table_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_table_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_signal_table_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_table_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_signal_table_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_signal_table_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_signal_table_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_signal_table_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_signal_table_new_in_scope_ (
    smt_signal_table_t * * self_p,      //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_signal_table_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_signal_table_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
static inline unsigned int
s_key_hash (int key, size_t hash_size)
{
    return ((uint) (((size_t) key) % hash_size));
}

static inline Bool
s_key_equal (int key1, int key2)
{
    return (key1 == key2);
}

static int
s_smt_signal_table_insert (smt_signal_table_t *self, smt_signal_t *item, int key)
{
    qbyte
        index,
        new_index;
    smt_signal_t
        *current_item,
        *next_item;
    int
        rc = 0;

    if (self->nbr_items >= self->max_items * SMT_SIGNAL_TABLE_LOAD_FACTOR / 100) {

        //  Load factor of hash table was exceeded
        //  We need to increase its size by GROWTH_FACTOR and rebuild it
        smt_signal_t
            **new_items;
        size_t
            new_max_items;

        //  Create new hash table
        new_max_items = self->max_items * SMT_SIGNAL_TABLE_GROWTH_FACTOR / 100;
        new_items = icl_mem_alloc (sizeof (smt_signal_t*) * new_max_items);
        assert (new_items);
        memset (new_items, 0, sizeof (smt_signal_t*) * new_max_items);

        //  Move all items to the new hash table
        for (index = 0; index != self->max_items; index++) {
            current_item = self->table_items [index];
            while (current_item) {
                next_item = current_item->table_next;
                new_index = s_key_hash (current_item->key, new_max_items);
                current_item->table_index = new_index;
                current_item->table_next = new_items [new_index];
                new_items [new_index] = current_item;
                current_item = next_item;
            }
        }

        //  Destroy old hash table
        icl_mem_free (self->table_items);
        self->table_items = new_items;
        self->max_items = new_max_items;
    }

    //  Insert new item itself
    index = s_key_hash (key, self->max_items);
    current_item = self->table_items [index];
    while (current_item) {
        if (s_key_equal (current_item->key, key)) {
            //  Attempted duplicate insertion
            rc = -1;
            break;
        }
        current_item = current_item->table_next;
    }
    if (!current_item) {
        item->table_head = self;
        item->table_index = index;
        item->table_next = self->table_items [index];
        self->table_items [index] = item;
        item->key = key;
        self->nbr_items++;
    }
    return rc;
}

static smt_signal_t *
s_smt_signal_table_lookup (smt_signal_table_t *self, int key)
{
    qbyte
        index;
    smt_signal_t
        *item;

    index = s_key_hash (key, self->max_items);
    item = self->table_items [index];

    while (item) {
        if (s_key_equal (item->key, key))
            break;
        item = item->table_next;
    }
    return item;
}

static void
s_smt_signal_table_remove (smt_signal_t *item)
{
    smt_signal_t
        *current_item,
        **prev_item;

    prev_item = &(item->table_head->table_items [item->table_index]);
    current_item = item->table_head->table_items [item->table_index];

    while (current_item) {
        if (current_item == item)
            break;
        prev_item = &(current_item->table_next);
        current_item = current_item->table_next;
    }
    assert (current_item == item);

    if (item) {
        item->table_head->nbr_items--;
        *prev_item = item->table_next;
        item->table_head = NULL;
        item->table_index = 0;
        item->table_next = NULL;
        item->key = 0;
    }
}

static void
s_smt_signal_table_purge (smt_signal_table_t *self)
{
    uint
        table_idx;
    smt_signal_t
        *current_item,
        *next_item;

    for (table_idx = 0; table_idx < self->max_items; table_idx++) {
        current_item = self->table_items [table_idx];
        while (current_item) {
            next_item = current_item->table_next;
            s_smt_signal_table_remove (current_item);
            smt_signal_destroy (&current_item);
            current_item = next_item;
        }
    }
}

//  Embed the version information in the resulting binary

char *EMBED__smt_signal_table_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_signal_table_component   = "smt_signal_table ";
char *EMBED__smt_signal_table_version     = "1.0 ";
char *EMBED__smt_signal_table_copyright   = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_signal_table_filename    = "smt_signal_table.icl ";
char *EMBED__smt_signal_table_builddate   = "2011/03/01 ";
char *EMBED__smt_signal_table_version_end  = "VeRsIoNeNd:ipc";

