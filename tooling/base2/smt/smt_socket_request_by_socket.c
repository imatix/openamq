/*---------------------------------------------------------------------------
    smt_socket_request_by_socket.c - smt_socket_request_by_socket component

    This class implements the list container for smt_socket_request
    Generated from smt_socket_request_by_socket.icl by icl_gen using GSL/4.
    
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
#include "smt_socket_request_by_socket.h"  //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_socket_request_by_socket_t

//  Shorthands for class methods

#define self_new            smt_socket_request_by_socket_new
#define self_annihilate     smt_socket_request_by_socket_annihilate
#define self_selftest       smt_socket_request_by_socket_selftest
#define self_isempty        smt_socket_request_by_socket_isempty
#define self_push           smt_socket_request_by_socket_push
#define self_pop            smt_socket_request_by_socket_pop
#define self_queue          smt_socket_request_by_socket_queue
#define self_relink_before  smt_socket_request_by_socket_relink_before
#define self_remove         smt_socket_request_by_socket_remove
#define self_first          smt_socket_request_by_socket_first
#define self_last           smt_socket_request_by_socket_last
#define self_next           smt_socket_request_by_socket_next
#define self_prev           smt_socket_request_by_socket_prev
#define self_rehook         smt_socket_request_by_socket_rehook
#define self_unhook         smt_socket_request_by_socket_unhook
#define self_terminate      smt_socket_request_by_socket_terminate
#define self_show           smt_socket_request_by_socket_show
#define self_destroy        smt_socket_request_by_socket_destroy
#define self_alloc          smt_socket_request_by_socket_alloc
#define self_free           smt_socket_request_by_socket_free
#define self_cache_initialise  smt_socket_request_by_socket_cache_initialise
#define self_cache_purge    smt_socket_request_by_socket_cache_purge
#define self_cache_terminate  smt_socket_request_by_socket_cache_terminate
#define self_show_animation  smt_socket_request_by_socket_show_animation
#define self_new_in_scope   smt_socket_request_by_socket_new_in_scope

static void
    smt_socket_request_by_socket_annihilate (
smt_socket_request_by_socket_t * ( * self_p )  //  Reference to object reference
);

static inline void
    smt_socket_request_by_socket_rehook (
smt_socket_request_t * item,            //  Not documented
smt_socket_request_t * left,            //  Not documented
smt_socket_request_t * right            //  Not documented
);

static inline smt_socket_request_t *
    smt_socket_request_by_socket_unhook (
smt_socket_request_t * item             //  Not documented
);

#define smt_socket_request_by_socket_alloc()  smt_socket_request_by_socket_alloc_ (__FILE__, __LINE__)
static smt_socket_request_by_socket_t *
    smt_socket_request_by_socket_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_socket_request_by_socket_free (
smt_socket_request_by_socket_t * self   //  Object reference
);

static void
    smt_socket_request_by_socket_cache_initialise (
void);

static void
    smt_socket_request_by_socket_cache_purge (
void);

static void
    smt_socket_request_by_socket_cache_terminate (
void);

Bool
    smt_socket_request_by_socket_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_new

    Type: Component method
    Creates and initialises a new smt_socket_request_by_socket_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_socket_request_by_socket_t *
    smt_socket_request_by_socket_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_socket_request_by_socket_t *
        self = NULL;                    //  Object reference

    self = smt_socket_request_by_socket_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_SOCKET_REQUEST_BY_SOCKET_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_socket_request_by_socket_show_);
#endif

self->smt_socket_request.by_socket_next = &self->smt_socket_request;
self->smt_socket_request.by_socket_prev = &self->smt_socket_request;
self->smt_socket_request.by_socket_head = self;

}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_by_socket_annihilate (
    smt_socket_request_by_socket_t * ( * self_p )  //  Reference to object reference
)
{

smt_socket_request_t
    *item;
    smt_socket_request_by_socket_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

item = (smt_socket_request_t *) self->smt_socket_request.by_socket_next;
while (item != &self->smt_socket_request) {
    //  Unlink child without lock (we already have the lock)
    smt_socket_request_by_socket_unhook (item);

    smt_socket_request_destroy (&item);
    item = (smt_socket_request_t *) self->smt_socket_request.by_socket_next;
}

}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_by_socket_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_isempty

    Type: Component method
    Accepts a smt_socket_request_by_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    smt_socket_request_by_socket_isempty (
    smt_socket_request_by_socket_t * self  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

rc = (self->smt_socket_request.by_socket_next == &self->smt_socket_request);


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_push

    Type: Component method
    Accepts a smt_socket_request_by_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    smt_socket_request_by_socket_push (
    smt_socket_request_by_socket_t * self,   //  Reference to object
    smt_socket_request_t * item         //  Not documented
)
{


    int
        rc = 0;                         //  Return code



SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

//  If by_socket_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_socket_head) {
}
else
if (item->by_socket_head != self) {
    icl_console_print ("E: smt_socket_request item inserted into multiple by_socket containers");
    assert (item->by_socket_head == self);
}
smt_socket_request_by_socket_unhook (item);
smt_socket_request_by_socket_rehook (item, &self->smt_socket_request, (smt_socket_request_t *) self->smt_socket_request.by_socket_next);




    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_by_socket_pop (
    smt_socket_request_by_socket_t * self  //  The list
)
{


    smt_socket_request_t *
        item = NULL;                    //  Not documented



SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

if (self->smt_socket_request.by_socket_next == &self->smt_socket_request)
    item = NULL;
else {
    item = smt_socket_request_by_socket_unhook ((smt_socket_request_t *) self->smt_socket_request.by_socket_next);
}




    return (item);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_queue

    Type: Component method
    Accepts a smt_socket_request_by_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_socket_request_by_socket_queue (
    smt_socket_request_by_socket_t * self,   //  Reference to object
    smt_socket_request_t * item         //  Not documented
)
{


    int
        rc = 0;                         //  Return code



SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

//  If by_socket_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_socket_head) {
}
else
if (item->by_socket_head != self) {
    icl_console_print ("E: smt_socket_request item inserted into multiple by_socket containers");
    assert (item->by_socket_head == self);
}
smt_socket_request_by_socket_unhook (item);
smt_socket_request_by_socket_rehook (item, (smt_socket_request_t *) self->smt_socket_request.by_socket_prev, &self->smt_socket_request);




    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_relink_before

    Type: Component method
    Accepts a smt_socket_request_by_socket_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_socket_request_by_socket_relink_before (
    smt_socket_request_t * item,        //  Not documented
    smt_socket_request_t * where        //  Not documented
)
{


    int
        rc = 0;                         //  Return code
    smt_socket_request_by_socket_t *
        self = (smt_socket_request_by_socket_t *) where->by_socket_head;  //  The list



SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

//  If by_socket_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_socket_head) {
}
else
if (item->by_socket_head != self) {
    icl_console_print ("E: smt_socket_request item inserted into multiple by_socket containers");
    assert (item->by_socket_head == self);
}
smt_socket_request_by_socket_unhook (item);
smt_socket_request_by_socket_rehook (item, (smt_socket_request_t *) where->by_socket_prev, where);




    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    smt_socket_request_by_socket_remove (
    smt_socket_request_t * item         //  Not documented
)
{


    smt_socket_request_by_socket_t *
        self = (smt_socket_request_by_socket_t *) item->by_socket_head;  //  The list
    int
        rc = 0;                         //  Not documented



if (self
&&  self == item->by_socket_head) { //  Catch case where item moved
    smt_socket_request_by_socket_unhook (item);
}



    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_first

    Type: Component method
    Returns the first smt_socket_request item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_by_socket_first_ (
    smt_socket_request_by_socket_t * self,   //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_socket_request_t *
        item = NULL;                    //  Not documented



SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

item = (smt_socket_request_t *) self->smt_socket_request.by_socket_next;
if (item == &self->smt_socket_request)
    item = NULL;





    return (item);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_last

    Type: Component method
    Returns the last smt_socket_request item on the list.  If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_by_socket_last_ (
    smt_socket_request_by_socket_t * self,   //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_socket_request_t *
        item = NULL;                    //  Not documented



SMT_SOCKET_REQUEST_BY_SOCKET_ASSERT_SANE (self);

item = (smt_socket_request_t *) self->smt_socket_request.by_socket_prev;
if (item == &self->smt_socket_request)
    item = NULL;





    return (item);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_next

    Type: Component method
    Returns the following item on the smt_socket_request list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_by_socket_next_ (
    smt_socket_request_t * ( * item_p ),   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_socket_request_t *
        item = *item_p;                 //  Dereferenced item
    smt_socket_request_t *
        next = NULL;                    //  The item to return
    smt_socket_request_by_socket_t *
        self = (smt_socket_request_by_socket_t *) item->by_socket_head;  //  The list



if (self
&&  self == item->by_socket_head) { //  Catch case where item moved
    next = (smt_socket_request_t *) item->by_socket_next;
    if (next == &self->smt_socket_request)
        next = NULL;

}




    return (next);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_prev

    Type: Component method
    Returns the preceding item on the smt_socket_request list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_socket_request_t *
    smt_socket_request_by_socket_prev_ (
    smt_socket_request_t * ( * item_p ),   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_socket_request_t *
        item = *item_p;                 //  Dereferenced item
    smt_socket_request_t *
        prev = NULL;                    //  The item to return
    smt_socket_request_by_socket_t *
        self = (smt_socket_request_by_socket_t *) item->by_socket_head;  //  The list



if (self
&&  self == item->by_socket_head) { //  Catch case where item moved
    prev = (smt_socket_request_t *) item->by_socket_prev;
    if (prev == &self->smt_socket_request)
        prev = NULL;

}




    return (prev);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    smt_socket_request_by_socket_rehook (
    smt_socket_request_t * item,        //  Not documented
    smt_socket_request_t * left,        //  Not documented
    smt_socket_request_t * right        //  Not documented
)
{
smt_socket_request_t
    *swap;

swap = (smt_socket_request_t *) left->by_socket_next;
left->by_socket_next = item->by_socket_next;
item->by_socket_next = swap;

swap = (smt_socket_request_t *) item->by_socket_prev;
item->by_socket_prev = right->by_socket_prev;
right->by_socket_prev = swap;

item->by_socket_head = left->by_socket_head;
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline smt_socket_request_t *
    smt_socket_request_by_socket_unhook (
    smt_socket_request_t * item         //  Not documented
)
{

smt_socket_request_by_socket_rehook (item, (smt_socket_request_t *) item->by_socket_prev, (smt_socket_request_t *) item->by_socket_next);
item->by_socket_head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_by_socket_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_by_socket_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
smt_socket_request_by_socket_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <smt_socket_request_by_socket file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_destroy

    Type: Component method
    Destroys a smt_socket_request_by_socket_t object. Takes the address of a
    smt_socket_request_by_socket_t reference (a pointer to a pointer) and nullifies the
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
    smt_socket_request_by_socket_destroy_ (
    smt_socket_request_by_socket_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_socket_request_by_socket_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    smt_socket_request_by_socket_annihilate (self_p);
    smt_socket_request_by_socket_free ((smt_socket_request_by_socket_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_socket_request_by_socket_t *
    smt_socket_request_by_socket_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_socket_request_by_socket_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_socket_request_by_socket_cache_initialise ();

self = (smt_socket_request_by_socket_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_socket_request_by_socket_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_free

    Type: Component method
    Freess a smt_socket_request_by_socket_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_by_socket_free (
    smt_socket_request_by_socket_t * self  //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (smt_socket_request_by_socket_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_socket_request_by_socket_t));
        self->object_tag = SMT_SOCKET_REQUEST_BY_SOCKET_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_by_socket_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_socket_request_by_socket_t));
icl_system_register (smt_socket_request_by_socket_cache_purge, smt_socket_request_by_socket_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_by_socket_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_socket_request_by_socket_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_by_socket_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_socket_request_by_socket_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_socket_request_by_socket_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_socket_request_by_socket_new_in_scope_ (
    smt_socket_request_by_socket_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_socket_request_by_socket_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_socket_request_by_socket_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_socket_request_by_socket_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_socket_request_by_socket_component  = "smt_socket_request_by_socket ";
char *EMBED__smt_socket_request_by_socket_version  = "1.0 ";
char *EMBED__smt_socket_request_by_socket_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_socket_request_by_socket_filename  = "smt_socket_request_by_socket.icl ";
char *EMBED__smt_socket_request_by_socket_builddate  = "2011/03/01 ";
char *EMBED__smt_socket_request_by_socket_version_end  = "VeRsIoNeNd:ipc";

