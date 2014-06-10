/*---------------------------------------------------------------------------
    ipr_bucket_list.c - ipr_bucket_list component

    Defines a bucket list.  You can use bucket lists to manage large
    amounts of data in memory without needing to pre-allocate the
    entire amount.
    Generated from ipr_bucket_list.icl by icl_gen using GSL/4.
    
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
#include "ipr_bucket_list.h"            //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_bucket_list_t

//  Shorthands for class methods

#define self_clear          ipr_bucket_list_clear
#define self_load           ipr_bucket_list_load
#define self_save           ipr_bucket_list_save
#define self_selftest       ipr_bucket_list_selftest
#define self_iter_new       ipr_bucket_list_iter_new
#define self_iter_object    ipr_bucket_list_iter_object
#define self_iter_destroy   ipr_bucket_list_iter_destroy
#define self_new            ipr_bucket_list_new
#define self_annihilate     ipr_bucket_list_annihilate
#define self_isempty        ipr_bucket_list_isempty
#define self_push           ipr_bucket_list_push
#define self_pop            ipr_bucket_list_pop
#define self_queue          ipr_bucket_list_queue
#define self_relink_before  ipr_bucket_list_relink_before
#define self_remove         ipr_bucket_list_remove
#define self_first          ipr_bucket_list_first
#define self_last           ipr_bucket_list_last
#define self_next           ipr_bucket_list_next
#define self_prev           ipr_bucket_list_prev
#define self_rehook         ipr_bucket_list_rehook
#define self_unhook         ipr_bucket_list_unhook
#define self_cache_initialise  ipr_bucket_list_cache_initialise
#define self_cache_purge    ipr_bucket_list_cache_purge
#define self_terminate      ipr_bucket_list_terminate
#define self_show           ipr_bucket_list_show
#define self_destroy        ipr_bucket_list_destroy
#define self_alloc          ipr_bucket_list_alloc
#define self_free           ipr_bucket_list_free
#define self_cache_terminate  ipr_bucket_list_cache_terminate
#define self_show_animation  ipr_bucket_list_show_animation
#define self_new_in_scope   ipr_bucket_list_new_in_scope

static void
    ipr_bucket_list_annihilate (
ipr_bucket_list_t * ( * self_p )        //  Reference to object reference
);

static inline void
    ipr_bucket_list_rehook (
ipr_bucket_list_iter_t * item,          //  Not documented
ipr_bucket_list_iter_t * left,          //  Not documented
ipr_bucket_list_iter_t * right          //  Not documented
);

static inline ipr_bucket_list_iter_t *
    ipr_bucket_list_unhook (
ipr_bucket_list_iter_t * item           //  Not documented
);

static void
    ipr_bucket_list_cache_initialise (
void);

static void
    ipr_bucket_list_cache_purge (
void);

#define ipr_bucket_list_alloc()         ipr_bucket_list_alloc_ (__FILE__, __LINE__)
static ipr_bucket_list_t *
    ipr_bucket_list_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_bucket_list_free (
ipr_bucket_list_t * self                //  Object reference
);

static void
    ipr_bucket_list_cache_terminate (
void);

Bool
    ipr_bucket_list_animating = TRUE;   //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static icl_cache_t
    *s_iter_cache = NULL;
/*  -------------------------------------------------------------------------
    ipr_bucket_list_clear

    Type: Component method
    Accepts a ipr_bucket_list_t reference and returns zero in case of success,
    1 in case of errors.
    Removes all the buckets from the list.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_clear (
    ipr_bucket_list_t * self            //  Reference to object
)
{
ipr_bucket_list_iter_t
    *item;
    int
        rc = 0;                         //  Return code

IPR_BUCKET_LIST_ASSERT_SANE (self);

//
item = (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.next;
while (item != &self->ipr_bucket_list_iter) {
    ipr_bucket_list_unhook (item);
    ipr_bucket_list_iter_destroy (&item);
    item = (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.next;
}
self->max_size = 0;
self->cur_size = 0;


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_load

    Type: Component method
    Accepts a ipr_bucket_list_t reference and returns zero in case of success,
    1 in case of errors.
    Reads an entire file into the bucket list.  Extends the list if it
    is not empty.  If you want to empty the bucket list first, use the
    ipr_bucket_list_clear method.  The file is read as binary data on
    systems that make such distinctions.  Returns an error if the file
    cannot be found.  If the chunking argument is zero, uses a suitable
    default.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_load (
    ipr_bucket_list_t * self,           //  Reference to object
    char * filename,                    //  File to read
    size_t chunking                     //  Size of buckets
)
{
FILE
    *file_stream;                   //  Handle to opened file
ipr_bucket_t
    *bucket;                        //  Allocated bucket
    int
        rc = 0;                         //  Return code

IPR_BUCKET_LIST_ASSERT_SANE (self);

//
assert (filename);
if (chunking == 0)
    chunking = IRP_BUCKET_DEFAULT_CHUNK;

file_stream = fopen (filename, "rb");
if (file_stream) {
    bucket = ipr_bucket_new (chunking);
    ipr_bucket_load (bucket, file_stream);
    while (bucket->cur_size) {
        self->max_size += bucket->max_size;
        self->cur_size += bucket->cur_size;
        ipr_bucket_list_queue (self, bucket);
        ipr_bucket_unlink (&bucket);
        bucket = ipr_bucket_new (chunking);
        ipr_bucket_load (bucket, file_stream);
    }
    ipr_bucket_unlink (&bucket);
    fclose (file_stream);
}
else
    rc = -1;                        //  Failed, cannot read file


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_save

    Type: Component method
    Accepts a ipr_bucket_list_t reference and returns zero in case of success,
    1 in case of errors.
    Writes a bucket list to a specified file.  The file is written as
    binary data on systems that make such distinctions.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_save (
    ipr_bucket_list_t * self,           //  Reference to object
    char * filename                     //  File to create
)
{
FILE
    *file_stream;                   //  Handle to opened file
ipr_bucket_list_iter_t
    *iterator;
    int
        rc = 0;                         //  Return code

IPR_BUCKET_LIST_ASSERT_SANE (self);

//
assert (filename);
file_stream = fopen (filename, "wb");
if (file_stream) {
    iterator = ipr_bucket_list_first (self);
    while (iterator) {
        ipr_bucket_save (iterator->item, file_stream);
        iterator = ipr_bucket_list_next (&iterator);
    }
    fclose (file_stream);
}
else
    rc = -1;                        //  Failed, cannot write file


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_list_selftest (
void)
{
ipr_bucket_list_t
    *bucket_list_1,
    *bucket_list_2;

//
bucket_list_1 = ipr_bucket_list_new ();
bucket_list_2 = ipr_bucket_list_new ();

ipr_bucket_list_load (bucket_list_1, "ipr_bucket_list.c", 1111);
ipr_bucket_list_save (bucket_list_1, "ipr_bucket_list.tmp");

ipr_bucket_list_load (bucket_list_2, "ipr_bucket_list.tmp", 3333);
assert (bucket_list_1->cur_size == bucket_list_2->cur_size);

ipr_file_delete ("ipr_bucket_list.tmp");
ipr_bucket_list_destroy (&bucket_list_1);
ipr_bucket_list_destroy (&bucket_list_2);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_iter_new

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_bucket_list_iter_t *
    ipr_bucket_list_iter_new (
    ipr_bucket_t * item                 //  Not documented
)
{
    ipr_bucket_list_iter_t *
        iter;                           //  Not documented

iter = icl_mem_cache_alloc (s_iter_cache);
iter->next = iter;
iter->prev = iter;
iter->head = NULL;
iter->item = ipr_bucket_link (item);


    return (iter);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_iter_object

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_bucket_t *
    ipr_bucket_list_iter_object (
    ipr_bucket_list_iter_t * iter       //  Not documented
)
{
    ipr_bucket_t *
        item;                           //  Not documented

item = ipr_bucket_link (iter->item);


    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_iter_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_list_iter_destroy (
    ipr_bucket_list_iter_t * ( * iter_p )  //  Not documented
)
{
    ipr_bucket_list_iter_t *
        iter = *iter_p;                 //  Dereferenced iter

if (iter) {
    ipr_bucket_list_remove (iter);
    ipr_bucket_unlink (&iter->item);
    icl_mem_free (iter);
    *iter_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_new

    Type: Component method
    Creates and initialises a new ipr_bucket_list_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_bucket_list_t *
    ipr_bucket_list_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_bucket_list_t *
        self = NULL;                    //  Object reference

    self = ipr_bucket_list_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_BUCKET_LIST_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_bucket_list_show_);
#endif

self->ipr_bucket_list_iter.next = &self->ipr_bucket_list_iter;
self->ipr_bucket_list_iter.prev = &self->ipr_bucket_list_iter;
self->ipr_bucket_list_iter.head = self;
self->ipr_bucket_list_iter.item = NULL;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_list_annihilate (
    ipr_bucket_list_t * ( * self_p )    //  Reference to object reference
)
{

ipr_bucket_list_iter_t
    *item;
    ipr_bucket_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_BUCKET_LIST_ASSERT_SANE (self);

item = (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.next;
while (item != &self->ipr_bucket_list_iter) {
    //  Unlink child without lock (we already have the lock)
    ipr_bucket_list_unhook (item);

    ipr_bucket_list_iter_destroy (&item);
    item = (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.next;
}

}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_isempty

    Type: Component method
    Accepts a ipr_bucket_list_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_isempty (
    ipr_bucket_list_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_BUCKET_LIST_ASSERT_SANE (self);

rc = (self->ipr_bucket_list_iter.next == &self->ipr_bucket_list_iter);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_push

    Type: Component method
    Accepts a ipr_bucket_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_push (
    ipr_bucket_list_t * self,           //  Reference to object
    ipr_bucket_t * item                 //  Not documented
)
{


ipr_bucket_list_iter_t
    *iter;
    int
        rc = 0;                         //  Return code



IPR_BUCKET_LIST_ASSERT_SANE (self);

iter = ipr_bucket_list_iter_new (item);
ipr_bucket_list_rehook (iter, &self->ipr_bucket_list_iter, (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.next);




    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

ipr_bucket_t *
    ipr_bucket_list_pop (
    ipr_bucket_list_t * self            //  The list
)
{


ipr_bucket_list_iter_t
    *iter = NULL;
    ipr_bucket_t *
        item = NULL;                    //  Not documented



IPR_BUCKET_LIST_ASSERT_SANE (self);

if (self->ipr_bucket_list_iter.next != &self->ipr_bucket_list_iter) {
    iter = ipr_bucket_list_unhook ((ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.next);
    item = iter->item;
    iter->item = NULL;          //  So don't destroy item
    ipr_bucket_list_iter_destroy (&iter);
}




    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_queue

    Type: Component method
    Accepts a ipr_bucket_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list. If the item
    was on another list, it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_queue (
    ipr_bucket_list_t * self,           //  Reference to object
    ipr_bucket_t * item                 //  Not documented
)
{


ipr_bucket_list_iter_t
    *iter;
    int
        rc = 0;                         //  Return code



IPR_BUCKET_LIST_ASSERT_SANE (self);

iter = ipr_bucket_list_iter_new (item);
ipr_bucket_list_rehook (iter, (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.prev, &self->ipr_bucket_list_iter);




    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_relink_before

    Type: Component method
    Accepts a ipr_bucket_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_relink_before (
    ipr_bucket_list_iter_t * item,      //  Not documented
    ipr_bucket_list_iter_t * where      //  Not documented
)
{


    int
        rc = 0;                         //  Return code
    ipr_bucket_list_t *
        self = (ipr_bucket_list_t *) where->head;  //  The list



IPR_BUCKET_LIST_ASSERT_SANE (self);

//  If head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->head) {
}
else
if (item->head != self) {
    icl_console_print ("E: ipr_bucket item inserted into multiple list containers");
    assert (item->head == self);
}
ipr_bucket_list_unhook (item);
ipr_bucket_list_rehook (item, (ipr_bucket_list_iter_t *) where->prev, where);




    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_remove

    Type: Component method
    Removes the ipr_bucket_list_iter item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    ipr_bucket_list_remove (
    ipr_bucket_list_iter_t * item       //  Not documented
)
{


    ipr_bucket_list_t *
        self = (ipr_bucket_list_t *) item->head;  //  The list
    int
        rc = 0;                         //  Not documented



if (self
&&  self == item->head) { //  Catch case where item moved
    ipr_bucket_list_unhook (item);
}



    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_first

    Type: Component method
    Returns the first ipr_bucket_list_iter item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

ipr_bucket_list_iter_t *
    ipr_bucket_list_first_ (
    ipr_bucket_list_t * self,           //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_bucket_list_iter_t *
        item = NULL;                    //  Not documented



IPR_BUCKET_LIST_ASSERT_SANE (self);

item = (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.next;
if (item == &self->ipr_bucket_list_iter)
    item = NULL;




    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_last

    Type: Component method
    Returns the last ipr_bucket_list_iter item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

ipr_bucket_list_iter_t *
    ipr_bucket_list_last_ (
    ipr_bucket_list_t * self,           //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_bucket_list_iter_t *
        item = NULL;                    //  Not documented



IPR_BUCKET_LIST_ASSERT_SANE (self);

item = (ipr_bucket_list_iter_t *) self->ipr_bucket_list_iter.prev;
if (item == &self->ipr_bucket_list_iter)
    item = NULL;




    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_next

    Type: Component method
    Returns the following item on the ipr_bucket_list_iter list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

ipr_bucket_list_iter_t *
    ipr_bucket_list_next_ (
    ipr_bucket_list_iter_t * ( * item_p ),   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_bucket_list_iter_t *
        item = *item_p;                 //  Dereferenced item
    ipr_bucket_list_iter_t *
        next = NULL;                    //  The item to return
    ipr_bucket_list_t *
        self = (ipr_bucket_list_t *) item->head;  //  The list



if (self
&&  self == item->head) { //  Catch case where item moved
    next = (ipr_bucket_list_iter_t *) item->next;
    if (next == &self->ipr_bucket_list_iter)
        next = NULL;
}



    return (next);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_prev

    Type: Component method
    Returns the preceding item on the ipr_bucket_list_iter list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

ipr_bucket_list_iter_t *
    ipr_bucket_list_prev_ (
    ipr_bucket_list_iter_t * ( * item_p ),   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_bucket_list_iter_t *
        item = *item_p;                 //  Dereferenced item
    ipr_bucket_list_iter_t *
        prev = NULL;                    //  The item to return
    ipr_bucket_list_t *
        self = (ipr_bucket_list_t *) item->head;  //  The list



if (self
&&  self == item->head) { //  Catch case where item moved
    prev = (ipr_bucket_list_iter_t *) item->prev;
    if (prev == &self->ipr_bucket_list_iter)
        prev = NULL;
}



    return (prev);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    ipr_bucket_list_rehook (
    ipr_bucket_list_iter_t * item,      //  Not documented
    ipr_bucket_list_iter_t * left,      //  Not documented
    ipr_bucket_list_iter_t * right      //  Not documented
)
{
ipr_bucket_list_iter_t
    *swap;

swap = (ipr_bucket_list_iter_t *) left->next;
left->next = item->next;
item->next = swap;

swap = (ipr_bucket_list_iter_t *) item->prev;
item->prev = right->prev;
right->prev = swap;

item->head = left->head;
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline ipr_bucket_list_iter_t *
    ipr_bucket_list_unhook (
    ipr_bucket_list_iter_t * item       //  Not documented
)
{

ipr_bucket_list_rehook (item, (ipr_bucket_list_iter_t *) item->prev, (ipr_bucket_list_iter_t *) item->next);
item->head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_list_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_bucket_list_t));
icl_system_register (ipr_bucket_list_cache_purge, ipr_bucket_list_cache_terminate);

s_iter_cache = icl_cache_get (sizeof (ipr_bucket_list_iter_t));
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_list_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

icl_mem_cache_purge (s_iter_cache);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_list_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_list_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_bucket_list_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_bucket_list file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_destroy

    Type: Component method
    Destroys a ipr_bucket_list_t object. Takes the address of a
    ipr_bucket_list_t reference (a pointer to a pointer) and nullifies the
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
    ipr_bucket_list_destroy_ (
    ipr_bucket_list_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_bucket_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_bucket_list_annihilate (self_p);
    ipr_bucket_list_free ((ipr_bucket_list_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_bucket_list_t *
    ipr_bucket_list_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_bucket_list_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_bucket_list_cache_initialise ();

self = (ipr_bucket_list_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_bucket_list_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_free

    Type: Component method
    Freess a ipr_bucket_list_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_list_free (
    ipr_bucket_list_t * self            //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_bucket_list_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_bucket_list_t));
        self->object_tag = IPR_BUCKET_LIST_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_bucket_list_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_list_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_bucket_list_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_bucket_list_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_bucket_list_new_in_scope_ (
    ipr_bucket_list_t * * self_p,       //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_bucket_list_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_bucket_list_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_bucket_list_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_bucket_list_component    = "ipr_bucket_list ";
char *EMBED__ipr_bucket_list_version      = "1.0 ";
char *EMBED__ipr_bucket_list_copyright    = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_bucket_list_filename     = "ipr_bucket_list.icl ";
char *EMBED__ipr_bucket_list_builddate    = "2011/03/01 ";
char *EMBED__ipr_bucket_list_version_end  = "VeRsIoNeNd:ipc";

