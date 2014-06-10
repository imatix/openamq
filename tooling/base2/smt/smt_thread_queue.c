/*---------------------------------------------------------------------------
    smt_thread_queue.c - smt_thread_queue component

    This class implements the list container for smt_thread
    Generated from smt_thread_queue.icl by icl_gen using GSL/4.
    
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
#include "smt_thread_queue.h"           //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_thread_queue_t

//  Shorthands for class methods

#define self_new            smt_thread_queue_new
#define self_annihilate     smt_thread_queue_annihilate
#define self_selftest       smt_thread_queue_selftest
#define self_isempty        smt_thread_queue_isempty
#define self_push           smt_thread_queue_push
#define self_pop            smt_thread_queue_pop
#define self_queue          smt_thread_queue_queue
#define self_relink_before  smt_thread_queue_relink_before
#define self_remove         smt_thread_queue_remove
#define self_first          smt_thread_queue_first
#define self_last           smt_thread_queue_last
#define self_next           smt_thread_queue_next
#define self_prev           smt_thread_queue_prev
#define self_rehook         smt_thread_queue_rehook
#define self_unhook         smt_thread_queue_unhook
#define self_terminate      smt_thread_queue_terminate
#define self_show           smt_thread_queue_show
#define self_destroy        smt_thread_queue_destroy
#define self_alloc          smt_thread_queue_alloc
#define self_free           smt_thread_queue_free
#define self_read_lock      smt_thread_queue_read_lock
#define self_write_lock     smt_thread_queue_write_lock
#define self_unlock         smt_thread_queue_unlock
#define self_link           smt_thread_queue_link
#define self_unlink         smt_thread_queue_unlink
#define self_cache_initialise  smt_thread_queue_cache_initialise
#define self_cache_purge    smt_thread_queue_cache_purge
#define self_cache_terminate  smt_thread_queue_cache_terminate
#define self_show_animation  smt_thread_queue_show_animation
#define self_new_in_scope   smt_thread_queue_new_in_scope

#define smt_thread_queue_annihilate(self)  smt_thread_queue_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_thread_queue_annihilate_ (
smt_thread_queue_t * ( * self_p ),      //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static inline void
    smt_thread_queue_rehook (
smt_thread_t * item,                    //  Not documented
smt_thread_t * left,                    //  Not documented
smt_thread_t * right                    //  Not documented
);

static inline smt_thread_t *
    smt_thread_queue_unhook (
smt_thread_t * item                     //  Not documented
);

#define smt_thread_queue_alloc()        smt_thread_queue_alloc_ (__FILE__, __LINE__)
static smt_thread_queue_t *
    smt_thread_queue_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_thread_queue_free(self)     smt_thread_queue_free_ (self, __FILE__, __LINE__)
static void
    smt_thread_queue_free_ (
smt_thread_queue_t * self,              //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_thread_queue_cache_initialise (
void);

static void
    smt_thread_queue_cache_purge (
void);

static void
    smt_thread_queue_cache_terminate (
void);

Bool
    smt_thread_queue_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_thread_queue_new

    Type: Component method
    Creates and initialises a new smt_thread_queue_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_thread_queue_t *
    smt_thread_queue_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_thread_queue_t *
        self = NULL;                    //  Object reference

    self = smt_thread_queue_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_THREAD_QUEUE_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
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
        icl_mem_set_callback (self, smt_thread_queue_show_);
#endif

self->smt_thread.queue_next = &self->smt_thread;
self->smt_thread.queue_prev = &self->smt_thread;
self->smt_thread.queue_head = self;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    smt_thread_queue_annihilate_ (
    smt_thread_queue_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
    int
        history_last;
#endif

smt_thread_t
    *item;
    smt_thread_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = self->links;
#endif

    SMT_THREAD_QUEUE_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

item = (smt_thread_t *) self->smt_thread.queue_next;
while (item != &self->smt_thread) {
    //  Unlink child without lock (we already have the lock)
    smt_thread_queue_unhook (item);

    smt_thread_destroy (&item);
    item = (smt_thread_t *) self->smt_thread.queue_next;
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    smt_thread_queue_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_queue_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_thread_queue_isempty

    Type: Component method
    Accepts a smt_thread_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_isempty (
    smt_thread_queue_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_THREAD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

rc = (self->smt_thread.queue_next == &self->smt_thread);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_push

    Type: Component method
    Accepts a smt_thread_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_push (
    smt_thread_queue_t * self,          //  Reference to object
    smt_thread_t * item                 //  Not documented
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

SMT_THREAD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

//  If queue_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->queue_head) {
}
else
if (item->queue_head != self) {
    icl_console_print ("E: smt_thread item inserted into multiple queue containers");
    assert (item->queue_head == self);
}
smt_thread_queue_unhook (item);
smt_thread_queue_rehook (item, &self->smt_thread, (smt_thread_t *) self->smt_thread.queue_next);
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
    smt_thread_queue_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_queue_pop (
    smt_thread_queue_t * self           //  The list
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_thread_t *
        item = NULL;                    //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

SMT_THREAD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

if (self->smt_thread.queue_next == &self->smt_thread)
    item = NULL;
else {
    item = smt_thread_queue_unhook ((smt_thread_t *) self->smt_thread.queue_next);
}
}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_queue

    Type: Component method
    Accepts a smt_thread_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_queue (
    smt_thread_queue_t * self,          //  Reference to object
    smt_thread_t * item                 //  Not documented
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

SMT_THREAD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

//  If queue_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->queue_head) {
}
else
if (item->queue_head != self) {
    icl_console_print ("E: smt_thread item inserted into multiple queue containers");
    assert (item->queue_head == self);
}
smt_thread_queue_unhook (item);
smt_thread_queue_rehook (item, (smt_thread_t *) self->smt_thread.queue_prev, &self->smt_thread);
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
    smt_thread_queue_relink_before

    Type: Component method
    Accepts a smt_thread_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_relink_before (
    smt_thread_t * item,                //  Not documented
    smt_thread_t * where                //  Not documented
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code
    smt_thread_queue_t *
        self = (smt_thread_queue_t *) where->queue_head;  //  The list


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

SMT_THREAD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

//  If queue_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->queue_head) {
}
else
if (item->queue_head != self) {
    icl_console_print ("E: smt_thread item inserted into multiple queue containers");
    assert (item->queue_head == self);
}
smt_thread_queue_unhook (item);
smt_thread_queue_rehook (item, (smt_thread_t *) where->queue_prev, where);
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
    smt_thread_queue_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_remove (
    smt_thread_t * item                 //  Not documented
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_thread_queue_t *
        self = (smt_thread_queue_t *) item->queue_head;  //  The list
    int
        rc = 0;                         //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

if (self
&&  self == item->queue_head) { //      Catch case where item moved
    smt_thread_queue_unhook (item);
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_first

    Type: Component method
    Returns the first smt_thread item on the list.    If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_queue_first_ (
    smt_thread_queue_t * self,          //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_thread_t *
        item = NULL;                    //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

SMT_THREAD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_thread_t *) self->smt_thread.queue_next;
if (item == &self->smt_thread)
    item = NULL;

}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_last

    Type: Component method
    Returns the last smt_thread item on the list.    If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_queue_last_ (
    smt_thread_queue_t * self,          //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_thread_t *
        item = NULL;                    //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

SMT_THREAD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_thread_t *) self->smt_thread.queue_prev;
if (item == &self->smt_thread)
    item = NULL;

}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_next

    Type: Component method
    Returns the following item on the smt_thread list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_queue_next_ (
    smt_thread_t * ( * item_p ),        //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_thread_t *
        item = *item_p;                 //  Dereferenced item
    smt_thread_t *
        next = NULL;                    //  The item to return
    smt_thread_queue_t *
        self = (smt_thread_queue_t *) item->queue_head;  //  The list


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

if (self
&&  self == item->queue_head) { //      Catch case where item moved
    next = (smt_thread_t *) item->queue_next;
    if (next == &self->smt_thread)
        next = NULL;

}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (next);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_prev

    Type: Component method
    Returns the preceding item on the smt_thread list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_queue_prev_ (
    smt_thread_t * ( * item_p ),        //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_thread_t *
        item = *item_p;                 //  Dereferenced item
    smt_thread_t *
        prev = NULL;                    //  The item to return
    smt_thread_queue_t *
        self = (smt_thread_queue_t *) item->queue_head;  //  The list


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

if (self
&&  self == item->queue_head) { //      Catch case where item moved
    prev = (smt_thread_t *) item->queue_prev;
    if (prev == &self->smt_thread)
        prev = NULL;

}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (prev);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    smt_thread_queue_rehook (
    smt_thread_t * item,                //  Not documented
    smt_thread_t * left,                //  Not documented
    smt_thread_t * right                //  Not documented
)
{
smt_thread_t
    *swap;

swap = (smt_thread_t *) left->queue_next;
left->queue_next = item->queue_next;
item->queue_next = swap;

swap = (smt_thread_t *) item->queue_prev;
item->queue_prev = right->queue_prev;
right->queue_prev = swap;

item->queue_head = left->queue_head;
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline smt_thread_t *
    smt_thread_queue_unhook (
    smt_thread_t * item                 //  Not documented
)
{

smt_thread_queue_rehook (item, (smt_thread_t *) item->queue_prev, (smt_thread_t *) item->queue_next);
item->queue_head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_queue_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_thread_queue_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_queue_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_thread_queue_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_thread_queue zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
    if (self->history_last > SMT_THREAD_QUEUE_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_THREAD_QUEUE_HISTORY_LENGTH;
        self->history_last %= SMT_THREAD_QUEUE_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_THREAD_QUEUE_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_thread_queue>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_thread_queue_destroy

    Type: Component method
    Destroys a smt_thread_queue_t object. Takes the address of a
    smt_thread_queue_t reference (a pointer to a pointer) and nullifies the
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
    smt_thread_queue_destroy_ (
    smt_thread_queue_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_thread_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_thread_queue_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_thread_queue", "E: missing link on smt_thread_queue object");
        smt_thread_queue_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_thread_queue_free_ ((smt_thread_queue_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_thread_queue_t *
    smt_thread_queue_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_thread_queue_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_thread_queue_cache_initialise ();

self = (smt_thread_queue_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_thread_queue_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_free

    Type: Component method
    Freess a smt_thread_queue_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_thread_queue_free_ (
    smt_thread_queue_t * self,          //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = self->links;
#endif

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (smt_thread_queue_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_thread_queue_t));
        self->object_tag = SMT_THREAD_QUEUE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_read_lock

    Type: Component method
    Accepts a smt_thread_queue_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_read_lock (
    smt_thread_queue_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_THREAD_QUEUE_ASSERT_SANE (self);
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
    smt_thread_queue_write_lock

    Type: Component method
    Accepts a smt_thread_queue_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_write_lock (
    smt_thread_queue_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_THREAD_QUEUE_ASSERT_SANE (self);
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
    smt_thread_queue_unlock

    Type: Component method
    Accepts a smt_thread_queue_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_thread_queue_unlock (
    smt_thread_queue_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_THREAD_QUEUE_ASSERT_SANE (self);
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
    smt_thread_queue_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_thread_queue_t *
    smt_thread_queue_link_ (
    smt_thread_queue_t * self,          //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
    int
        history_last;
#endif

    if (self) {
        SMT_THREAD_QUEUE_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_thread_queue_unlink_ (
    smt_thread_queue_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
    int
        history_last;
#endif

    smt_thread_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        SMT_THREAD_QUEUE_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_thread_queue", "E: missing link on smt_thread_queue object");
            smt_thread_queue_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_QUEUE)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_THREAD_QUEUE_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_thread_queue_annihilate_ (self_p, file, line);
        smt_thread_queue_free_ ((smt_thread_queue_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_thread_queue_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_thread_queue_t));
icl_system_register (smt_thread_queue_cache_purge, smt_thread_queue_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_thread_queue_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_thread_queue_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_thread_queue_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_thread_queue_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_thread_queue_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_thread_queue_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_thread_queue_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_queue_new_in_scope_ (
    smt_thread_queue_t * * self_p,      //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_thread_queue_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_thread_queue_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_thread_queue_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_thread_queue_component   = "smt_thread_queue ";
char *EMBED__smt_thread_queue_version     = "1.0 ";
char *EMBED__smt_thread_queue_copyright   = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_thread_queue_filename    = "smt_thread_queue.icl ";
char *EMBED__smt_thread_queue_builddate   = "2011/03/01 ";
char *EMBED__smt_thread_queue_version_end  = "VeRsIoNeNd:ipc";

