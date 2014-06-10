/*---------------------------------------------------------------------------
    ipr_symbol.c - ipr_symbol component

Implements a symbol table that holds arbitrary data, keyed by name.
Can be used directly, or inherited to create specific symbol table
structures.
    Generated from ipr_symbol.icl by icl_gen using GSL/4.
    
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
#include "ipr_symbol.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_symbol_t

//  Shorthands for class methods

#define self_new            ipr_symbol_new
#define self_annihilate     ipr_symbol_annihilate
#define self_assume         ipr_symbol_assume
#define self_selftest       ipr_symbol_selftest
#define self_remove_from_all_containers  ipr_symbol_remove_from_all_containers
#define self_show           ipr_symbol_show
#define self_terminate      ipr_symbol_terminate
#define self_destroy        ipr_symbol_destroy
#define self_alloc          ipr_symbol_alloc
#define self_free           ipr_symbol_free
#define self_read_lock      ipr_symbol_read_lock
#define self_write_lock     ipr_symbol_write_lock
#define self_unlock         ipr_symbol_unlock
#define self_link           ipr_symbol_link
#define self_unlink         ipr_symbol_unlink
#define self_cache_initialise  ipr_symbol_cache_initialise
#define self_cache_purge    ipr_symbol_cache_purge
#define self_cache_terminate  ipr_symbol_cache_terminate
#define self_show_animation  ipr_symbol_show_animation
#define self_new_in_scope   ipr_symbol_new_in_scope

#define ipr_symbol_annihilate(self)     ipr_symbol_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_symbol_annihilate_ (
ipr_symbol_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_symbol_alloc()              ipr_symbol_alloc_ (__FILE__, __LINE__)
static ipr_symbol_t *
    ipr_symbol_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_symbol_free(self)           ipr_symbol_free_ (self, __FILE__, __LINE__)
static void
    ipr_symbol_free_ (
ipr_symbol_t * self,                    //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    ipr_symbol_cache_initialise (
void);

static void
    ipr_symbol_cache_purge (
void);

static void
    ipr_symbol_cache_terminate (
void);

Bool
    ipr_symbol_animating = TRUE;        //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_symbol_new

    Type: Component method
    Creates and initialises a new ipr_symbol_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

ipr_symbol_t *
    ipr_symbol_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_symbol_table_t * table,         //  Table to insert into
    char * key,                         //  Hash key
    void * data                         //  Data for symbol
)
{
    ipr_symbol_t *
        self = NULL;                    //  Object reference

    self = ipr_symbol_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_SYMBOL_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
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
        icl_mem_set_callback (self, ipr_symbol_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->data = data;
if (table && self && ipr_symbol_table_insert (table, key, self))
    ipr_symbol_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_symbol_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_symbol_annihilate_ (
    ipr_symbol_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    int
        history_last;
#endif

    ipr_symbol_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_SYMBOL_HISTORY_LENGTH] = self->links;
#endif

    IPR_SYMBOL_ASSERT_SANE (self);
    ipr_symbol_remove_from_all_containers (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    ipr_symbol_assume

    Type: Component method
    Create or update symbol with specified value.
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_assume (
    ipr_symbol_table_t * table,         //  Symbol table
    char * name,                        //  Symbol name
    void * data                         //  Data for symbol
)
{
ipr_symbol_t
    *item;                          //  Inserted item

item = ipr_symbol_table_search (table, name);
if (item)
    item->data = data;
else
    item = ipr_symbol_new (table, name, data);

ipr_symbol_unlink (&item);
}
/*  -------------------------------------------------------------------------
    ipr_symbol_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_selftest (
void)
{
ipr_symbol_table_t *
    table;
ipr_symbol_t *
    item;

table = ipr_symbol_table_new ();

//  Insert some items                                                      
item = ipr_symbol_new (table, "0xdeadbeef", "deadbeef");
ipr_symbol_unlink (&item);
item = ipr_symbol_new (table, "0xabadcafe", "abadcafe");
ipr_symbol_unlink (&item);

//  Retrieve items                                                         
item = ipr_symbol_table_search (table, "0xdeadbeef");
assert (item && streq (item->data, "deadbeef"));
ipr_symbol_unlink (&item);
item = ipr_symbol_table_search (table, "0xabadcafe");
assert (item && streq (item->data, "abadcafe"));
ipr_symbol_unlink (&item);

//  Get a non existent item                                                
item = ipr_symbol_table_search (table, "0xf0000000");
assert (item == NULL);

//  Delete an item                                                         
item = ipr_symbol_table_search (table, "0xdeadbeef");
ipr_symbol_destroy (&item);
item = ipr_symbol_table_search (table, "0xdeadbeef");
assert (item == NULL);

ipr_symbol_table_destroy (&table);
}
/*  -------------------------------------------------------------------------
    ipr_symbol_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_remove_from_all_containers (
    ipr_symbol_t * self                 //  The item
)
{

IPR_SYMBOL_ASSERT_SANE (self);
ipr_symbol_table_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_symbol_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_symbol_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_symbol zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    if (self->history_last > IPR_SYMBOL_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_SYMBOL_HISTORY_LENGTH;
        self->history_last %= IPR_SYMBOL_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_SYMBOL_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_symbol>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_symbol_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_symbol_destroy

    Type: Component method
    Destroys a ipr_symbol_t object. Takes the address of a
    ipr_symbol_t reference (a pointer to a pointer) and nullifies the
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
    ipr_symbol_destroy_ (
    ipr_symbol_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_symbol_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_symbol_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_symbol", "E: missing link on ipr_symbol object");
        ipr_symbol_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_symbol_free_ ((ipr_symbol_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_symbol_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_symbol_t *
    ipr_symbol_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_symbol_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_symbol_cache_initialise ();

self = (ipr_symbol_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_symbol_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_symbol_free

    Type: Component method
    Freess a ipr_symbol_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_symbol_free_ (
    ipr_symbol_t * self,                //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_SYMBOL_HISTORY_LENGTH] = self->links;
#endif

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (ipr_symbol_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_symbol_t));
        self->object_tag = IPR_SYMBOL_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_symbol_read_lock

    Type: Component method
    Accepts a ipr_symbol_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_symbol_read_lock (
    ipr_symbol_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_SYMBOL_ASSERT_SANE (self);
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
    ipr_symbol_write_lock

    Type: Component method
    Accepts a ipr_symbol_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_symbol_write_lock (
    ipr_symbol_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_SYMBOL_ASSERT_SANE (self);
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
    ipr_symbol_unlock

    Type: Component method
    Accepts a ipr_symbol_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_symbol_unlock (
    ipr_symbol_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_SYMBOL_ASSERT_SANE (self);
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
    ipr_symbol_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_symbol_t *
    ipr_symbol_link_ (
    ipr_symbol_t * self,                //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    int
        history_last;
#endif

    if (self) {
        IPR_SYMBOL_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_SYMBOL_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_symbol_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_unlink_ (
    ipr_symbol_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    int
        history_last;
#endif

    ipr_symbol_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_SYMBOL_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_symbol", "E: missing link on ipr_symbol object");
            ipr_symbol_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_SYMBOL_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_SYMBOL_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_symbol_annihilate_ (self_p, file, line);
        ipr_symbol_free_ ((ipr_symbol_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_symbol_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_symbol_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_symbol_t));
icl_system_register (ipr_symbol_cache_purge, ipr_symbol_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_symbol_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_symbol_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_symbol_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_symbol_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_symbol_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_symbol_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_symbol_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_symbol_new_in_scope_ (
    ipr_symbol_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_symbol_table_t * table,         //  Table to insert into
    char * key,                         //  Hash key
    void * data                         //  Data for symbol
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_symbol_new_ (file,line,table,key,data);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_symbol_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_symbol_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_symbol_component         = "ipr_symbol ";
char *EMBED__ipr_symbol_version           = "1.0 ";
char *EMBED__ipr_symbol_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_symbol_filename          = "ipr_symbol.icl ";
char *EMBED__ipr_symbol_builddate         = "2011/03/01 ";
char *EMBED__ipr_symbol_version_end       = "VeRsIoNeNd:ipc";

