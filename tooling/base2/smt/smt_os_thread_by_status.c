/*---------------------------------------------------------------------------
    smt_os_thread_by_status.c - smt_os_thread_by_status component

    This class implements the list container for smt_os_thread
    Generated from smt_os_thread_by_status.icl by icl_gen using GSL/4.
    
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
#include "smt_os_thread_by_status.h"    //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_os_thread_by_status_t

//  Shorthands for class methods

#define self_new            smt_os_thread_by_status_new
#define self_annihilate     smt_os_thread_by_status_annihilate
#define self_selftest       smt_os_thread_by_status_selftest
#define self_isempty        smt_os_thread_by_status_isempty
#define self_push           smt_os_thread_by_status_push
#define self_pop            smt_os_thread_by_status_pop
#define self_queue          smt_os_thread_by_status_queue
#define self_relink_before  smt_os_thread_by_status_relink_before
#define self_remove         smt_os_thread_by_status_remove
#define self_first          smt_os_thread_by_status_first
#define self_last           smt_os_thread_by_status_last
#define self_next           smt_os_thread_by_status_next
#define self_prev           smt_os_thread_by_status_prev
#define self_rehook         smt_os_thread_by_status_rehook
#define self_unhook         smt_os_thread_by_status_unhook
#define self_terminate      smt_os_thread_by_status_terminate
#define self_show           smt_os_thread_by_status_show
#define self_destroy        smt_os_thread_by_status_destroy
#define self_alloc          smt_os_thread_by_status_alloc
#define self_free           smt_os_thread_by_status_free
#define self_read_lock      smt_os_thread_by_status_read_lock
#define self_write_lock     smt_os_thread_by_status_write_lock
#define self_unlock         smt_os_thread_by_status_unlock
#define self_link           smt_os_thread_by_status_link
#define self_unlink         smt_os_thread_by_status_unlink
#define self_cache_initialise  smt_os_thread_by_status_cache_initialise
#define self_cache_purge    smt_os_thread_by_status_cache_purge
#define self_cache_terminate  smt_os_thread_by_status_cache_terminate
#define self_show_animation  smt_os_thread_by_status_show_animation
#define self_new_in_scope   smt_os_thread_by_status_new_in_scope

#define smt_os_thread_by_status_annihilate(self)  smt_os_thread_by_status_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_os_thread_by_status_annihilate_ (
smt_os_thread_by_status_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static inline void
    smt_os_thread_by_status_rehook (
smt_os_thread_t * item,                 //  Not documented
smt_os_thread_t * left,                 //  Not documented
smt_os_thread_t * right                 //  Not documented
);

static inline smt_os_thread_t *
    smt_os_thread_by_status_unhook (
smt_os_thread_t * item                  //  Not documented
);

#define smt_os_thread_by_status_alloc()  smt_os_thread_by_status_alloc_ (__FILE__, __LINE__)
static smt_os_thread_by_status_t *
    smt_os_thread_by_status_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_os_thread_by_status_free(self)  smt_os_thread_by_status_free_ (self, __FILE__, __LINE__)
static void
    smt_os_thread_by_status_free_ (
smt_os_thread_by_status_t * self,       //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_os_thread_by_status_cache_initialise (
void);

static void
    smt_os_thread_by_status_cache_purge (
void);

static void
    smt_os_thread_by_status_cache_terminate (
void);

Bool
    smt_os_thread_by_status_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_new

    Type: Component method
    Creates and initialises a new smt_os_thread_by_status_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_os_thread_by_status_t *
    smt_os_thread_by_status_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_os_thread_by_status_t *
        self = NULL;                    //  Object reference

    self = smt_os_thread_by_status_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_OS_THREAD_BY_STATUS_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
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
        icl_mem_set_callback (self, smt_os_thread_by_status_show_);
#endif

self->smt_os_thread.by_status_next = &self->smt_os_thread;
self->smt_os_thread.by_status_prev = &self->smt_os_thread;
self->smt_os_thread.by_status_head = self;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_by_status_annihilate_ (
    smt_os_thread_by_status_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
    int
        history_last;
#endif

smt_os_thread_t
    *item;
    smt_os_thread_by_status_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links;
#endif

    SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

item = (smt_os_thread_t *) self->smt_os_thread.by_status_next;
while (item != &self->smt_os_thread) {
    //  Unlink child without lock (we already have the lock)
    smt_os_thread_by_status_unhook (item);

    smt_os_thread_unlink (&item);
    item = (smt_os_thread_t *) self->smt_os_thread.by_status_next;
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_by_status_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_isempty

    Type: Component method
    Accepts a smt_os_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_isempty (
    smt_os_thread_by_status_t * self    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

rc = (self->smt_os_thread.by_status_next == &self->smt_os_thread);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_push

    Type: Component method
    Accepts a smt_os_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_push (
    smt_os_thread_by_status_t * self,   //  Reference to object
    smt_os_thread_t * item              //  Not documented
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

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

//  If by_status_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_status_head) {
    smt_os_thread_link (item);
}
else
if (item->by_status_head != self) {
    icl_console_print ("E: smt_os_thread item inserted into multiple by_status containers");
    assert (item->by_status_head == self);
}
smt_os_thread_by_status_unhook (item);
smt_os_thread_by_status_rehook (item, &self->smt_os_thread, (smt_os_thread_t *) self->smt_os_thread.by_status_next);
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
    smt_os_thread_by_status_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

smt_os_thread_t *
    smt_os_thread_by_status_pop (
    smt_os_thread_by_status_t * self    //  The list
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_os_thread_t *
        item = NULL;                    //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

if (self->smt_os_thread.by_status_next == &self->smt_os_thread)
    item = NULL;
else {
    item = smt_os_thread_by_status_unhook ((smt_os_thread_t *) self->smt_os_thread.by_status_next);
}
}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (item);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_queue

    Type: Component method
    Accepts a smt_os_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_queue (
    smt_os_thread_by_status_t * self,   //  Reference to object
    smt_os_thread_t * item              //  Not documented
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

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

//  If by_status_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_status_head) {
    smt_os_thread_link (item);
}
else
if (item->by_status_head != self) {
    icl_console_print ("E: smt_os_thread item inserted into multiple by_status containers");
    assert (item->by_status_head == self);
}
smt_os_thread_by_status_unhook (item);
smt_os_thread_by_status_rehook (item, (smt_os_thread_t *) self->smt_os_thread.by_status_prev, &self->smt_os_thread);
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
    smt_os_thread_by_status_relink_before

    Type: Component method
    Accepts a smt_os_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_relink_before (
    smt_os_thread_t * item,             //  Not documented
    smt_os_thread_t * where             //  Not documented
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code
    smt_os_thread_by_status_t *
        self = (smt_os_thread_by_status_t *) where->by_status_head;  //  The list


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

//  If by_status_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_status_head) {
    smt_os_thread_link (item);
}
else
if (item->by_status_head != self) {
    icl_console_print ("E: smt_os_thread item inserted into multiple by_status containers");
    assert (item->by_status_head == self);
}
smt_os_thread_by_status_unhook (item);
smt_os_thread_by_status_rehook (item, (smt_os_thread_t *) where->by_status_prev, where);
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
    smt_os_thread_by_status_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_remove (
    smt_os_thread_t * item              //  Not documented
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_os_thread_by_status_t *
        self = (smt_os_thread_by_status_t *) item->by_status_head;  //  The list
    int
        rc = 0;                         //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

if (self
&&  self == item->by_status_head) { //  Catch case where item moved
    smt_os_thread_by_status_unhook (item);
    smt_os_thread_unlink (&item);
}
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_first

    Type: Component method
    Returns the first smt_os_thread item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_os_thread_t *
    smt_os_thread_by_status_first_ (
    smt_os_thread_by_status_t * self,   //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_os_thread_t *
        item = NULL;                    //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_os_thread_t *) self->smt_os_thread.by_status_next;
if (item == &self->smt_os_thread)
    item = NULL;
else
    smt_os_thread_link (item);

}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (item);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_last

    Type: Component method
    Returns the last smt_os_thread item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_os_thread_t *
    smt_os_thread_by_status_last_ (
    smt_os_thread_by_status_t * self,   //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_os_thread_t *
        item = NULL;                    //  Not documented


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_os_thread_t *) self->smt_os_thread.by_status_prev;
if (item == &self->smt_os_thread)
    item = NULL;
else
    smt_os_thread_link (item);

}

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (item);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_next

    Type: Component method
    Returns the following item on the smt_os_thread list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_os_thread_t *
    smt_os_thread_by_status_next_ (
    smt_os_thread_t * ( * item_p ),     //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_os_thread_t *
        item = *item_p;                 //  Dereferenced item
    smt_os_thread_t *
        next = NULL;                    //  The item to return
    smt_os_thread_by_status_t *
        self = (smt_os_thread_by_status_t *) item->by_status_head;  //  The list


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

if (self
&&  self == item->by_status_head) { //  Catch case where item moved
    next = (smt_os_thread_t *) item->by_status_next;
    if (next == &self->smt_os_thread)
        next = NULL;
    else
        smt_os_thread_link (next);

}
smt_os_thread_unlink (item_p);

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (next);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_prev

    Type: Component method
    Returns the preceding item on the smt_os_thread list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_os_thread_t *
    smt_os_thread_by_status_prev_ (
    smt_os_thread_t * ( * item_p ),     //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    smt_os_thread_t *
        item = *item_p;                 //  Dereferenced item
    smt_os_thread_t *
        prev = NULL;                    //  The item to return
    smt_os_thread_by_status_t *
        self = (smt_os_thread_by_status_t *) item->by_status_head;  //  The list


#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

if (self
&&  self == item->by_status_head) { //  Catch case where item moved
    prev = (smt_os_thread_t *) item->by_status_prev;
    if (prev == &self->smt_os_thread)
        prev = NULL;
    else
        smt_os_thread_link (prev);

}
smt_os_thread_unlink (item_p);

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif



    return (prev);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    smt_os_thread_by_status_rehook (
    smt_os_thread_t * item,             //  Not documented
    smt_os_thread_t * left,             //  Not documented
    smt_os_thread_t * right             //  Not documented
)
{
smt_os_thread_t
    *swap;

swap = (smt_os_thread_t *) left->by_status_next;
left->by_status_next = item->by_status_next;
item->by_status_next = swap;

swap = (smt_os_thread_t *) item->by_status_prev;
item->by_status_prev = right->by_status_prev;
right->by_status_prev = swap;

item->by_status_head = left->by_status_head;
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline smt_os_thread_t *
    smt_os_thread_by_status_unhook (
    smt_os_thread_t * item              //  Not documented
)
{

smt_os_thread_by_status_rehook (item, (smt_os_thread_t *) item->by_status_prev, (smt_os_thread_t *) item->by_status_next);
item->by_status_head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_by_status_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_by_status_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_os_thread_by_status_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_os_thread_by_status zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
    if (self->history_last > SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH;
        self->history_last %= SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_os_thread_by_status>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_destroy

    Type: Component method
    Destroys a smt_os_thread_by_status_t object. Takes the address of a
    smt_os_thread_by_status_t reference (a pointer to a pointer) and nullifies the
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
    smt_os_thread_by_status_destroy_ (
    smt_os_thread_by_status_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_os_thread_by_status_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_os_thread_by_status_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_os_thread_by_status", "E: missing link on smt_os_thread_by_status object");
        smt_os_thread_by_status_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_os_thread_by_status_free_ ((smt_os_thread_by_status_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_os_thread_by_status_t *
    smt_os_thread_by_status_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_os_thread_by_status_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_os_thread_by_status_cache_initialise ();

self = (smt_os_thread_by_status_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_os_thread_by_status_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_free

    Type: Component method
    Freess a smt_os_thread_by_status_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_by_status_free_ (
    smt_os_thread_by_status_t * self,   //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links;
#endif

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (smt_os_thread_by_status_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_os_thread_by_status_t));
        self->object_tag = SMT_OS_THREAD_BY_STATUS_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_read_lock

    Type: Component method
    Accepts a smt_os_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_read_lock (
    smt_os_thread_by_status_t * self    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
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
    smt_os_thread_by_status_write_lock

    Type: Component method
    Accepts a smt_os_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_write_lock (
    smt_os_thread_by_status_t * self    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
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
    smt_os_thread_by_status_unlock

    Type: Component method
    Accepts a smt_os_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_by_status_unlock (
    smt_os_thread_by_status_t * self    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
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
    smt_os_thread_by_status_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_os_thread_by_status_t *
    smt_os_thread_by_status_link_ (
    smt_os_thread_by_status_t * self,   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
    int
        history_last;
#endif

    if (self) {
        SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_by_status_unlink_ (
    smt_os_thread_by_status_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
    int
        history_last;
#endif

    smt_os_thread_by_status_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        SMT_OS_THREAD_BY_STATUS_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_os_thread_by_status", "E: missing link on smt_os_thread_by_status object");
            smt_os_thread_by_status_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD_BY_STATUS)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_OS_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_os_thread_by_status_annihilate_ (self_p, file, line);
        smt_os_thread_by_status_free_ ((smt_os_thread_by_status_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_by_status_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_os_thread_by_status_t));
icl_system_register (smt_os_thread_by_status_cache_purge, smt_os_thread_by_status_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_by_status_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_by_status_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_by_status_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_os_thread_by_status_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_os_thread_by_status_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_by_status_new_in_scope_ (
    smt_os_thread_by_status_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_os_thread_by_status_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_os_thread_by_status_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_os_thread_by_status_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_os_thread_by_status_component  = "smt_os_thread_by_status ";
char *EMBED__smt_os_thread_by_status_version  = "1.0 ";
char *EMBED__smt_os_thread_by_status_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_os_thread_by_status_filename  = "smt_os_thread_by_status.icl ";
char *EMBED__smt_os_thread_by_status_builddate  = "2011/03/01 ";
char *EMBED__smt_os_thread_by_status_version_end  = "VeRsIoNeNd:ipc";

