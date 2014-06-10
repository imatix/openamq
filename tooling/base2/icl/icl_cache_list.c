/*---------------------------------------------------------------------------
    icl_cache_list.c - icl_cache_list component

    This class implements the list container for icl_cache
    Generated from icl_cache_list.icl by icl_gen using GSL/4.
    
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
#include "icl_cache_list.h"             //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_cache_list_t

//  Shorthands for class methods

#define self_new            icl_cache_list_new
#define self_annihilate     icl_cache_list_annihilate
#define self_selftest       icl_cache_list_selftest
#define self_isempty        icl_cache_list_isempty
#define self_push           icl_cache_list_push
#define self_pop            icl_cache_list_pop
#define self_queue          icl_cache_list_queue
#define self_relink_before  icl_cache_list_relink_before
#define self_remove         icl_cache_list_remove
#define self_first          icl_cache_list_first
#define self_last           icl_cache_list_last
#define self_next           icl_cache_list_next
#define self_prev           icl_cache_list_prev
#define self_rehook         icl_cache_list_rehook
#define self_unhook         icl_cache_list_unhook
#define self_terminate      icl_cache_list_terminate
#define self_show           icl_cache_list_show
#define self_destroy        icl_cache_list_destroy
#define self_alloc          icl_cache_list_alloc
#define self_free           icl_cache_list_free
#define self_lock           icl_cache_list_lock
#define self_unlock         icl_cache_list_unlock
#define self_show_animation  icl_cache_list_show_animation
#define self_new_in_scope   icl_cache_list_new_in_scope

static void
    icl_cache_list_annihilate (
icl_cache_list_t * ( * self_p )         //  Reference to object reference
);

static inline void
    icl_cache_list_rehook (
icl_cache_t * item,                     //  Not documented
icl_cache_t * left,                     //  Not documented
icl_cache_t * right                     //  Not documented
);

static inline icl_cache_t *
    icl_cache_list_unhook (
icl_cache_t * item                      //  Not documented
);

#define icl_cache_list_alloc()          icl_cache_list_alloc_ (__FILE__, __LINE__)
static icl_cache_list_t *
    icl_cache_list_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_cache_list_free (
icl_cache_list_t * self                 //  Object reference
);

Bool
    icl_cache_list_animating = TRUE;    //  Animation enabled by default

/*  -------------------------------------------------------------------------
    icl_cache_list_new

    Type: Component method
    Creates and initialises a new icl_cache_list_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_cache_list_t *
    icl_cache_list_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_cache_list_t *
        self = NULL;                    //  Object reference

    self = icl_cache_list_alloc_ (file, line);
    if (self) {
        self->object_tag   = ICL_CACHE_LIST_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->mutex = icl_mutex_new ();
#endif

self->icl_cache.list_next = &self->icl_cache;
self->icl_cache.list_prev = &self->icl_cache;
self->icl_cache.list_head = self;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    icl_cache_list_annihilate (
    icl_cache_list_t * ( * self_p )     //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

icl_cache_t
    *item;
    icl_cache_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    ICL_CACHE_LIST_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    mutex = self->mutex;
    if (mutex)
         icl_mutex_lock (mutex);
#endif

item = (icl_cache_t *) self->icl_cache.list_next;
while (item != &self->icl_cache) {
    //  Unlink child without lock (we already have the lock)
    icl_cache_list_unhook (item);

    icl_cache_destroy (&item);
    item = (icl_cache_t *) self->icl_cache.list_next;
}
#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

}
/*  -------------------------------------------------------------------------
    icl_cache_list_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cache_list_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_cache_list_isempty

    Type: Component method
    Accepts a icl_cache_list_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    icl_cache_list_isempty (
    icl_cache_list_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_CACHE_LIST_ASSERT_SANE (self);

rc = (self->icl_cache.list_next == &self->icl_cache);


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_push

    Type: Component method
    Accepts a icl_cache_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    icl_cache_list_push (
    icl_cache_list_t * self,            //  Reference to object
    icl_cache_t * item                  //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


ICL_CACHE_LIST_ASSERT_SANE (self);

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
}
else
if (item->list_head != self) {
    icl_console_print ("E: icl_cache item inserted into multiple list containers");
    assert (item->list_head == self);
}
icl_cache_list_unhook (item);
icl_cache_list_rehook (item, &self->icl_cache, (icl_cache_t *) self->icl_cache.list_next);


#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

icl_cache_t *
    icl_cache_list_pop (
    icl_cache_list_t * self             //  The list
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    icl_cache_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


ICL_CACHE_LIST_ASSERT_SANE (self);

if (self->icl_cache.list_next == &self->icl_cache)
    item = NULL;
else {
    item = icl_cache_list_unhook ((icl_cache_t *) self->icl_cache.list_next);
}


#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_queue

    Type: Component method
    Accepts a icl_cache_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    icl_cache_list_queue (
    icl_cache_list_t * self,            //  Reference to object
    icl_cache_t * item                  //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


ICL_CACHE_LIST_ASSERT_SANE (self);

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
}
else
if (item->list_head != self) {
    icl_console_print ("E: icl_cache item inserted into multiple list containers");
    assert (item->list_head == self);
}
icl_cache_list_unhook (item);
icl_cache_list_rehook (item, (icl_cache_t *) self->icl_cache.list_prev, &self->icl_cache);


#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_relink_before

    Type: Component method
    Accepts a icl_cache_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    icl_cache_list_relink_before (
    icl_cache_t * item,                 //  Not documented
    icl_cache_t * where                 //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    int
        rc = 0;                         //  Return code
    icl_cache_list_t *
        self = (icl_cache_list_t *) where->list_head;  //  The list

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


ICL_CACHE_LIST_ASSERT_SANE (self);

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
}
else
if (item->list_head != self) {
    icl_console_print ("E: icl_cache item inserted into multiple list containers");
    assert (item->list_head == self);
}
icl_cache_list_unhook (item);
icl_cache_list_rehook (item, (icl_cache_t *) where->list_prev, where);


#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    icl_cache_list_remove (
    icl_cache_t * item                  //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    icl_cache_list_t *
        self = (icl_cache_list_t *) item->list_head;  //  The list
    int
        rc = 0;                         //  Not documented

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


if (self
&&  self == item->list_head) { //       Catch case where item moved
    icl_cache_list_unhook (item);
}

#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_first

    Type: Component method
    Returns the first icl_cache item on the list.     If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

icl_cache_t *
    icl_cache_list_first_ (
    icl_cache_list_t * self,            //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    icl_cache_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


ICL_CACHE_LIST_ASSERT_SANE (self);

item = (icl_cache_t *) self->icl_cache.list_next;
if (item == &self->icl_cache)
    item = NULL;



#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_last

    Type: Component method
    Returns the last icl_cache item on the list.     If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

icl_cache_t *
    icl_cache_list_last_ (
    icl_cache_list_t * self,            //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    icl_cache_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


ICL_CACHE_LIST_ASSERT_SANE (self);

item = (icl_cache_t *) self->icl_cache.list_prev;
if (item == &self->icl_cache)
    item = NULL;



#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_next

    Type: Component method
    Returns the following item on the icl_cache list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

icl_cache_t *
    icl_cache_list_next_ (
    icl_cache_t * ( * item_p ),         //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    icl_cache_t *
        item = *item_p;                 //  Dereferenced item
    icl_cache_t *
        next = NULL;                    //  The item to return
    icl_cache_list_t *
        self = (icl_cache_list_t *) item->list_head;  //  The list

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


if (self
&&  self == item->list_head) { //       Catch case where item moved
    next = (icl_cache_t *) item->list_next;
    if (next == &self->icl_cache)
        next = NULL;

}


#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (next);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_prev

    Type: Component method
    Returns the preceding item on the icl_cache list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

icl_cache_t *
    icl_cache_list_prev_ (
    icl_cache_t * ( * item_p ),         //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif


    icl_cache_t *
        item = *item_p;                 //  Dereferenced item
    icl_cache_t *
        prev = NULL;                    //  The item to return
    icl_cache_list_t *
        self = (icl_cache_list_t *) item->list_head;  //  The list

#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif


if (self
&&  self == item->list_head) { //       Catch case where item moved
    prev = (icl_cache_t *) item->list_prev;
    if (prev == &self->icl_cache)
        prev = NULL;

}


#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif


    return (prev);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_cache_list_rehook (
    icl_cache_t * item,                 //  Not documented
    icl_cache_t * left,                 //  Not documented
    icl_cache_t * right                 //  Not documented
)
{
icl_cache_t
    *swap;

swap = (icl_cache_t *) left->list_next;
left->list_next = item->list_next;
item->list_next = swap;

swap = (icl_cache_t *) item->list_prev;
item->list_prev = right->list_prev;
right->list_prev = swap;

item->list_head = left->list_head;
}
/*  -------------------------------------------------------------------------
    icl_cache_list_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline icl_cache_t *
    icl_cache_list_unhook (
    icl_cache_t * item                  //  Not documented
)
{

icl_cache_list_rehook (item, (icl_cache_t *) item->list_prev, (icl_cache_t *) item->list_next);
item->list_head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cache_list_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    icl_cache_list_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cache_list_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_cache_list_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_cache_list file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_cache_list_destroy

    Type: Component method
    Destroys a icl_cache_list_t object. Takes the address of a
    icl_cache_list_t reference (a pointer to a pointer) and nullifies the
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
    icl_cache_list_destroy_ (
    icl_cache_list_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_cache_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_cache_list_annihilate (self_p);
    icl_cache_list_free ((icl_cache_list_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_cache_list_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_cache_list_t *
    icl_cache_list_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_cache_list_t *
        self = NULL;                    //  Object reference

self = (icl_cache_list_t *) malloc (sizeof (icl_cache_list_t));
if (self)
    memset (self, 0, sizeof (icl_cache_list_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_free

    Type: Component method
    Freess a icl_cache_list_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_cache_list_free (
    icl_cache_list_t * self             //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->mutex)
        icl_mutex_destroy (&self->mutex);
#endif
        memset (&self->object_tag, 0, sizeof (icl_cache_list_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_cache_list_t));
        self->object_tag = ICL_CACHE_LIST_DEAD;
        free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_cache_list_lock

    Type: Component method
    Accepts a icl_cache_list_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_cache_list_lock (
    icl_cache_list_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_CACHE_LIST_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_mutex_lock (self->mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_unlock

    Type: Component method
    Accepts a icl_cache_list_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    icl_cache_list_unlock (
    icl_cache_list_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_CACHE_LIST_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_mutex_unlock (self->mutex);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cache_list_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_cache_list_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_cache_list_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_cache_list_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_cache_list_new_in_scope_ (
    icl_cache_list_t * * self_p,        //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_cache_list_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_cache_list_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_cache_list_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_cache_list_component     = "icl_cache_list ";
char *EMBED__icl_cache_list_version       = "1.0 ";
char *EMBED__icl_cache_list_copyright     = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_cache_list_filename      = "icl_cache_list.icl ";
char *EMBED__icl_cache_list_builddate     = "2011/03/01 ";
char *EMBED__icl_cache_list_version_end   = "VeRsIoNeNd:ipc";

