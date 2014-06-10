/*---------------------------------------------------------------------------
    ipr_nonce_table.c - ipr_nonce_table component

    This class implements the hash table container for ipr_nonce
    Generated from ipr_nonce_table.icl by icl_gen using GSL/4.
    
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
#include "ipr_nonce_table.h"            //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_nonce_table_t

//  Shorthands for class methods

#define self_new            ipr_nonce_table_new
#define self_annihilate     ipr_nonce_table_annihilate
#define self_selftest       ipr_nonce_table_selftest
#define self_insert         ipr_nonce_table_insert
#define self_remove         ipr_nonce_table_remove
#define self_search         ipr_nonce_table_search
#define self_apply          ipr_nonce_table_apply
#define self_terminate      ipr_nonce_table_terminate
#define self_show           ipr_nonce_table_show
#define self_destroy        ipr_nonce_table_destroy
#define self_alloc          ipr_nonce_table_alloc
#define self_free           ipr_nonce_table_free
#define self_read_lock      ipr_nonce_table_read_lock
#define self_write_lock     ipr_nonce_table_write_lock
#define self_unlock         ipr_nonce_table_unlock
#define self_cache_initialise  ipr_nonce_table_cache_initialise
#define self_cache_purge    ipr_nonce_table_cache_purge
#define self_cache_terminate  ipr_nonce_table_cache_terminate
#define self_show_animation  ipr_nonce_table_show_animation
#define self_new_in_scope   ipr_nonce_table_new_in_scope

static void
    ipr_nonce_table_annihilate (
ipr_nonce_table_t * ( * self_p )        //  Reference to object reference
);

#define ipr_nonce_table_alloc()         ipr_nonce_table_alloc_ (__FILE__, __LINE__)
static ipr_nonce_table_t *
    ipr_nonce_table_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_nonce_table_free (
ipr_nonce_table_t * self                //  Object reference
);

static void
    ipr_nonce_table_cache_initialise (
void);

static void
    ipr_nonce_table_cache_purge (
void);

static void
    ipr_nonce_table_cache_terminate (
void);

Bool
    ipr_nonce_table_animating = TRUE;   //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


//  Hash support functions
static inline unsigned int
    s_key_hash (char * key, size_t hash_size);
static inline Bool
    s_key_equal (char * key1, char * key2);

//  Table functions to be called once locks are in place
static int
    s_ipr_nonce_table_insert (ipr_nonce_table_t *self, ipr_nonce_t *item, char * key);
static ipr_nonce_t *
    s_ipr_nonce_table_lookup (ipr_nonce_table_t *self, char * key);
static void
    s_ipr_nonce_table_remove (ipr_nonce_t *item);
static void
    s_ipr_nonce_table_purge (ipr_nonce_table_t *self);
/*  -------------------------------------------------------------------------
    ipr_nonce_table_new

    Type: Component method
    Creates and initialises a new ipr_nonce_table_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_nonce_table_t *
    ipr_nonce_table_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_nonce_table_t *
        self = NULL;                    //  Object reference

    self = ipr_nonce_table_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_NONCE_TABLE_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_nonce_table_show_);
#endif

self->max_items = IPR_NONCE_TABLE_INITIAL_SIZE;
self->table_items = icl_mem_alloc (sizeof (ipr_nonce_t*) *
    IPR_NONCE_TABLE_INITIAL_SIZE);
assert (self->table_items);
memset (self->table_items, 0, sizeof (ipr_nonce_t*) *
    IPR_NONCE_TABLE_INITIAL_SIZE);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_table_annihilate (
    ipr_nonce_table_t * ( * self_p )    //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    ipr_nonce_table_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    IPR_NONCE_TABLE_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

s_ipr_nonce_table_purge (self);
if (self->table_items)
    icl_mem_free (self->table_items);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_table_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_insert

    Type: Component method
    Accepts a ipr_nonce_table_t reference and returns zero in case of success,
    1 in case of errors.
    Insert a new item into the hash table if possible. If successful,
    the number of links to the item is automatically incremented.
    Trying to insert a duplicate key will result in nothing being inserted.
    -------------------------------------------------------------------------
 */

int
    ipr_nonce_table_insert (
    ipr_nonce_table_t * self,           //  Reference to object
    char * key,                         //  Hash key
    ipr_nonce_t * item                  //  Item to insert
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

IPR_NONCE_TABLE_ASSERT_SANE (self);

//
if (item->table_head) {
    if (item->table_head != self) {
        icl_console_print ("E: ipr_nonce item inserted into multiple table containers");
        assert (item->table_head == self);
    }
    s_ipr_nonce_table_remove (item);
}
rc = s_ipr_nonce_table_insert (self, item, key);


#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_remove

    Type: Component method
    Remove an item from a hash table.  If the item was not in any hash
    table, this function does nothing.
    -------------------------------------------------------------------------
 */

int
    ipr_nonce_table_remove (
    ipr_nonce_t * item                  //  Item to remove
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    ipr_nonce_table_t *
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
    s_ipr_nonce_table_remove (item);
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_search

    Type: Component method
    Find object by key in the hash table. Returns reference to object
    if found, else null.
    -------------------------------------------------------------------------
 */

ipr_nonce_t *
    ipr_nonce_table_search (
    ipr_nonce_table_t * self,           //  Table to search
    char * key                          //  Hash key
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    ipr_nonce_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

//
IPR_NONCE_TABLE_ASSERT_SANE (self);
if (key)
    item = s_ipr_nonce_table_lookup (self, key);

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_apply

    Type: Component method
    This method iterates through all the items in the hash and calls the
    callback function for each of them.
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_table_apply (
    ipr_nonce_table_t * self,           //  Table to iterate
    ipr_nonce_table_callback_fn * callback,   //  Not documented
    void * argument                     //  Arbitrary argument pointer
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

size_t
    pos;
ipr_nonce_t
    *item;

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

//
IPR_NONCE_TABLE_ASSERT_SANE (self);
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
    ipr_nonce_table_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_table_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_table_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_nonce_table_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_nonce_table file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_destroy

    Type: Component method
    Destroys a ipr_nonce_table_t object. Takes the address of a
    ipr_nonce_table_t reference (a pointer to a pointer) and nullifies the
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
    ipr_nonce_table_destroy_ (
    ipr_nonce_table_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_nonce_table_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_nonce_table_annihilate (self_p);
    ipr_nonce_table_free ((ipr_nonce_table_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_nonce_table_t *
    ipr_nonce_table_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_nonce_table_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_nonce_table_cache_initialise ();

self = (ipr_nonce_table_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_nonce_table_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_free

    Type: Component method
    Freess a ipr_nonce_table_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_table_free (
    ipr_nonce_table_t * self            //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (ipr_nonce_table_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_nonce_table_t));
        self->object_tag = IPR_NONCE_TABLE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_read_lock

    Type: Component method
    Accepts a ipr_nonce_table_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_nonce_table_read_lock (
    ipr_nonce_table_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_NONCE_TABLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_write_lock

    Type: Component method
    Accepts a ipr_nonce_table_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_nonce_table_write_lock (
    ipr_nonce_table_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_NONCE_TABLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_unlock

    Type: Component method
    Accepts a ipr_nonce_table_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_nonce_table_unlock (
    ipr_nonce_table_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_NONCE_TABLE_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_table_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_nonce_table_t));
icl_system_register (ipr_nonce_table_cache_purge, ipr_nonce_table_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_table_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_nonce_table_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_table_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_nonce_table_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_nonce_table_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_nonce_table_new_in_scope_ (
    ipr_nonce_table_t * * self_p,       //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_nonce_table_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_nonce_table_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
static inline unsigned int
s_key_hash (char * key, size_t hash_size)
{
    uint
        key_hash = 0;

    //  Torek hashing function
    while (*key) {
        key_hash *= 33;
        key_hash += *key;
        key++;
    }
    key_hash %= hash_size;
    return (key_hash);
}

static inline Bool
s_key_equal (char * key1, char * key2)
{
    return (streq (key1, key2));
}

static int
s_ipr_nonce_table_insert (ipr_nonce_table_t *self, ipr_nonce_t *item, char * key)
{
    qbyte
        index,
        new_index;
    ipr_nonce_t
        *current_item,
        *next_item;
    int
        rc = 0;

    if (self->nbr_items >= self->max_items * IPR_NONCE_TABLE_LOAD_FACTOR / 100) {

        //  Load factor of hash table was exceeded
        //  We need to increase its size by GROWTH_FACTOR and rebuild it
        ipr_nonce_t
            **new_items;
        size_t
            new_max_items;

        //  Create new hash table
        new_max_items = self->max_items * IPR_NONCE_TABLE_GROWTH_FACTOR / 100;
        new_items = icl_mem_alloc (sizeof (ipr_nonce_t*) * new_max_items);
        assert (new_items);
        memset (new_items, 0, sizeof (ipr_nonce_t*) * new_max_items);

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
        icl_shortstr_cpy (item->key, key);
        self->nbr_items++;
    }
    return rc;
}

static ipr_nonce_t *
s_ipr_nonce_table_lookup (ipr_nonce_table_t *self, char * key)
{
    qbyte
        index;
    ipr_nonce_t
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
s_ipr_nonce_table_remove (ipr_nonce_t *item)
{
    ipr_nonce_t
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
        icl_shortstr_cpy (item->key, "");
    }
}

static void
s_ipr_nonce_table_purge (ipr_nonce_table_t *self)
{
    uint
        table_idx;
    ipr_nonce_t
        *current_item,
        *next_item;

    for (table_idx = 0; table_idx < self->max_items; table_idx++) {
        current_item = self->table_items [table_idx];
        while (current_item) {
            next_item = current_item->table_next;
            s_ipr_nonce_table_remove (current_item);
            ipr_nonce_destroy (&current_item);
            current_item = next_item;
        }
    }
}

//  Embed the version information in the resulting binary

char *EMBED__ipr_nonce_table_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_nonce_table_component    = "ipr_nonce_table ";
char *EMBED__ipr_nonce_table_version      = "1.0 ";
char *EMBED__ipr_nonce_table_copyright    = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_nonce_table_filename     = "ipr_nonce_table.icl ";
char *EMBED__ipr_nonce_table_builddate    = "2011/03/01 ";
char *EMBED__ipr_nonce_table_version_end  = "VeRsIoNeNd:ipc";

