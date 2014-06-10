/*---------------------------------------------------------------------------
    smt_method_list.c - smt_method_list component

    This class implements the list container for smt_method
    Generated from smt_method_list.icl by icl_gen using GSL/4.
    
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
#include "smt_method_list.h"            //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_method_list_t

//  Shorthands for class methods

#define self_new            smt_method_list_new
#define self_annihilate     smt_method_list_annihilate
#define self_selftest       smt_method_list_selftest
#define self_isempty        smt_method_list_isempty
#define self_push           smt_method_list_push
#define self_pop            smt_method_list_pop
#define self_queue          smt_method_list_queue
#define self_relink_before  smt_method_list_relink_before
#define self_remove         smt_method_list_remove
#define self_first          smt_method_list_first
#define self_last           smt_method_list_last
#define self_next           smt_method_list_next
#define self_prev           smt_method_list_prev
#define self_rehook         smt_method_list_rehook
#define self_unhook         smt_method_list_unhook
#define self_terminate      smt_method_list_terminate
#define self_show           smt_method_list_show
#define self_destroy        smt_method_list_destroy
#define self_alloc          smt_method_list_alloc
#define self_free           smt_method_list_free
#define self_link           smt_method_list_link
#define self_unlink         smt_method_list_unlink
#define self_cache_initialise  smt_method_list_cache_initialise
#define self_cache_purge    smt_method_list_cache_purge
#define self_cache_terminate  smt_method_list_cache_terminate
#define self_show_animation  smt_method_list_show_animation
#define self_new_in_scope   smt_method_list_new_in_scope

#define smt_method_list_annihilate(self)  smt_method_list_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_method_list_annihilate_ (
smt_method_list_t * ( * self_p ),       //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static inline void
    smt_method_list_rehook (
smt_method_t * item,                    //  Not documented
smt_method_t * left,                    //  Not documented
smt_method_t * right                    //  Not documented
);

static inline smt_method_t *
    smt_method_list_unhook (
smt_method_t * item                     //  Not documented
);

#define smt_method_list_alloc()         smt_method_list_alloc_ (__FILE__, __LINE__)
static smt_method_list_t *
    smt_method_list_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_method_list_free(self)      smt_method_list_free_ (self, __FILE__, __LINE__)
static void
    smt_method_list_free_ (
smt_method_list_t * self,               //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_method_list_cache_initialise (
void);

static void
    smt_method_list_cache_purge (
void);

static void
    smt_method_list_cache_terminate (
void);

Bool
    smt_method_list_animating = TRUE;   //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_method_list_new

    Type: Component method
    Creates and initialises a new smt_method_list_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_method_list_t *
    smt_method_list_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_method_list_t *
        self = NULL;                    //  Object reference

    self = smt_method_list_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_METHOD_LIST_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_method_list_show_);
#endif

self->smt_method.list_next = &self->smt_method;
self->smt_method.list_prev = &self->smt_method;
self->smt_method.list_head = self;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_list_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    smt_method_list_annihilate_ (
    smt_method_list_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
    int
        history_last;
#endif

smt_method_t
    *item;
    smt_method_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = self->links;
#endif

    SMT_METHOD_LIST_ASSERT_SANE (self);

item = (smt_method_t *) self->smt_method.list_next;
while (item != &self->smt_method) {
    //  Unlink child without lock (we already have the lock)
    smt_method_list_unhook (item);

    smt_method_unlink (&item);
    item = (smt_method_t *) self->smt_method.list_next;
}

}
/*  -------------------------------------------------------------------------
    smt_method_list_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_list_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_method_list_isempty

    Type: Component method
    Accepts a smt_method_list_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    smt_method_list_isempty (
    smt_method_list_t * self            //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_METHOD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

rc = (self->smt_method.list_next == &self->smt_method);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_list_push

    Type: Component method
    Accepts a smt_method_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    smt_method_list_push (
    smt_method_list_t * self,           //  Reference to object
    smt_method_t * item                 //  Not documented
)
{


    int
        rc = 0;                         //  Return code



SMT_METHOD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    smt_method_link (item);
}
else
if (item->list_head != self) {
    icl_console_print ("E: smt_method item inserted into multiple list containers");
    assert (item->list_head == self);
}
smt_method_list_unhook (item);
smt_method_list_rehook (item, &self->smt_method, (smt_method_t *) self->smt_method.list_next);
}
else
    rc = -1;                        //  Return error on zombie object.




    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_list_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_list_pop (
    smt_method_list_t * self            //  The list
)
{


    smt_method_t *
        item = NULL;                    //  Not documented



SMT_METHOD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

if (self->smt_method.list_next == &self->smt_method)
    item = NULL;
else {
    item = smt_method_list_unhook ((smt_method_t *) self->smt_method.list_next);
}
}




    return (item);
}
/*  -------------------------------------------------------------------------
    smt_method_list_queue

    Type: Component method
    Accepts a smt_method_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_method_list_queue (
    smt_method_list_t * self,           //  Reference to object
    smt_method_t * item                 //  Not documented
)
{


    int
        rc = 0;                         //  Return code



SMT_METHOD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    smt_method_link (item);
}
else
if (item->list_head != self) {
    icl_console_print ("E: smt_method item inserted into multiple list containers");
    assert (item->list_head == self);
}
smt_method_list_unhook (item);
smt_method_list_rehook (item, (smt_method_t *) self->smt_method.list_prev, &self->smt_method);
}
else
    rc = -1;                        //  Return error on zombie object.




    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_list_relink_before

    Type: Component method
    Accepts a smt_method_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_method_list_relink_before (
    smt_method_t * item,                //  Not documented
    smt_method_t * where                //  Not documented
)
{


    int
        rc = 0;                         //  Return code
    smt_method_list_t *
        self = (smt_method_list_t *) where->list_head;  //  The list



SMT_METHOD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    smt_method_link (item);
}
else
if (item->list_head != self) {
    icl_console_print ("E: smt_method item inserted into multiple list containers");
    assert (item->list_head == self);
}
smt_method_list_unhook (item);
smt_method_list_rehook (item, (smt_method_t *) where->list_prev, where);
}
else
    rc = -1;                        //  Return error on zombie object.




    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_list_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    smt_method_list_remove (
    smt_method_t * item                 //  Not documented
)
{


    smt_method_list_t *
        self = (smt_method_list_t *) item->list_head;  //  The list
    int
        rc = 0;                         //  Not documented



if (self
&&  self == item->list_head) { //       Catch case where item moved
    smt_method_list_unhook (item);
    smt_method_unlink (&item);
}



    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_list_first

    Type: Component method
    Returns the first smt_method item on the list.    If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_list_first_ (
    smt_method_list_t * self,           //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_method_t *
        item = NULL;                    //  Not documented



SMT_METHOD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_method_t *) self->smt_method.list_next;
if (item == &self->smt_method)
    item = NULL;
else
    smt_method_link (item);

}




    return (item);
}
/*  -------------------------------------------------------------------------
    smt_method_list_last

    Type: Component method
    Returns the last smt_method item on the list.    If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_list_last_ (
    smt_method_list_t * self,           //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_method_t *
        item = NULL;                    //  Not documented



SMT_METHOD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_method_t *) self->smt_method.list_prev;
if (item == &self->smt_method)
    item = NULL;
else
    smt_method_link (item);

}




    return (item);
}
/*  -------------------------------------------------------------------------
    smt_method_list_next

    Type: Component method
    Returns the following item on the smt_method list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_list_next_ (
    smt_method_t * ( * item_p ),        //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_method_t *
        item = *item_p;                 //  Dereferenced item
    smt_method_t *
        next = NULL;                    //  The item to return
    smt_method_list_t *
        self = (smt_method_list_t *) item->list_head;  //  The list



if (self
&&  self == item->list_head) { //       Catch case where item moved
    next = (smt_method_t *) item->list_next;
    if (next == &self->smt_method)
        next = NULL;
    else
        smt_method_link (next);

}
smt_method_unlink (item_p);




    return (next);
}
/*  -------------------------------------------------------------------------
    smt_method_list_prev

    Type: Component method
    Returns the preceding item on the smt_method list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_list_prev_ (
    smt_method_t * ( * item_p ),        //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_method_t *
        item = *item_p;                 //  Dereferenced item
    smt_method_t *
        prev = NULL;                    //  The item to return
    smt_method_list_t *
        self = (smt_method_list_t *) item->list_head;  //  The list



if (self
&&  self == item->list_head) { //       Catch case where item moved
    prev = (smt_method_t *) item->list_prev;
    if (prev == &self->smt_method)
        prev = NULL;
    else
        smt_method_link (prev);

}
smt_method_unlink (item_p);




    return (prev);
}
/*  -------------------------------------------------------------------------
    smt_method_list_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    smt_method_list_rehook (
    smt_method_t * item,                //  Not documented
    smt_method_t * left,                //  Not documented
    smt_method_t * right                //  Not documented
)
{
smt_method_t
    *swap;

swap = (smt_method_t *) left->list_next;
left->list_next = item->list_next;
item->list_next = swap;

swap = (smt_method_t *) item->list_prev;
item->list_prev = right->list_prev;
right->list_prev = swap;

item->list_head = left->list_head;
}
/*  -------------------------------------------------------------------------
    smt_method_list_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline smt_method_t *
    smt_method_list_unhook (
    smt_method_t * item                 //  Not documented
)
{

smt_method_list_rehook (item, (smt_method_t *) item->list_prev, (smt_method_t *) item->list_next);
item->list_head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    smt_method_list_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_list_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_method_list_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_list_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_method_list_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_method_list zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
    if (self->history_last > SMT_METHOD_LIST_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_METHOD_LIST_HISTORY_LENGTH;
        self->history_last %= SMT_METHOD_LIST_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_METHOD_LIST_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_method_list>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_method_list_destroy

    Type: Component method
    Destroys a smt_method_list_t object. Takes the address of a
    smt_method_list_t reference (a pointer to a pointer) and nullifies the
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
    smt_method_list_destroy_ (
    smt_method_list_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_method_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_method_list_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_method_list", "E: missing link on smt_method_list object");
        smt_method_list_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_method_list_free_ ((smt_method_list_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_method_list_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_method_list_t *
    smt_method_list_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_method_list_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_method_list_cache_initialise ();

self = (smt_method_list_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_method_list_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_list_free

    Type: Component method
    Freess a smt_method_list_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_method_list_free_ (
    smt_method_list_t * self,           //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (smt_method_list_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_method_list_t));
        self->object_tag = SMT_METHOD_LIST_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_method_list_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_method_list_t *
    smt_method_list_link_ (
    smt_method_list_t * self,           //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
    int
        history_last;
#endif

    if (self) {
        SMT_METHOD_LIST_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_list_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_method_list_unlink_ (
    smt_method_list_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
    int
        history_last;
#endif

    smt_method_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        SMT_METHOD_LIST_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_method_list", "E: missing link on smt_method_list object");
            smt_method_list_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_LIST)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_METHOD_LIST_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_method_list_annihilate_ (self_p, file, line);
        smt_method_list_free_ ((smt_method_list_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_method_list_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_method_list_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_method_list_t));
icl_system_register (smt_method_list_cache_purge, smt_method_list_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_method_list_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_method_list_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_method_list_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_method_list_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_method_list_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_method_list_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_method_list_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_method_list_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_list_new_in_scope_ (
    smt_method_list_t * * self_p,       //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_method_list_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_method_list_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_method_list_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_method_list_component    = "smt_method_list ";
char *EMBED__smt_method_list_version      = "1.0 ";
char *EMBED__smt_method_list_copyright    = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_method_list_filename     = "smt_method_list.icl ";
char *EMBED__smt_method_list_builddate    = "2011/03/01 ";
char *EMBED__smt_method_list_version_end  = "VeRsIoNeNd:ipc";

