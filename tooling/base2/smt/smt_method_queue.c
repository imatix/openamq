/*---------------------------------------------------------------------------
    smt_method_queue.c - smt_method_queue component

    This class implements the queue container for SMT method queues.
    Generated from smt_method_queue.icl by icl_gen using GSL/4.
    
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
#include "smt_method_queue.h"           //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_method_queue_t

//  Shorthands for class methods

#define self_new            smt_method_queue_new
#define self_sync_new       smt_method_queue_sync_new
#define self_wait           smt_method_queue_wait
#define self_isempty        smt_method_queue_isempty
#define self_trigger        smt_method_queue_trigger
#define self_register       smt_method_queue_register
#define self_annihilate     smt_method_queue_annihilate
#define self_free           smt_method_queue_free
#define self_selftest       smt_method_queue_selftest
#define self_pop            smt_method_queue_pop
#define self_queue          smt_method_queue_queue
#define self_terminate      smt_method_queue_terminate
#define self_show           smt_method_queue_show
#define self_destroy        smt_method_queue_destroy
#define self_alloc          smt_method_queue_alloc
#define self_link           smt_method_queue_link
#define self_unlink         smt_method_queue_unlink
#define self_cache_initialise  smt_method_queue_cache_initialise
#define self_cache_purge    smt_method_queue_cache_purge
#define self_cache_terminate  smt_method_queue_cache_terminate
#define self_show_animation  smt_method_queue_show_animation
#define self_new_in_scope   smt_method_queue_new_in_scope

#define smt_method_queue_annihilate(self)  smt_method_queue_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_method_queue_annihilate_ (
smt_method_queue_t * ( * self_p ),      //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_method_queue_free(self)     smt_method_queue_free_ (self, __FILE__, __LINE__)
static void
    smt_method_queue_free_ (
smt_method_queue_t * self,              //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_method_queue_alloc()        smt_method_queue_alloc_ (__FILE__, __LINE__)
static smt_method_queue_t *
    smt_method_queue_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_method_queue_cache_initialise (
void);

static void
    smt_method_queue_cache_purge (
void);

static void
    smt_method_queue_cache_terminate (
void);

Bool
    smt_method_queue_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_method_queue_new

    Type: Component method
    Creates and initialises a new smt_method_queue_t object, returns a
    reference to the created object.
    Creates a simple method queue without synchrononisation methods.
    -------------------------------------------------------------------------
 */

smt_method_queue_t *
    smt_method_queue_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_method_queue_t *
        self = NULL;                    //  Object reference

    self = smt_method_queue_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_METHOD_QUEUE_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_method_queue_show_);
#endif

#if (defined (BASE_THREADSAFE))
    self->mutex = icl_mutex_new ();
#endif

    self->head = NULL;
    self->tail = NULL;

    self->count = 0;

    self->notify = NULL;
    self->data   = NULL;
#if (defined (BASE_THREADSAFE))
    self->sync_mutex    = NULL;
    self->sync_cond     = NULL;
    self->sync_wait_cnt = 0;
#else
    self->sync = FALSE;
#endif
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_sync_new

    Type: Component method
    Creates and initialises a new smt_method_queue_t object, returns a
    reference to the created object.
    Creates a method queue with synchrononisation methods (wait and trigger).
    -------------------------------------------------------------------------
 */

smt_method_queue_t *
    smt_method_queue_sync_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_method_queue_t *
        self = NULL;                    //  Object reference

    self = smt_method_queue_new ();
#if (defined (BASE_THREADSAFE))
    self->sync_mutex = icl_mutex_new ();
    self->sync_cond  = icl_cond_new ();
#else
    self->sync = TRUE;
#endif

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_wait

    Type: Component method
    Accepts a smt_method_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Wait until the method queue gets triggered or timeout occurs.

    In a multi-thread environment this means waiting on the thread's
    condition variable.  The trigger method will signal that condition
    variable, allowing this method to terminate.

    In a single-thread environment this means running the SMT engine as long
    as it wants to run.  The trigger method will signal to SMT that it's time
    to stop, allowing this method to terminate.

    The method returns zero if an event arrived or the request timed out
    and -1 if the queue was destroyed.
    -------------------------------------------------------------------------
 */

int
    smt_method_queue_wait (
    smt_method_queue_t * self,          //  Reference to object
    qbyte msecs                         //  Timeout for wait
)
{
    int64_t
        usecs = msecs * 1000;
#if defined (BASE_THREADSAFE)
    int
        wait_rc = 0;
    apr_time_t
        time_at_call = 0,
        time_at_return;
#else
    smt_timer_request_t
        *timer_request = NULL;
#endif
    int
        rc = 0;                         //  Return code

SMT_METHOD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

#if defined (BASE_THREADSAFE)
    assert (self->sync_mutex);

    //  First check that we can't return immediately without placing lock
    if (smt_method_queue_isempty (self)) {
        icl_mutex_lock (self->sync_mutex);

        while (!self->zombie
           &&  smt_method_queue_isempty (self)
           &&  !wait_rc) {
            self->sync_wait_cnt++;
            if (usecs) {
                time_at_return = smt_time_now ();
                if (time_at_call != 0)  //  Not first time through
                    usecs -= (time_at_return - time_at_call);
                if (usecs > 0) {
                    time_at_call = time_at_return;
                    wait_rc = icl_cond_timed_wait (self->sync_cond, self->sync_mutex, usecs);
                }
                else
                    wait_rc = APR_TIMEUP;
            }
            else
                icl_cond_wait (self->sync_cond, self->sync_mutex);

            self->sync_wait_cnt--;
        }
        icl_mutex_unlock (self->sync_mutex);
    }
#else
    assert (self->sync);

    if (usecs)
        timer_request = smt_timer_request_new (smt_os_thread, 
                                               smt_time_now () + usecs,
                                               SMT_NULL_EVENT);
    while (!self->zombie
       &&  smt_method_queue_isempty (self)) {
        smt_wait (0);
        if (timer_request
        &&  timer_request->expired) {
            rc = SMT_TIMEOUT;
            break;
        }
    }
    if (timer_request)
        smt_timer_request_destroy (&timer_request);
#endif
    if (self->zombie)
        rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_isempty

    Type: Component method
    Accepts a smt_method_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    smt_method_queue_isempty (
    smt_method_queue_t * self           //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_METHOD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

rc = (self->count == 0);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_trigger

    Type: Component method
    Accepts a smt_method_queue_t reference and returns zero in case of success,
    1 in case of errors.
    Trigger the queue.  See the description of the 'wait' method.
    -------------------------------------------------------------------------
 */

int
    smt_method_queue_trigger (
    smt_method_queue_t * self,          //  Reference to object
    smt_method_queue_notify_t event     //  What kind of trigger is this?
)
{
    int
        rc = 0;                         //  Return code

SMT_METHOD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

    //  Call registered notify callback.
    if (self->notify)
        (self->notify) (self->data, event);

    //  Do thread synchronisation.                                             
#if defined (BASE_THREADSAFE)
    if (self->sync_mutex) {
        icl_mutex_lock    (self->sync_mutex);
        if (self->sync_wait_cnt)
            icl_cond_signal (self->sync_cond);
        icl_mutex_unlock  (self->sync_mutex);
    }
#else
    if (self->sync)
        smt_request_break ();
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_register

    Type: Component method
    Accepts a smt_method_queue_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    smt_method_queue_register (
    smt_method_queue_t * self,          //  Reference to object
    smt_method_queue_notify_fn * notify,   //  Function to call on arrival
    void * data                         //  Data for notify
)
{
    int
        rc = 0;                         //  Return code

SMT_METHOD_QUEUE_ASSERT_SANE (self);
if (!self->zombie) {

self->notify = notify;
self->data   = data;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_annihilate

    Type: Component method
    Destroys the queue and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    smt_method_queue_annihilate_ (
    smt_method_queue_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
    int
        history_last;
#endif

    smt_method_t
        *item;
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    smt_method_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = self->links;
#endif

    SMT_METHOD_QUEUE_ASSERT_SANE (self);

#if defined (BASE_THREADSAFE)
    mutex = self->mutex;
    if (mutex)
         icl_mutex_lock (mutex);
#endif


    item = (smt_method_t *) self->head;
    while (item != NULL) {
        //  Unlink child without lock (we already have the lock)
        self->head = item->queue_next;

        smt_method_unlink (&item);
        item = (smt_method_t *) self->head;
    }
smt_method_queue_trigger (self, SMT_METHOD_QUEUE_DESTROY);
#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

}
/*  -------------------------------------------------------------------------
    smt_method_queue_free

    Type: Component method
    Freess a smt_method_queue_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_method_queue_free_ (
    smt_method_queue_t * self,          //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = self->links;
#endif

#if defined (BASE_THREADSAFE)
    //  Get and release lock before destroying.  Otherwise we may clash        
    //  with another thread.                                                   
    if (self->sync_mutex) {
        icl_mutex_lock    (self->sync_mutex);
        icl_mutex_unlock  (self->sync_mutex);
        icl_mutex_destroy (&self->sync_mutex);

        icl_cond_destroy  (&self->sync_cond);
    }
#endif
#if defined (BASE_THREADSAFE)
    if (self->mutex)
        icl_mutex_destroy (&self->mutex);
#endif
    self->object_tag = SMT_METHOD_QUEUE_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_method_queue_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_queue_selftest (
void)
{
smt_method_queue_t *self;

self = smt_method_queue_new ();
smt_method_queue_destroy (&self);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_pop

    Type: Component method
    Removes the next item in the queue, if any, and returns it.  If
    the queue was empty, returns NULL.

    The number of links is unchanged; removing from the queue and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

smt_method_t *
    smt_method_queue_pop (
    smt_method_queue_t * self           //  The queue
)
{
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    smt_method_t *
        item = NULL;                    //  Not documented

    SMT_METHOD_QUEUE_ASSERT_SANE (self);
#if defined (BASE_THREADSAFE)
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

    if (!self->zombie) {
        item = (smt_method_t *) self->head;
        if (item) {
            self->head = item->queue_next;
            if (self->head == NULL)
                self->tail = NULL;
            item->queue_next = NULL;
            item->queue_head = NULL;
        }

        if (item) {
            icl_atomic_dec32 (&self->count);
        }
    }

#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

    return (item);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_queue

    Type: Component method
    Attaches the item to the end of the specified queue.  The item may not
    be in a queue of the same class.

    The number of links is automatically adjusted - if the item was already
    in the queue, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_method_queue_queue (
    smt_method_queue_t * self,          //  Reference to object
    smt_method_t * item                 //  Not documented
)
{
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    int
        rc = 0;                         //  Return code

#if defined (BASE_THREADSAFE)
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

    SMT_METHOD_QUEUE_ASSERT_SANE (self);
    if (!self->zombie) {

        if (! item->queue_head) {
            smt_method_link (item);

            if (self->tail)
                self->tail->queue_next = item;
            else
                self->head = item;
            self->tail = item;
            item->queue_next = NULL;
            item->queue_head = self;
        }
        else {
            //  Requeuing into the same queue isn't allowed.
            icl_console_print ("E: smt_method item inserted into multiple queue containers");
            assert (item->queue_head == NULL);
        }

        icl_atomic_inc32 (&self->count);

    }
    else
        rc = -1;

#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_queue_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    smt_method_queue_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_queue_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_method_queue_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_method_queue zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
    if (self->history_last > SMT_METHOD_QUEUE_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_METHOD_QUEUE_HISTORY_LENGTH;
        self->history_last %= SMT_METHOD_QUEUE_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_METHOD_QUEUE_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_method_queue>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_method_queue_destroy

    Type: Component method
    Destroys a smt_method_queue_t object. Takes the address of a
    smt_method_queue_t reference (a pointer to a pointer) and nullifies the
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
    smt_method_queue_destroy_ (
    smt_method_queue_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_method_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_method_queue_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_method_queue", "E: missing link on smt_method_queue object");
        smt_method_queue_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_method_queue_free_ ((smt_method_queue_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_method_queue_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_method_queue_t *
    smt_method_queue_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_method_queue_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_method_queue_cache_initialise ();

self = (smt_method_queue_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_method_queue_t *
    smt_method_queue_link_ (
    smt_method_queue_t * self,          //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
    int
        history_last;
#endif

    if (self) {
        SMT_METHOD_QUEUE_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_method_queue_unlink_ (
    smt_method_queue_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
    int
        history_last;
#endif

    smt_method_queue_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        SMT_METHOD_QUEUE_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_method_queue", "E: missing link on smt_method_queue object");
            smt_method_queue_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_METHOD_QUEUE)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_METHOD_QUEUE_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_method_queue_annihilate_ (self_p, file, line);
        smt_method_queue_free_ ((smt_method_queue_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_method_queue_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_method_queue_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_method_queue_t));
icl_system_register (smt_method_queue_cache_purge, smt_method_queue_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_method_queue_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_method_queue_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_method_queue_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_method_queue_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_method_queue_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_method_queue_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_method_queue_animating = enabled;
}
/*  -------------------------------------------------------------------------
    smt_method_queue_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_method_queue_new_in_scope_ (
    smt_method_queue_t * * self_p,      //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_method_queue_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_method_queue_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_method_queue_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_method_queue_component   = "smt_method_queue ";
char *EMBED__smt_method_queue_version     = "1.0 ";
char *EMBED__smt_method_queue_copyright   = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_method_queue_filename    = "smt_method_queue.icl ";
char *EMBED__smt_method_queue_builddate   = "2011/03/01 ";
char *EMBED__smt_method_queue_version_end  = "VeRsIoNeNd:ipc";

