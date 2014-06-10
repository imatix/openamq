/*---------------------------------------------------------------------------
    demo_content_basic_list.c - demo_content_basic_list component

    Generated from demo_content_basic_list.icl by icl_gen using GSL/4.
    
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
#include "demo_content_basic_list.h"    //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_content_basic_list_t

//  Shorthands for class methods

#define self_selftest       demo_content_basic_list_selftest
#define self_iter_new       demo_content_basic_list_iter_new
#define self_iter_object    demo_content_basic_list_iter_object
#define self_iter_destroy   demo_content_basic_list_iter_destroy
#define self_new            demo_content_basic_list_new
#define self_annihilate     demo_content_basic_list_annihilate
#define self_isempty        demo_content_basic_list_isempty
#define self_push           demo_content_basic_list_push
#define self_pop            demo_content_basic_list_pop
#define self_queue          demo_content_basic_list_queue
#define self_relink_before  demo_content_basic_list_relink_before
#define self_remove         demo_content_basic_list_remove
#define self_first          demo_content_basic_list_first
#define self_last           demo_content_basic_list_last
#define self_next           demo_content_basic_list_next
#define self_prev           demo_content_basic_list_prev
#define self_count          demo_content_basic_list_count
#define self_rehook         demo_content_basic_list_rehook
#define self_unhook         demo_content_basic_list_unhook
#define self_cache_initialise  demo_content_basic_list_cache_initialise
#define self_cache_purge    demo_content_basic_list_cache_purge
#define self_terminate      demo_content_basic_list_terminate
#define self_show           demo_content_basic_list_show
#define self_destroy        demo_content_basic_list_destroy
#define self_alloc          demo_content_basic_list_alloc
#define self_free           demo_content_basic_list_free
#define self_cache_terminate  demo_content_basic_list_cache_terminate
#define self_show_animation  demo_content_basic_list_show_animation
#define self_new_in_scope   demo_content_basic_list_new_in_scope

static void
    demo_content_basic_list_annihilate (
demo_content_basic_list_t * ( * self_p )  //  Reference to object reference
);

static inline void
    demo_content_basic_list_rehook (
demo_content_basic_list_iter_t * item,   //  Not documented
demo_content_basic_list_iter_t * left,   //  Not documented
demo_content_basic_list_iter_t * right  //  Not documented
);

static inline demo_content_basic_list_iter_t *
    demo_content_basic_list_unhook (
demo_content_basic_list_iter_t * item   //  Not documented
);

static void
    demo_content_basic_list_cache_initialise (
void);

static void
    demo_content_basic_list_cache_purge (
void);

#define demo_content_basic_list_alloc()  demo_content_basic_list_alloc_ (__FILE__, __LINE__)
static demo_content_basic_list_t *
    demo_content_basic_list_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_content_basic_list_free (
demo_content_basic_list_t * self        //  Object reference
);

static void
    demo_content_basic_list_cache_terminate (
void);

Bool
    demo_content_basic_list_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static icl_cache_t
    *s_iter_cache = NULL;
/*  -------------------------------------------------------------------------
    demo_content_basic_list_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_content_basic_list_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_iter_new

    Type: Component method
    -------------------------------------------------------------------------
 */

demo_content_basic_list_iter_t *
    demo_content_basic_list_iter_new (
    demo_content_basic_t * item         //  Not documented
)
{
    demo_content_basic_list_iter_t *
        iter;                           //  Not documented

iter = icl_mem_cache_alloc (s_iter_cache);
iter->next = iter;
iter->prev = iter;
iter->head = NULL;
iter->item = demo_content_basic_link (item);


    return (iter);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_iter_object

    Type: Component method
    -------------------------------------------------------------------------
 */

demo_content_basic_t *
    demo_content_basic_list_iter_object (
    demo_content_basic_list_iter_t * iter  //  Not documented
)
{
    demo_content_basic_t *
        item;                           //  Not documented

item = demo_content_basic_link (iter->item);


    return (item);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_iter_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_content_basic_list_iter_destroy (
    demo_content_basic_list_iter_t * ( * iter_p )  //  Not documented
)
{
    demo_content_basic_list_iter_t *
        iter = *iter_p;                 //  Dereferenced iter

if (iter) {
    demo_content_basic_list_remove (iter);
    demo_content_basic_unlink (&iter->item);
    icl_mem_free (iter);
    *iter_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_new

    Type: Component method
    Creates and initialises a new demo_content_basic_list_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

demo_content_basic_list_t *
    demo_content_basic_list_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    demo_content_basic_list_t *
        self = NULL;                    //  Object reference

    self = demo_content_basic_list_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_CONTENT_BASIC_LIST_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_content_basic_list_show_);
#endif

self->demo_content_basic_list_iter.next = &self->demo_content_basic_list_iter;
self->demo_content_basic_list_iter.prev = &self->demo_content_basic_list_iter;
self->demo_content_basic_list_iter.head = self;
self->demo_content_basic_list_iter.item = NULL;
self->count = 0;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    demo_content_basic_list_annihilate (
    demo_content_basic_list_t * ( * self_p )  //  Reference to object reference
)
{

demo_content_basic_list_iter_t
    *item;
    demo_content_basic_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

item = (demo_content_basic_list_iter_t *) self->demo_content_basic_list_iter.next;
while (item != &self->demo_content_basic_list_iter) {
    //  Unlink child without lock (we already have the lock)
    demo_content_basic_list_unhook (item);

    demo_content_basic_list_iter_destroy (&item);
    item = (demo_content_basic_list_iter_t *) self->demo_content_basic_list_iter.next;
}

}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_isempty

    Type: Component method
    Accepts a demo_content_basic_list_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    demo_content_basic_list_isempty (
    demo_content_basic_list_t * self    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

rc = (self->demo_content_basic_list_iter.next == &self->demo_content_basic_list_iter);


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_push

    Type: Component method
    Accepts a demo_content_basic_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    demo_content_basic_list_push (
    demo_content_basic_list_t * self,   //  Reference to object
    demo_content_basic_t * item         //  Not documented
)
{


demo_content_basic_list_iter_t
    *iter;
    int
        rc = 0;                         //  Return code



DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

iter = demo_content_basic_list_iter_new (item);
icl_atomic_inc32 (&self->count);
demo_content_basic_list_rehook (iter, &self->demo_content_basic_list_iter, (demo_content_basic_list_iter_t *) self->demo_content_basic_list_iter.next);




    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

demo_content_basic_t *
    demo_content_basic_list_pop (
    demo_content_basic_list_t * self    //  The list
)
{


demo_content_basic_list_iter_t
    *iter = NULL;
    demo_content_basic_t *
        item = NULL;                    //  Not documented



DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

if (self->demo_content_basic_list_iter.next != &self->demo_content_basic_list_iter) {
    iter = demo_content_basic_list_unhook ((demo_content_basic_list_iter_t *) self->demo_content_basic_list_iter.next);
    icl_atomic_dec32 (&self->count);
    item = iter->item;
    iter->item = NULL;          //  So don't destroy item
    demo_content_basic_list_iter_destroy (&iter);
}




    return (item);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_queue

    Type: Component method
    Accepts a demo_content_basic_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list. If the item
    was on another list, it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    demo_content_basic_list_queue (
    demo_content_basic_list_t * self,   //  Reference to object
    demo_content_basic_t * item         //  Not documented
)
{


demo_content_basic_list_iter_t
    *iter;
    int
        rc = 0;                         //  Return code



DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

iter = demo_content_basic_list_iter_new (item);
icl_atomic_inc32 (&self->count);
demo_content_basic_list_rehook (iter, (demo_content_basic_list_iter_t *) self->demo_content_basic_list_iter.prev, &self->demo_content_basic_list_iter);




    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_relink_before

    Type: Component method
    Accepts a demo_content_basic_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    demo_content_basic_list_relink_before (
    demo_content_basic_list_iter_t * item,   //  Not documented
    demo_content_basic_list_iter_t * where  //  Not documented
)
{


    int
        rc = 0;                         //  Return code
    demo_content_basic_list_t *
        self = (demo_content_basic_list_t *) where->head;  //  The list



DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

//  If head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->head) {
    icl_atomic_inc32 (&self->count);
}
else
if (item->head != self) {
    icl_console_print ("E: demo_content_basic item inserted into multiple list containers");
    assert (item->head == self);
}
demo_content_basic_list_unhook (item);
demo_content_basic_list_rehook (item, (demo_content_basic_list_iter_t *) where->prev, where);




    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_remove

    Type: Component method
    Removes the demo_content_basic_list_iter item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    demo_content_basic_list_remove (
    demo_content_basic_list_iter_t * item  //  Not documented
)
{


    demo_content_basic_list_t *
        self = (demo_content_basic_list_t *) item->head;  //  The list
    int
        rc = 0;                         //  Not documented



if (self
&&  self == item->head) { //  Catch case where item moved
    demo_content_basic_list_unhook (item);
    icl_atomic_dec32 (&self->count);
}



    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_first

    Type: Component method
    Returns the first demo_content_basic_list_iter item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

demo_content_basic_list_iter_t *
    demo_content_basic_list_first_ (
    demo_content_basic_list_t * self,   //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    demo_content_basic_list_iter_t *
        item = NULL;                    //  Not documented



DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

item = (demo_content_basic_list_iter_t *) self->demo_content_basic_list_iter.next;
if (item == &self->demo_content_basic_list_iter)
    item = NULL;




    return (item);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_last

    Type: Component method
    Returns the last demo_content_basic_list_iter item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

demo_content_basic_list_iter_t *
    demo_content_basic_list_last_ (
    demo_content_basic_list_t * self,   //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    demo_content_basic_list_iter_t *
        item = NULL;                    //  Not documented



DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);

item = (demo_content_basic_list_iter_t *) self->demo_content_basic_list_iter.prev;
if (item == &self->demo_content_basic_list_iter)
    item = NULL;




    return (item);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_next

    Type: Component method
    Returns the following item on the demo_content_basic_list_iter list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

demo_content_basic_list_iter_t *
    demo_content_basic_list_next_ (
    demo_content_basic_list_iter_t * ( * item_p ),   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    demo_content_basic_list_iter_t *
        item = *item_p;                 //  Dereferenced item
    demo_content_basic_list_iter_t *
        next = NULL;                    //  The item to return
    demo_content_basic_list_t *
        self = (demo_content_basic_list_t *) item->head;  //  The list



if (self
&&  self == item->head) { //  Catch case where item moved
    next = (demo_content_basic_list_iter_t *) item->next;
    if (next == &self->demo_content_basic_list_iter)
        next = NULL;
}



    return (next);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_prev

    Type: Component method
    Returns the preceding item on the demo_content_basic_list_iter list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

demo_content_basic_list_iter_t *
    demo_content_basic_list_prev_ (
    demo_content_basic_list_iter_t * ( * item_p ),   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    demo_content_basic_list_iter_t *
        item = *item_p;                 //  Dereferenced item
    demo_content_basic_list_iter_t *
        prev = NULL;                    //  The item to return
    demo_content_basic_list_t *
        self = (demo_content_basic_list_t *) item->head;  //  The list



if (self
&&  self == item->head) { //  Catch case where item moved
    prev = (demo_content_basic_list_iter_t *) item->prev;
    if (prev == &self->demo_content_basic_list_iter)
        prev = NULL;
}



    return (prev);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_count

    Type: Component method
    Return the count of the number of objects in the list.
    -------------------------------------------------------------------------
 */

qbyte
    demo_content_basic_list_count (
    demo_content_basic_list_t * self    //  The list
)
{
    qbyte
        count;                          //  Not documented

//
DEMO_CONTENT_BASIC_LIST_ASSERT_SANE (self);
icl_atomic_set32 (&count, self->count);

    return (count);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    demo_content_basic_list_rehook (
    demo_content_basic_list_iter_t * item,   //  Not documented
    demo_content_basic_list_iter_t * left,   //  Not documented
    demo_content_basic_list_iter_t * right  //  Not documented
)
{
demo_content_basic_list_iter_t
    *swap;

swap = (demo_content_basic_list_iter_t *) left->next;
left->next = item->next;
item->next = swap;

swap = (demo_content_basic_list_iter_t *) item->prev;
item->prev = right->prev;
right->prev = swap;

item->head = left->head;
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline demo_content_basic_list_iter_t *
    demo_content_basic_list_unhook (
    demo_content_basic_list_iter_t * item  //  Not documented
)
{

demo_content_basic_list_rehook (item, (demo_content_basic_list_iter_t *) item->prev, (demo_content_basic_list_iter_t *) item->next);
item->head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_content_basic_list_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_content_basic_list_t));
icl_system_register (demo_content_basic_list_cache_purge, demo_content_basic_list_cache_terminate);

s_iter_cache = icl_cache_get (sizeof (demo_content_basic_list_iter_t));
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_content_basic_list_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

icl_mem_cache_purge (s_iter_cache);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_content_basic_list_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_content_basic_list_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
demo_content_basic_list_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <demo_content_basic_list file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_destroy

    Type: Component method
    Destroys a demo_content_basic_list_t object. Takes the address of a
    demo_content_basic_list_t reference (a pointer to a pointer) and nullifies the
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
    demo_content_basic_list_destroy_ (
    demo_content_basic_list_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    demo_content_basic_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    demo_content_basic_list_annihilate (self_p);
    demo_content_basic_list_free ((demo_content_basic_list_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_content_basic_list_t *
    demo_content_basic_list_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_content_basic_list_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_content_basic_list_cache_initialise ();

self = (demo_content_basic_list_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_content_basic_list_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_free

    Type: Component method
    Freess a demo_content_basic_list_t object.
    -------------------------------------------------------------------------
 */

static void
    demo_content_basic_list_free (
    demo_content_basic_list_t * self    //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (demo_content_basic_list_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_content_basic_list_t));
        self->object_tag = DEMO_CONTENT_BASIC_LIST_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_content_basic_list_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_content_basic_list_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_content_basic_list_animating = enabled;
}
/*  -------------------------------------------------------------------------
    demo_content_basic_list_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_content_basic_list_new_in_scope_ (
    demo_content_basic_list_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_content_basic_list_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_content_basic_list_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__demo_content_basic_list_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_content_basic_list_component  = "demo_content_basic_list ";
char *EMBED__demo_content_basic_list_version  = "1.0 ";
char *EMBED__demo_content_basic_list_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_content_basic_list_filename  = "demo_content_basic_list.icl ";
char *EMBED__demo_content_basic_list_builddate  = "2011/03/01 ";
char *EMBED__demo_content_basic_list_version_end  = "VeRsIoNeNd:ipc";

