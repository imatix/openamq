/*---------------------------------------------------------------------------
    smt_signal.c - smt_signal component

    The smt_signal class is used to manage signal handlers in an SMT
    application.
    Generated from smt_signal.icl by icl_gen using GSL/4.
    
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
#include "smt_signal.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_signal_t

//  Shorthands for class methods

#define self_initialise     smt_signal_initialise
#define self_terminate      smt_signal_terminate
#define self_new            smt_signal_new
#define self_annihilate     smt_signal_annihilate
#define self_register       smt_signal_register
#define self_register_shut_down  smt_signal_register_shut_down
#define self_deregister     smt_signal_deregister
#define self_handler        smt_signal_handler
#define self_selftest       smt_signal_selftest
#define self_remove_from_all_containers  smt_signal_remove_from_all_containers
#define self_show           smt_signal_show
#define self_destroy        smt_signal_destroy
#define self_alloc          smt_signal_alloc
#define self_free           smt_signal_free
#define self_read_lock      smt_signal_read_lock
#define self_write_lock     smt_signal_write_lock
#define self_unlock         smt_signal_unlock
#define self_cache_initialise  smt_signal_cache_initialise
#define self_cache_purge    smt_signal_cache_purge
#define self_cache_terminate  smt_signal_cache_terminate
#define self_show_animation  smt_signal_show_animation

static void
    smt_signal_terminate (
void);

#define smt_signal_new(table,key)       smt_signal_new_ (__FILE__, __LINE__, table, key)
static smt_signal_t *
    smt_signal_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
smt_signal_table_t * table,             //  Table to insert into
int key                                 //  Hash key
);

static void
    smt_signal_annihilate (
smt_signal_t * ( * self_p )             //  Reference to object reference
);

static void
    smt_signal_handler (
int the_signal                          //  Not documented
);

#define smt_signal_alloc()              smt_signal_alloc_ (__FILE__, __LINE__)
static smt_signal_t *
    smt_signal_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_signal_free (
smt_signal_t * self                     //  Object reference
);

static void
    smt_signal_cache_initialise (
void);

static void
    smt_signal_cache_purge (
void);

static void
    smt_signal_cache_terminate (
void);

Bool
    smt_signal_animating = TRUE;        //  Animation enabled by default
static Bool
    s_smt_signal_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_smt_signal_mutex       = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


smt_thread_t
    *s_signal_thread;                   //  Only one thread may receive signals

smt_signal_table_t
    *s_signal_table;
/*  -------------------------------------------------------------------------
    smt_signal_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_signal_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_smt_signal_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_smt_signal_mutex)
            s_smt_signal_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_smt_signal_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_smt_signal_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

s_signal_thread = NULL;
s_signal_table  = smt_signal_table_new ();
            s_smt_signal_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_smt_signal_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    smt_signal_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_terminate (
void)
{

if (s_smt_signal_active) {

smt_signal_table_destroy (&s_signal_table);
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_smt_signal_mutex);
#endif
        s_smt_signal_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    smt_signal_new

    Type: Component method
    Creates and initialises a new smt_signal_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

static smt_signal_t *
    smt_signal_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    smt_signal_table_t * table,         //  Table to insert into
    int key                             //  Hash key
)
{
    smt_signal_t *
        self = NULL;                    //  Object reference

if (!s_smt_signal_active)
    self_initialise ();
    self = smt_signal_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_SIGNAL_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_signal_show_);
#endif

self->by_thread_next = self;
self->by_thread_prev = self;
self->by_thread_head = NULL;
self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->event = SMT_NULL_EVENT;
if (table && self && smt_signal_table_insert (table, key, self))
    smt_signal_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_signal_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_annihilate (
    smt_signal_t * ( * self_p )         //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

int                                 //  Need to save key value because it
    the_signal = (*self_p)->key;    //  gets lost on removal from table.
    smt_signal_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    SMT_SIGNAL_ASSERT_SANE (self);
    smt_signal_remove_from_all_containers (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

if (the_signal)                     //  Will be zero when purging signals
    signal (the_signal, SIG_DFL);

//  If thread no longer has any signals registered, reset s_signal_thread
if (s_signal_thread
&&  smt_signal_by_thread_isempty (s_signal_thread->signal_list))
    s_signal_thread = NULL;
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    smt_signal_register

    Type: Component method
    Register a signal handler.  Upon receipt of the_signal, SMT will deliver
    the requested event to the specified thread.  If the signal was already
    registered by a different thread, returns -1.
    -------------------------------------------------------------------------
 */

int
    smt_signal_register (
    int the_signal,                     //  The signal to register
    smt_thread_t * thread,              //  The thread to register the signal with
    smt_event_t event                   //  The event to be sent on signal
)
{
smt_signal_t
    *self;
    int
        rc = 0;                         //  Return code

if (!s_smt_signal_active)
    self_initialise ();
if (!s_signal_thread) {
    s_signal_thread = thread;
    if (!thread->signal_list)
        thread->signal_list = smt_signal_by_thread_new ();
}
else
if (thread != s_signal_thread)
    rc = -1;
else
if (event == SMT_NULL_EVENT) {
    icl_console_print ("Event must be provided in signal registration.");
    rc = -1;
}
if (!rc) {
    //  Find or create a signal entry.
    self = smt_signal_table_search (s_signal_table, the_signal);
    if (!self)
        self = smt_signal_new (s_signal_table, the_signal);

    if (self->event == SMT_NULL_EVENT)          
        smt_signal_by_thread_queue (thread->signal_list, self);

    self->event = event;

    //  And register the signal handler.
    signal (the_signal, smt_signal_handler);
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_register_shut_down

    Type: Component method
    Register a shutdown signal.  Upon receipt of the_signal, SMT will proceed
    with a graceful shutdown of the application by sending a shutdown event to
    all agents.
    -------------------------------------------------------------------------
 */

void
    smt_signal_register_shut_down (
    int the_signal                      //  The signal to register
)
{
smt_signal_t
    *self;

if (!s_smt_signal_active)
    self_initialise ();
//  Check whether signal is already registered with some thread
self = smt_signal_table_search (s_signal_table, the_signal);
if (!self)
    self = smt_signal_new (s_signal_table, the_signal);

if (self->event != SMT_NULL_EVENT)
    smt_signal_by_thread_remove (self);

self->event = SMT_NULL_EVENT;

//  And register the signal handler.
signal (the_signal, smt_signal_handler);
}
/*  -------------------------------------------------------------------------
    smt_signal_deregister

    Type: Component method
    Deregister the specified shutdown signal.
    -------------------------------------------------------------------------
 */

void
    smt_signal_deregister (
    int the_signal                      //  The signal to deregister
)
{
smt_signal_t
    *self;

if (!s_smt_signal_active)
    self_initialise ();
//  Check whether signal is already registered with some thread
self = smt_signal_table_search (s_signal_table, the_signal);
if (self)
    smt_signal_destroy (&self);
}
/*  -------------------------------------------------------------------------
    smt_signal_handler

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_handler (
    int the_signal                      //  Not documented
)
{
smt_signal_t
    *self;

//  If signal table doesn't exist then we do nothing.
//
if (s_signal_table) {
    self = smt_signal_table_search (s_signal_table, the_signal);

    //  Check that signal is still registered.  Can happen that signal
    //  is delivered to a thread after another thread has deregistered it.
    if (self) {
        //  See whether this is an event or shutdown signal.
        if (self->event != SMT_NULL_EVENT) {
            s_signal_thread->signal_raised = TRUE;
            s_signal_thread->signal_event  = self->event;
            smt_thread_wake (s_signal_thread);
        }
        else {
            //  Set flags so that running threads will notice.
            smt_signal_raised = TRUE;
            smt_break_requested = TRUE;
            smt_os_thread_wake (smt_os_thread);
        }
        //  Re-register the signal, seems to be needed on some systems
        signal (the_signal, smt_signal_handler);
    }
}
}
/*  -------------------------------------------------------------------------
    smt_signal_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_signal_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_signal_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    smt_signal_remove_from_all_containers (
    smt_signal_t * self                 //  The itemThe item
)
{

SMT_SIGNAL_ASSERT_SANE (self);
smt_signal_table_remove (self);
smt_signal_by_thread_remove (self);
}
/*  -------------------------------------------------------------------------
    smt_signal_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_signal_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
smt_signal_t
    *self;
int
    container_links;


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
if (self->by_thread_head)
   container_links++;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <smt_signal file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    smt_signal_destroy

    Type: Component method
    Destroys a smt_signal_t object. Takes the address of a
    smt_signal_t reference (a pointer to a pointer) and nullifies the
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
    smt_signal_destroy_ (
    smt_signal_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_signal_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    smt_signal_annihilate (self_p);
    smt_signal_free ((smt_signal_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_signal_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_signal_t *
    smt_signal_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_signal_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_signal_cache_initialise ();

self = (smt_signal_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_signal_free

    Type: Component method
    Freess a smt_signal_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_signal_free (
    smt_signal_t * self                 //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        self->object_tag = SMT_SIGNAL_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_signal_read_lock

    Type: Component method
    Accepts a smt_signal_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_signal_read_lock (
    smt_signal_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_SIGNAL_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_write_lock

    Type: Component method
    Accepts a smt_signal_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_signal_write_lock (
    smt_signal_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_SIGNAL_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_unlock

    Type: Component method
    Accepts a smt_signal_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_signal_unlock (
    smt_signal_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_SIGNAL_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_signal_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_signal_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_signal_t));
icl_system_register (smt_signal_cache_purge, smt_signal_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_signal_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_signal_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_signal_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_signal_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_signal_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_signal_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__smt_signal_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_signal_component         = "smt_signal ";
char *EMBED__smt_signal_version           = "1.0b0 ";
char *EMBED__smt_signal_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_signal_filename          = "smt_signal.icl ";
char *EMBED__smt_signal_builddate         = "2011/03/01 ";
char *EMBED__smt_signal_version_end       = "VeRsIoNeNd:ipc";

