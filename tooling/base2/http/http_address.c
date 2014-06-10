/*---------------------------------------------------------------------------
    http_address.c - http_address component

    Stores data about a known IP address, used for DoS detection and other
    administration.  The address holds a looseref list of http_agent threads.
    Generated from http_address.icl by icl_gen using GSL/4.
    
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
#include "http_address.h"               //  Definitions for our class

//  Shorthand for class type

#define self_t              http_address_t

//  Shorthands for class methods

#define self_new            http_address_new
#define self_annihilate     http_address_annihilate
#define self_thread_open    http_address_thread_open
#define self_thread_close   http_address_thread_close
#define self_selftest       http_address_selftest
#define self_remove_from_all_containers  http_address_remove_from_all_containers
#define self_show           http_address_show
#define self_terminate      http_address_terminate
#define self_destroy        http_address_destroy
#define self_alloc          http_address_alloc
#define self_free           http_address_free
#define self_lock           http_address_lock
#define self_unlock         http_address_unlock
#define self_cache_initialise  http_address_cache_initialise
#define self_cache_purge    http_address_cache_purge
#define self_cache_terminate  http_address_cache_terminate
#define self_show_animation  http_address_show_animation
#define self_new_in_scope   http_address_new_in_scope

static void
    http_address_annihilate (
http_address_t * ( * self_p )           //  Reference to object reference
);

#define http_address_alloc()            http_address_alloc_ (__FILE__, __LINE__)
static http_address_t *
    http_address_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    http_address_free (
http_address_t * self                   //  Object reference
);

static void
    http_address_cache_initialise (
void);

static void
    http_address_cache_purge (
void);

static void
    http_address_cache_terminate (
void);

Bool
    http_address_animating = TRUE;      //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    http_address_new

    Type: Component method
    Creates and initialises a new http_address_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    -------------------------------------------------------------------------
 */

http_address_t *
    http_address_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_address_table_t * table,       //  Table to insert into
    char * key                          //  Hash key
)
{
    http_address_t *
        self = NULL;                    //  Object reference

    self = http_address_alloc_ (file, line);
    if (self) {
        self->object_tag   = HTTP_ADDRESS_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->mutex = icl_mutex_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, http_address_show_);
#endif

self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->thread_list = ipr_looseref_list_new ();
self->thread_quota = http_config_address_quota (http_config);
//todo: if address is on whitelist then set to infinity
if (table && self && http_address_table_insert (table, key, self))
    http_address_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    http_address_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_address_annihilate (
    http_address_t * ( * self_p )       //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif

smt_thread_t
    *thread;
    http_address_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    HTTP_ADDRESS_ASSERT_SANE (self);
    http_address_remove_from_all_containers (self);
#if (defined (BASE_THREADSAFE))
    mutex = self->mutex;
    if (mutex)
         icl_mutex_lock (mutex);
#endif

//
while ((thread = (smt_thread_t *) ipr_looseref_pop (self->thread_list))) {
icl_console_print ("end thread=%pp", thread);
    smt_thread_shut_down (thread);
    smt_thread_unlink (&thread);
}
ipr_looseref_list_destroy (&self->thread_list);
#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

}
/*  -------------------------------------------------------------------------
    http_address_thread_open

    Type: Component method
    Adds an http_agent thread to the address's list of threads and culls
    the oldest thread if the address has gone over quota.
    -------------------------------------------------------------------------
 */

int
    http_address_thread_open (
    http_address_table_t * table,       //  Not documented
    http_access_context_t * context     //  Not documented
)
{
http_address_t
    *self;
smt_thread_t
    *thread;
    int
        rc = 0;                         //  Not documented

//
assert (table);
assert (context);
assert (context->address);
assert (context->thread);

self = http_address_table_search (table, context->address);
if (!self) {
    self = http_address_new (table, context->address);
    //  This can return null if two threads create the same address at
    //  the same time... so let's go find the actual address object
    if (!self) {
        self = http_address_table_search (table, context->address);
        assert (self);
    }
}
assert (self);

self_lock (self);
ipr_looseref_queue (self->thread_list, smt_thread_link (context->thread));
//  Cull oldest thread if quota is non-zero but would become zero
//  I.e. if it's exactly one... a quota of zero means "unlimited".
if ( self->thread_quota
&& --self->thread_quota == 0) {
    //  Find oldest looseref entry
    thread = (smt_thread_t *) ipr_looseref_pop (self->thread_list);
    assert (thread);
    smt_thread_shut_down (thread);
    smt_thread_unlink (&thread);
    self->thread_quota++;
}
self_unlock (self);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_address_thread_close

    Type: Component method
    Removes an http_agent thread from the address's list of threads.  The
    thread will be missing from the table, if it was already culled.
    -------------------------------------------------------------------------
 */

int
    http_address_thread_close (
    http_address_table_t * table,       //  Not documented
    http_access_context_t * context     //  Not documented
)
{
ipr_looseref_t
    *looseref;
http_address_t
    *self;
smt_thread_t
    *thread;
    int
        rc = 0;                         //  Not documented

//
assert (table);
assert (context);
assert (context->address);
assert (context->thread);

self = http_address_table_search (table, context->address);
assert (self);
self_lock (self);

//  Find looseref entry for context->thread
looseref = ipr_looseref_list_first (self->thread_list);
while (looseref
&& (smt_thread_t *) looseref->object != context->thread)
    looseref = ipr_looseref_list_next (&looseref);

if (looseref) {
    //  Now kill the thread and get back our quota
    thread = (smt_thread_t *) looseref->object;
    ipr_looseref_destroy (&looseref);
    assert (thread);
    smt_thread_shut_down (thread);
    smt_thread_unlink (&thread);
    self->thread_quota++;
}
self_unlock (self);

    return (rc);
}
/*  -------------------------------------------------------------------------
    http_address_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_address_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    http_address_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    http_address_remove_from_all_containers (
    http_address_t * self               //  The item
)
{

HTTP_ADDRESS_ASSERT_SANE (self);
http_address_table_remove (self);
}
/*  -------------------------------------------------------------------------
    http_address_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_address_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
http_address_t
    *self;
int
    container_links;


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <http_address file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    http_address_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_address_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    http_address_destroy

    Type: Component method
    Destroys a http_address_t object. Takes the address of a
    http_address_t reference (a pointer to a pointer) and nullifies the
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
    http_address_destroy_ (
    http_address_t * ( * self_p ),      //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    http_address_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    http_address_annihilate (self_p);
    http_address_free ((http_address_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    http_address_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static http_address_t *
    http_address_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    http_address_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    http_address_cache_initialise ();

self = (http_address_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (http_address_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    http_address_free

    Type: Component method
    Freess a http_address_t object.
    -------------------------------------------------------------------------
 */

static void
    http_address_free (
    http_address_t * self               //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->mutex)
        icl_mutex_destroy (&self->mutex);
#endif
        memset (&self->object_tag, 0, sizeof (http_address_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (http_address_t));
        self->object_tag = HTTP_ADDRESS_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    http_address_lock

    Type: Component method
    Accepts a http_address_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_address_lock (
    http_address_t * self               //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_ADDRESS_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_mutex_lock (self->mutex);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    http_address_unlock

    Type: Component method
    Accepts a http_address_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    http_address_unlock (
    http_address_t * self               //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

HTTP_ADDRESS_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_mutex_unlock (self->mutex);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    http_address_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    http_address_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (http_address_t));
icl_system_register (http_address_cache_purge, http_address_cache_terminate);
}
/*  -------------------------------------------------------------------------
    http_address_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_address_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    http_address_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    http_address_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    http_address_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    http_address_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

http_address_animating = enabled;
}
/*  -------------------------------------------------------------------------
    http_address_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    http_address_new_in_scope_ (
    http_address_t * * self_p,          //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    http_address_table_t * table,       //  Table to insert into
    char * key                          //  Hash key
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = http_address_new_ (file,line,table,key);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) http_address_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__http_address_version_start   = "VeRsIoNsTaRt:ipc";
char *EMBED__http_address_component       = "http_address ";
char *EMBED__http_address_version         = "1.0 ";
char *EMBED__http_address_copyright       = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__http_address_filename        = "http_address.icl ";
char *EMBED__http_address_builddate       = "2011/03/01 ";
char *EMBED__http_address_version_end     = "VeRsIoNeNd:ipc";

