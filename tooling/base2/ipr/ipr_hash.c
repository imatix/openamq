/*---------------------------------------------------------------------------
    ipr_hash.c - ipr_hash component

Implements a loose-reference hash with no locking, for use in lock-free agents.
The hash is indexed by a string key, and holds a void pointer reference.  Hash
items are reference counted, so use unlink and destroy appropriately.
    Generated from ipr_hash.icl by icl_gen using GSL/4.
    
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
#include "ipr_hash.h"                   //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_hash_t

//  Shorthands for class methods

#define self_new            ipr_hash_new
#define self_insert         ipr_hash_insert
#define self_lookup         ipr_hash_lookup
#define self_delete         ipr_hash_delete
#define self_selftest       ipr_hash_selftest
#define self_remove_from_all_containers  ipr_hash_remove_from_all_containers
#define self_show           ipr_hash_show
#define self_terminate      ipr_hash_terminate
#define self_destroy        ipr_hash_destroy
#define self_annihilate     ipr_hash_annihilate
#define self_alloc          ipr_hash_alloc
#define self_free           ipr_hash_free
#define self_link           ipr_hash_link
#define self_unlink         ipr_hash_unlink
#define self_cache_initialise  ipr_hash_cache_initialise
#define self_cache_purge    ipr_hash_cache_purge
#define self_cache_terminate  ipr_hash_cache_terminate
#define self_show_animation  ipr_hash_show_animation
#define self_new_in_scope   ipr_hash_new_in_scope

#define ipr_hash_annihilate(self)       ipr_hash_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_hash_annihilate_ (
ipr_hash_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_hash_alloc()                ipr_hash_alloc_ (__FILE__, __LINE__)
static ipr_hash_t *
    ipr_hash_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_hash_free(self)             ipr_hash_free_ (self, __FILE__, __LINE__)
static void
    ipr_hash_free_ (
ipr_hash_t * self,                      //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    ipr_hash_cache_initialise (
void);

static void
    ipr_hash_cache_purge (
void);

static void
    ipr_hash_cache_terminate (
void);

Bool
    ipr_hash_animating = TRUE;          //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_hash_new

    Type: Component method
    Creates and initialises a new ipr_hash_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

ipr_hash_t *
    ipr_hash_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_hash_table_t * table,           //  Table to insert into
    char * key,                         //  Hash key
    void * data                         //  Not documented
)
{
    ipr_hash_t *
        self = NULL;                    //  Object reference

    self = ipr_hash_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_HASH_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_hash_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
//
self->data = data;
if (table && self && ipr_hash_table_insert (table, key, self))
    ipr_hash_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_hash_insert

    Type: Component method
    Creates a new hash for the specified data value, does not return
    a hash item.  Asserts that the specified key does not already
    exist.
    -------------------------------------------------------------------------
 */

void
    ipr_hash_insert (
    ipr_hash_table_t * table,           //  Not documented
    char * key,                         //  Not documented
    void * data                         //  Not documented
)
{
ipr_hash_t
    *hash;

//
assert (!ipr_hash_table_search (table, key));
hash = ipr_hash_new (table, key, data);
ipr_hash_unlink (&hash);
}
/*  -------------------------------------------------------------------------
    ipr_hash_lookup

    Type: Component method
    Searches a hash table for a key value and returns the data reference if
    found, else null.
    -------------------------------------------------------------------------
 */

void *
    ipr_hash_lookup (
    ipr_hash_table_t * table,           //  Not documented
    char * key                          //  Not documented
)
{
ipr_hash_t
    *hash;
    void *
        data = NULL;                    //  Not documented

hash = ipr_hash_table_search (table, key);
if (hash) {
    data = hash->data;
    ipr_hash_unlink (&hash);
}

    return (data);
}
/*  -------------------------------------------------------------------------
    ipr_hash_delete

    Type: Component method
    Deletes the hash for the specified data value, does not return
    a hash item.  Asserts that the specified key existed.
    -------------------------------------------------------------------------
 */

void
    ipr_hash_delete (
    ipr_hash_table_t * table,           //  Not documented
    char * key                          //  Not documented
)
{
ipr_hash_t
    *hash;

//
hash = ipr_hash_table_search (table, key);
assert (hash);
ipr_hash_destroy (&hash);
}
/*  -------------------------------------------------------------------------
    ipr_hash_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_hash_selftest (
void)
{

ipr_hash_table_t
    *table;
ipr_hash_t
    *hash;
char
    *some_data = "hello";

table = ipr_hash_table_new ();
hash = ipr_hash_new (table, "tmp_000000", some_data);
ipr_hash_unlink (&hash);

assert (ipr_hash_lookup (table, "tmp_000000") == some_data);
hash = ipr_hash_table_search (table, "tmp_000000");
assert (hash);
ipr_hash_destroy (&hash);
ipr_hash_table_destroy (&table);
}
/*  -------------------------------------------------------------------------
    ipr_hash_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_hash_remove_from_all_containers (
    ipr_hash_t * self                   //  The item
)
{

IPR_HASH_ASSERT_SANE (self);
ipr_hash_table_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_hash_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_hash_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_hash_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_hash zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    if (self->history_last > IPR_HASH_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_HASH_HISTORY_LENGTH;
        self->history_last %= IPR_HASH_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_HASH_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_hash>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_hash_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_hash_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_hash_destroy

    Type: Component method
    Destroys a ipr_hash_t object. Takes the address of a
    ipr_hash_t reference (a pointer to a pointer) and nullifies the
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
    ipr_hash_destroy_ (
    ipr_hash_t * ( * self_p ),          //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_hash_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_hash_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_hash", "E: missing link on ipr_hash object");
        ipr_hash_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_hash_free_ ((ipr_hash_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_hash_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_hash_annihilate_ (
    ipr_hash_t * ( * self_p ),          //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    int
        history_last;
#endif

    ipr_hash_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_HASH_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_HASH_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_HASH_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_HASH_HISTORY_LENGTH] = self->links;
#endif

    IPR_HASH_ASSERT_SANE (self);
    ipr_hash_remove_from_all_containers (self);


}
/*  -------------------------------------------------------------------------
    ipr_hash_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_hash_t *
    ipr_hash_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_hash_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_hash_cache_initialise ();

self = (ipr_hash_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_hash_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_hash_free

    Type: Component method
    Freess a ipr_hash_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_hash_free_ (
    ipr_hash_t * self,                  //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_HASH_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_HASH_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_HASH_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_HASH_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (ipr_hash_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_hash_t));
        self->object_tag = IPR_HASH_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_hash_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_hash_t *
    ipr_hash_link_ (
    ipr_hash_t * self,                  //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    int
        history_last;
#endif

    if (self) {
        IPR_HASH_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_HASH_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_HASH_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_HASH_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_HASH_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_hash_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_hash_unlink_ (
    ipr_hash_t * ( * self_p ),          //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    int
        history_last;
#endif

    ipr_hash_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_HASH_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_hash", "E: missing link on ipr_hash object");
            ipr_hash_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_HASH_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_HASH_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_HASH_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_HASH_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_hash_annihilate_ (self_p, file, line);
        ipr_hash_free_ ((ipr_hash_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_hash_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_hash_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_hash_t));
icl_system_register (ipr_hash_cache_purge, ipr_hash_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_hash_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_hash_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_hash_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_hash_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_hash_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_hash_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_hash_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_hash_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_hash_new_in_scope_ (
    ipr_hash_t * * self_p,              //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_hash_table_t * table,           //  Table to insert into
    char * key,                         //  Hash key
    void * data                         //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_hash_new_ (file,line,table,key,data);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_hash_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_hash_version_start       = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_hash_component           = "ipr_hash ";
char *EMBED__ipr_hash_version             = "1.0 ";
char *EMBED__ipr_hash_copyright           = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_hash_filename            = "ipr_hash.icl ";
char *EMBED__ipr_hash_builddate           = "2011/03/01 ";
char *EMBED__ipr_hash_version_end         = "VeRsIoNeNd:ipc";

