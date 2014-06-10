/*---------------------------------------------------------------------------
    ipr_token_list.c - ipr_token_list component

    This class implements the list container for ipr_token
    Generated from ipr_token_list.icl by icl_gen using GSL/4.
    
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
#include "ipr_token_list.h"             //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_token_list_t

//  Shorthands for class methods

#define self_cat            ipr_token_list_cat
#define self_merge          ipr_token_list_merge
#define self_selftest       ipr_token_list_selftest
#define self_new            ipr_token_list_new
#define self_annihilate     ipr_token_list_annihilate
#define self_isempty        ipr_token_list_isempty
#define self_push           ipr_token_list_push
#define self_pop            ipr_token_list_pop
#define self_queue          ipr_token_list_queue
#define self_relink_before  ipr_token_list_relink_before
#define self_remove         ipr_token_list_remove
#define self_first          ipr_token_list_first
#define self_last           ipr_token_list_last
#define self_next           ipr_token_list_next
#define self_prev           ipr_token_list_prev
#define self_count          ipr_token_list_count
#define self_rehook         ipr_token_list_rehook
#define self_unhook         ipr_token_list_unhook
#define self_terminate      ipr_token_list_terminate
#define self_show           ipr_token_list_show
#define self_destroy        ipr_token_list_destroy
#define self_alloc          ipr_token_list_alloc
#define self_free           ipr_token_list_free
#define self_show_animation  ipr_token_list_show_animation
#define self_new_in_scope   ipr_token_list_new_in_scope

static void
    ipr_token_list_annihilate (
ipr_token_list_t * ( * self_p )         //  Reference to object reference
);

static inline void
    ipr_token_list_rehook (
ipr_token_t * item,                     //  Not documented
ipr_token_t * left,                     //  Not documented
ipr_token_t * right                     //  Not documented
);

static inline ipr_token_t *
    ipr_token_list_unhook (
ipr_token_t * item                      //  Not documented
);

#define ipr_token_list_alloc()          ipr_token_list_alloc_ (__FILE__, __LINE__)
static ipr_token_list_t *
    ipr_token_list_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_token_list_free (
ipr_token_list_t * self                 //  Object reference
);

Bool
    ipr_token_list_animating = TRUE;    //  Animation enabled by default

/*  -------------------------------------------------------------------------
    ipr_token_list_cat

    Type: Component method
    Accepts a ipr_token_list_t reference and returns zero in case of success,
    1 in case of errors.
    Concatenates one token list onto another.
    -------------------------------------------------------------------------
 */

int
    ipr_token_list_cat (
    ipr_token_list_t * self,            //  Reference to object
    ipr_token_list_t * source           //  List to copy from
)
{
ipr_token_t
    *token,
    *copy;
    int
        rc = 0;                         //  Return code

IPR_TOKEN_LIST_ASSERT_SANE (self);

//
token = ipr_token_list_first (source);
while (token) {
    copy = ipr_token_new ();
    icl_shortstr_cpy (copy->value, token->value);
    ipr_token_list_queue (self, copy);
    ipr_token_unlink (&copy);
    token = ipr_token_list_next (&token);
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_merge

    Type: Component method
    Accepts a ipr_token_list_t reference and returns zero in case of success,
    1 in case of errors.
    Merges one token list into another, appending only tokens that
    are not already in the first list.  Note: this function will be
    slow for large lists, since it does sequential scans.
    -------------------------------------------------------------------------
 */

int
    ipr_token_list_merge (
    ipr_token_list_t * self,            //  Reference to object
    ipr_token_list_t * source           //  List to copy from
)
{
ipr_token_t
    *token,
    *copy;
    int
        rc = 0;                         //  Return code

IPR_TOKEN_LIST_ASSERT_SANE (self);

//
token = ipr_token_list_first (source);
while (token) {
    copy = ipr_token_list_first (self);
    while (copy) {
        if (streq (token->value, copy->value))
            break;                  //  Token already exists in list
        copy = ipr_token_list_next (&copy);
    }
    //  Append only if the token was not already there
    if (!copy) {
        copy = ipr_token_new ();
        icl_shortstr_cpy (copy->value, token->value);
        ipr_token_list_queue (self, copy);
    }
    ipr_token_unlink (&copy);
    token = ipr_token_list_next (&token);
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_list_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    ipr_token_list_new

    Type: Component method
    Creates and initialises a new ipr_token_list_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_token_list_t *
    ipr_token_list_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_token_list_t *
        self = NULL;                    //  Object reference

    self = ipr_token_list_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_TOKEN_LIST_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_token_list_show_);
#endif

self->ipr_token.list_next = &self->ipr_token;
self->ipr_token.list_prev = &self->ipr_token;
self->ipr_token.list_head = self;
self->count = 0;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    ipr_token_list_annihilate (
    ipr_token_list_t * ( * self_p )     //  Reference to object reference
)
{

ipr_token_t
    *item;
    ipr_token_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_TOKEN_LIST_ASSERT_SANE (self);

item = (ipr_token_t *) self->ipr_token.list_next;
while (item != &self->ipr_token) {
    //  Unlink child without lock (we already have the lock)
    ipr_token_list_unhook (item);

    ipr_token_unlink (&item);
    item = (ipr_token_t *) self->ipr_token.list_next;
}

}
/*  -------------------------------------------------------------------------
    ipr_token_list_isempty

    Type: Component method
    Accepts a ipr_token_list_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    ipr_token_list_isempty (
    ipr_token_list_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_TOKEN_LIST_ASSERT_SANE (self);

rc = (self->ipr_token.list_next == &self->ipr_token);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_push

    Type: Component method
    Accepts a ipr_token_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    ipr_token_list_push (
    ipr_token_list_t * self,            //  Reference to object
    ipr_token_t * item                  //  Not documented
)
{


    int
        rc = 0;                         //  Return code



IPR_TOKEN_LIST_ASSERT_SANE (self);

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    ipr_token_link (item);
    icl_atomic_inc32 (&self->count);
}
else
if (item->list_head != self) {
    icl_console_print ("E: ipr_token item inserted into multiple list containers");
    assert (item->list_head == self);
}
ipr_token_list_unhook (item);
ipr_token_list_rehook (item, &self->ipr_token, (ipr_token_t *) self->ipr_token.list_next);




    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_token_list_pop (
    ipr_token_list_t * self             //  The list
)
{


    ipr_token_t *
        item = NULL;                    //  Not documented



IPR_TOKEN_LIST_ASSERT_SANE (self);

if (self->ipr_token.list_next == &self->ipr_token)
    item = NULL;
else {
    item = ipr_token_list_unhook ((ipr_token_t *) self->ipr_token.list_next);
    icl_atomic_dec32 (&self->count);
}




    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_queue

    Type: Component method
    Accepts a ipr_token_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    ipr_token_list_queue (
    ipr_token_list_t * self,            //  Reference to object
    ipr_token_t * item                  //  Not documented
)
{


    int
        rc = 0;                         //  Return code



IPR_TOKEN_LIST_ASSERT_SANE (self);

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    ipr_token_link (item);
    icl_atomic_inc32 (&self->count);
}
else
if (item->list_head != self) {
    icl_console_print ("E: ipr_token item inserted into multiple list containers");
    assert (item->list_head == self);
}
ipr_token_list_unhook (item);
ipr_token_list_rehook (item, (ipr_token_t *) self->ipr_token.list_prev, &self->ipr_token);




    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_relink_before

    Type: Component method
    Accepts a ipr_token_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    ipr_token_list_relink_before (
    ipr_token_t * item,                 //  Not documented
    ipr_token_t * where                 //  Not documented
)
{


    int
        rc = 0;                         //  Return code
    ipr_token_list_t *
        self = (ipr_token_list_t *) where->list_head;  //  The list



IPR_TOKEN_LIST_ASSERT_SANE (self);

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    ipr_token_link (item);
    icl_atomic_inc32 (&self->count);
}
else
if (item->list_head != self) {
    icl_console_print ("E: ipr_token item inserted into multiple list containers");
    assert (item->list_head == self);
}
ipr_token_list_unhook (item);
ipr_token_list_rehook (item, (ipr_token_t *) where->list_prev, where);




    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    ipr_token_list_remove (
    ipr_token_t * item                  //  Not documented
)
{


    ipr_token_list_t *
        self = (ipr_token_list_t *) item->list_head;  //  The list
    int
        rc = 0;                         //  Not documented



if (self
&&  self == item->list_head) { //       Catch case where item moved
    ipr_token_list_unhook (item);
    ipr_token_unlink (&item);
    icl_atomic_dec32 (&self->count);
}



    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_first

    Type: Component method
    Returns the first ipr_token item on the list.     If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_token_list_first_ (
    ipr_token_list_t * self,            //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_token_t *
        item = NULL;                    //  Not documented



IPR_TOKEN_LIST_ASSERT_SANE (self);

item = (ipr_token_t *) self->ipr_token.list_next;
if (item == &self->ipr_token)
    item = NULL;
else
    ipr_token_link (item);





    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_last

    Type: Component method
    Returns the last ipr_token item on the list.     If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_token_list_last_ (
    ipr_token_list_t * self,            //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_token_t *
        item = NULL;                    //  Not documented



IPR_TOKEN_LIST_ASSERT_SANE (self);

item = (ipr_token_t *) self->ipr_token.list_prev;
if (item == &self->ipr_token)
    item = NULL;
else
    ipr_token_link (item);





    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_next

    Type: Component method
    Returns the following item on the ipr_token list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_token_list_next_ (
    ipr_token_t * ( * item_p ),         //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_token_t *
        item = *item_p;                 //  Dereferenced item
    ipr_token_t *
        next = NULL;                    //  The item to return
    ipr_token_list_t *
        self = (ipr_token_list_t *) item->list_head;  //  The list



if (self
&&  self == item->list_head) { //       Catch case where item moved
    next = (ipr_token_t *) item->list_next;
    if (next == &self->ipr_token)
        next = NULL;
    else
        ipr_token_link (next);

}
ipr_token_unlink (item_p);




    return (next);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_prev

    Type: Component method
    Returns the preceding item on the ipr_token list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_token_list_prev_ (
    ipr_token_t * ( * item_p ),         //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    ipr_token_t *
        item = *item_p;                 //  Dereferenced item
    ipr_token_t *
        prev = NULL;                    //  The item to return
    ipr_token_list_t *
        self = (ipr_token_list_t *) item->list_head;  //  The list



if (self
&&  self == item->list_head) { //       Catch case where item moved
    prev = (ipr_token_t *) item->list_prev;
    if (prev == &self->ipr_token)
        prev = NULL;
    else
        ipr_token_link (prev);

}
ipr_token_unlink (item_p);




    return (prev);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_count

    Type: Component method
    Return the count of the number of objects in the list.
    -------------------------------------------------------------------------
 */

qbyte
    ipr_token_list_count (
    ipr_token_list_t * self             //  The list
)
{
    qbyte
        count;                          //  Not documented

IPR_TOKEN_LIST_ASSERT_SANE (self);
count = icl_atomic_get32 (&self->count);


    return (count);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    ipr_token_list_rehook (
    ipr_token_t * item,                 //  Not documented
    ipr_token_t * left,                 //  Not documented
    ipr_token_t * right                 //  Not documented
)
{
ipr_token_t
    *swap;

swap = (ipr_token_t *) left->list_next;
left->list_next = item->list_next;
item->list_next = swap;

swap = (ipr_token_t *) item->list_prev;
item->list_prev = right->list_prev;
right->list_prev = swap;

item->list_head = left->list_head;
}
/*  -------------------------------------------------------------------------
    ipr_token_list_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline ipr_token_t *
    ipr_token_list_unhook (
    ipr_token_t * item                  //  Not documented
)
{

ipr_token_list_rehook (item, (ipr_token_t *) item->list_prev, (ipr_token_t *) item->list_next);
item->list_head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_list_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_token_list_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_list_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_token_list_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_token_list file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_token_list_destroy

    Type: Component method
    Destroys a ipr_token_list_t object. Takes the address of a
    ipr_token_list_t reference (a pointer to a pointer) and nullifies the
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
    ipr_token_list_destroy_ (
    ipr_token_list_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_token_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_token_list_annihilate (self_p);
    ipr_token_list_free ((ipr_token_list_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_token_list_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_token_list_t *
    ipr_token_list_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_token_list_t *
        self = NULL;                    //  Object reference

self = (ipr_token_list_t *) icl_mem_alloc_ (sizeof (ipr_token_list_t), file, line);
if (self)
    memset (self, 0, sizeof (ipr_token_list_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_token_list_free

    Type: Component method
    Freess a ipr_token_list_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_token_list_free (
    ipr_token_list_t * self             //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_token_list_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_token_list_t));
        self->object_tag = IPR_TOKEN_LIST_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_token_list_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_token_list_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_token_list_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_token_list_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_list_new_in_scope_ (
    ipr_token_list_t * * self_p,        //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_token_list_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_token_list_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_token_list_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_token_list_component     = "ipr_token_list ";
char *EMBED__ipr_token_list_version       = "1.0 ";
char *EMBED__ipr_token_list_copyright     = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_token_list_filename      = "ipr_token_list.icl ";
char *EMBED__ipr_token_list_builddate     = "2011/03/01 ";
char *EMBED__ipr_token_list_version_end   = "VeRsIoNeNd:ipc";

