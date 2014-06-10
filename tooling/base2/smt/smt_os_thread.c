/*---------------------------------------------------------------------------
    smt_os_thread.c - smt_os_thread component

    The smt_os_thread class manipulates OS threads for SMT.

    It is for SMT internal use only.
    Generated from smt_os_thread.icl by icl_gen using GSL/4.
    
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
#include "smt_os_thread.h"              //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_os_thread_t

//  Shorthands for class methods

#define self_new            smt_os_thread_new
#define self_start          smt_os_thread_start
#define self_annihilate     smt_os_thread_annihilate
#define self_free           smt_os_thread_free
#define self_remove_thread  smt_os_thread_remove_thread
#define self_request_break  smt_request_break
#define self_shut_down      smt_shut_down
#define self_wait           smt_wait
#define self_wake           smt_os_thread_wake
#define self_assign         smt_os_thread_assign
#define self_selftest       smt_os_thread_selftest
#define self_detect_signal  smt_os_thread_detect_signal
#define self_set_polling_threads  smt_set_polling_threads
#define self_set_working_threads  smt_set_working_threads
#define self_set_action_limit  smt_set_action_limit
#define self_time_now       smt_time_now
#define self_initialise     smt_initialise
#define self_terminate      smt_terminate
#define self_remove_from_all_containers  smt_os_thread_remove_from_all_containers
#define self_show           smt_os_thread_show
#define self_join           smt_os_thread_join
#define self_destroy        smt_os_thread_destroy
#define self_alloc          smt_os_thread_alloc
#define self_link           smt_os_thread_link
#define self_unlink         smt_os_thread_unlink
#define self_cache_initialise  smt_os_thread_cache_initialise
#define self_cache_purge    smt_os_thread_cache_purge
#define self_cache_terminate  smt_os_thread_cache_terminate
#define self_show_animation  smt_os_thread_show_animation

#define smt_os_thread_new()             smt_os_thread_new_ (__FILE__, __LINE__)
static smt_os_thread_t *
    smt_os_thread_new_ (
char * file,                            //  Source file for callSource file for call
size_t line                             //  Line number for callLine number for call
);

static int
    smt_os_thread_start (
smt_os_thread_t * self                  //  Reference to object
);

#define smt_os_thread_annihilate(self)  smt_os_thread_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_os_thread_annihilate_ (
smt_os_thread_t * ( * self_p ),         //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define smt_os_thread_free(self)        smt_os_thread_free_ (self, __FILE__, __LINE__)
static void
    smt_os_thread_free_ (
smt_os_thread_t * self,                 //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_os_thread_detect_signal (
void);

#define smt_os_thread_alloc()           smt_os_thread_alloc_ (__FILE__, __LINE__)
static smt_os_thread_t *
    smt_os_thread_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_os_thread_cache_initialise (
void);

static void
    smt_os_thread_cache_purge (
void);

static void
    smt_os_thread_cache_terminate (
void);

Bool
    smt_os_thread_animating = TRUE;     //  Animation enabled by default
static Bool
    s_smt_os_thread_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_smt_os_thread_mutex    = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


#define SMT_SCHEDULE_COUNT      2       //  Number of sleeping thread queues
#define SMT_SCHEDULE_DEFAULT    {2, 2}  //  Number of OS threads per schedule

#define SMT_LOOP_DETECTION      1       //  Abort after this no. of seconds
#define SMT_MAX_SOCKETS         1024    //  Maximum number of busy sockets
#if !defined(HAVE_KQUEUE) && !defined(HAVE_EPOLL) && !defined(HAVE_POLL)  &&  defined(FD_SETSIZE)  && FD_SETSIZE < SMT_MAX_SOCKETS
#    undef  SMT_MAX_SOCKETS
#    define SMT_MAX_SOCKETS     FD_SETSIZE
#endif

#if (defined (BASE_THREADSAFE)) && (defined (__WINDOWS__))
#define SMT_PORT_BASE 20000
#endif

//  Global variables

volatile qbyte
    smt_signal_raised = FALSE;          //  Set in handler
Bool
    smt_break_requested = FALSE;        //  Interrupt SMT operation?
volatile qbyte
    smt_actions_remaining = 0;          //  Action count for debugging
smt_os_thread_t
    *smt_os_thread;                     //  One of the threads

//  Local static variables.

static volatile qbyte
    smt_signal_noted;                   //  Set by 1st to detect signal

#if (defined (BASE_THREADSAFE))
static volatile apr_time_t
    s_time_now;
static volatile qbyte
    s_os_thread_active_count = 0;       //  Number of active OS threads
static icl_cond_t
    *s_sync_cond = NULL;                //  For synching thread operations     
static volatile qbyte
    s_sync_cond_waiting = 0;            //  Avoid signal when noone waiting    
static icl_mutex_t
    *s_sync_mutex = NULL;               //  For synching thread operations     
static smt_os_thread_by_status_t
    **s_sleeping_queue = NULL;          //  One queue per schedule
static smt_os_thread_list_t
    *s_os_thread_list = NULL;           //  List of all OS threads

#    if (defined (__WINDOWS__))
static apr_port_t
    s_port;
static apr_socket_t
    *s_master_sock = NULL;
#    endif

static uint
    s_os_thread_cnt[SMT_SCHEDULE_COUNT] = SMT_SCHEDULE_DEFAULT,
    s_last_os_thread[SMT_SCHEDULE_COUNT],
    s_os_thread_first[SMT_SCHEDULE_COUNT];
static smt_os_thread_t
    **s_os_thread = NULL;               //  Array of OS threads

//  Statistics                                                                 

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
static icl_stats_t
    *s_count_stat,
    *s_active_stat;
#    endif

#endif

static Bool
    s_terminating = FALSE;              //  Let interrupt handler know

//  Local function prototypes

static void * APR_THREAD_FUNC
    s_execute (apr_thread_t *apr_thread, void * data);

#if (defined (BASE_THREADSAFE))
static void
    s_decrement_active_thread_count (void);
static void * APR_THREAD_FUNC
    s_time_update (apr_thread_t *apr_thread, void * data);
#endif

#if (defined (__WINDOWS__))
static BOOL WINAPI
    s_handle_ctrl (DWORD ctrltype);
#endif
/*  -------------------------------------------------------------------------
    smt_os_thread_new

    Type: Component method
    Creates and initialises a new smt_os_thread_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    Initialises the new item to be a valid list item.
    Internal method.
    
    Creates a new OS thread and inserts it into the list of OS threads.
    -------------------------------------------------------------------------
 */

static smt_os_thread_t *
    smt_os_thread_new_ (
    char * file,                        //  Source file for callSource file for call
    size_t line                         //  Line number for callLine number for call
)
{
    int
        priority_index;
#if (defined (BASE_THREADSAFE))
    apr_pollfd_t
        pollfd;
#    if (defined (__WINDOWS__))
    apr_sockaddr_t
        *sockaddr;
#    endif
#endif
    smt_os_thread_t *
        self = NULL;                    //  Object reference

if (!s_smt_os_thread_active)
    self_initialise ();
    self = smt_os_thread_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_OS_THREAD_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_os_thread_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
self->by_status_next = self;
self->by_status_prev = self;
self->by_status_head = NULL;
    self->file          = file;
    self->line          = line;
    self->started       = FALSE;
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "smt_os_thread_new");
#if (defined (BASE_THREADSAFE))
    self->threadattr = NULL;
    icl_apr_assert (apr_threadattr_create (&self->threadattr, self->pool));
#endif

    self->startup              = s_execute;
    self->shutting_down        = FALSE;
    self->shutdown_request     = FALSE;
    self->terminate_request    = FALSE;
    self->socket_request_count = 0;
    self->timer_request_count  = 0;
    self->timer_request_list   = smt_timer_request_list_new ();
    self->thread_list          = smt_thread_list_new ();
    self->waiting_list         = smt_thread_by_status_new ();
    self->waking_list          = smt_thread_by_status_new ();
    self->last_poll_tag        = 0;

    for (priority_index = 0; priority_index < SMT_PRIORITY_LAST; priority_index++)
        self->waiting_list_entry [priority_index] = NULL;

    icl_apr_assert (apr_pollset_create (
        &self->pollset, SMT_MAX_SOCKETS, self->pool, 0 /*APR_POLLSET_THREADSAFE*/ ));
    icl_apr_assert (apr_pollset_create (
        &self->waking_pollset, SMT_MAX_SOCKETS, self->pool, 0 /*APR_POLLSET_THREADSAFE*/ ));

#if (defined (BASE_THREADSAFE))
    self->thread_queue         = smt_thread_queue_new ();
    self->sleeping_queue       = NULL;
#    if (defined (__WINDOWS__))
    icl_apr_assert (apr_socket_create (&self->sock_in, APR_INET, SOCK_STREAM, APR_PROTO_TCP, self->pool));
    icl_apr_assert (apr_sockaddr_info_get (&sockaddr, "localhost", APR_INET, s_port, 0, self->pool));
    icl_apr_assert (apr_socket_connect (self->sock_in, sockaddr));
    icl_apr_assert (apr_socket_accept (&self->sock_out, s_master_sock, self->pool));

    icl_apr_assert (apr_socket_opt_set (self->sock_in,  APR_SO_NONBLOCK, 1));
    icl_apr_assert (apr_socket_opt_set (self->sock_out, APR_SO_NONBLOCK, 1));

    pollfd.p           = self->pool;
    pollfd.desc_type   = APR_POLL_SOCKET;
    pollfd.reqevents   = APR_POLLIN;
    pollfd.rtnevents   = 0;
    pollfd.desc.s      = self->sock_in;
    pollfd.client_data = NULL;
#    else
    icl_apr_assert (apr_file_pipe_create (
        &self->pipe_in, &self->pipe_out, self->pool));
    pollfd.p           = self->pool;
    pollfd.desc_type   = APR_POLL_FILE;
    pollfd.reqevents   = APR_POLLIN;
    pollfd.rtnevents   = 0;
    pollfd.desc.f      = self->pipe_in;
    pollfd.client_data = NULL;
#    endif

    self->cond    = icl_cond_new ();
    self->mutex   = icl_mutex_new ();
    self->waiting = FALSE;

#if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    self->thread_count_stat = NULL;
    self->state_wake_stat   = NULL;
    self->state_poll_stat   = NULL;
    self->state_sleep_stat  = NULL;
    self->state_active_stat = NULL;
#endif


    icl_apr_assert (apr_pollset_add (self->pollset, &pollfd));
    icl_apr_assert (apr_pollset_add (self->waking_pollset, &pollfd));

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    icl_stats_inc (NULL, &s_count_stat);
#    endif

    smt_os_thread_list_queue (s_os_thread_list, self);
#endif

    icl_atomic_set32 (&self->status, SMT_OS_THREAD_NEWBORN);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_start

    Type: Component method
    Accepts a smt_os_thread_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static int
    smt_os_thread_start (
    smt_os_thread_t * self              //  Reference to object
)
{
qbyte
    old_status;
    int
        rc = 0;                         //  Return code

SMT_OS_THREAD_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_atomic_inc32 (&s_os_thread_active_count);
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    icl_stats_inc (NULL, &s_active_stat);
#    endif
#endif

    //  Set status to active and assert that it was newborn.
    old_status = icl_atomic_cas32 (&self->status, SMT_OS_THREAD_ACTIVE, SMT_OS_THREAD_NEWBORN);
    assert (old_status == SMT_OS_THREAD_NEWBORN);
    self->started = TRUE;
#if (defined (BASE_THREADSAFE))
    smt_os_thread_link (self);          //  OS thread needs its own link
    icl_apr_assert (apr_thread_create (&self->thread, self->threadattr, self->startup, self, self->pool));
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_annihilate_ (
    smt_os_thread_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    int
        history_last;
#endif

    smt_os_thread_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = self->links;
#endif

    SMT_OS_THREAD_ASSERT_SANE (self);
    smt_os_thread_remove_from_all_containers (self);

    apr_pollset_destroy            ( self->pollset);
    apr_pollset_destroy            ( self->waking_pollset);
    smt_timer_request_list_destroy (&self->timer_request_list);
    smt_thread_list_destroy        (&self->thread_list);
    smt_thread_by_status_destroy   (&self->waiting_list);
    smt_thread_by_status_destroy   (&self->waking_list);

#if (defined (BASE_THREADSAFE))
    smt_thread_queue_destroy       (&self->thread_queue);
    icl_cond_destroy               (&self->cond);
    icl_mutex_destroy              (&self->mutex);

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    icl_stats_dec (NULL, &s_count_stat);
#    endif
#endif

}
/*  -------------------------------------------------------------------------
    smt_os_thread_free

    Type: Component method
    Freess a smt_os_thread_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_free_ (
    smt_os_thread_t * self,             //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = self->links;
#endif

#if (defined (BASE_THREADSAFE))
    //  Close the pipe/socket only on free to avoid other threads having
    //  to synchronise pipe activity.                                          
#    if (defined (__WINDOWS__))
    icl_apr_assert (apr_socket_close (self->sock_in));
    icl_apr_assert (apr_socket_close (self->sock_out));
#    else
    icl_apr_assert (apr_file_close (self->pipe_in));
    icl_apr_assert (apr_file_close (self->pipe_out));
#    endif
#endif
apr_pool_destroy (self->pool);
    self->object_tag = SMT_OS_THREAD_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_request_break

    Type: Component method
    This method requests a break in the SMT engine.
    -------------------------------------------------------------------------
 */

void
    smt_request_break (
void)
{

smt_break_requested = TRUE;
}
/*  -------------------------------------------------------------------------
    smt_shut_down

    Type: Component method
    Requests all threads to shut down, thus eventually terminating the SMT
    engine.
    -------------------------------------------------------------------------
 */

void
    smt_shut_down (
void)
{
#if (defined (BASE_THREADSAFE))
    smt_os_thread_t
        *os_thread;
#endif

#if (defined (BASE_THREADSAFE))
    //  We cheat a little with the active thread count to stop the first
    //  thread shutting down quickly triggering the master thread.
    icl_atomic_inc32 (&s_os_thread_active_count);

    //  Shut down the threads that haven't started yet.
    smt_thread_shut_down_all_newborn ();

    os_thread = smt_os_thread_list_first (s_os_thread_list);
    while (os_thread) {
        os_thread->shutdown_request = TRUE;
        smt_os_thread_wake (os_thread);
        os_thread = smt_os_thread_list_next (&os_thread);
    }

    //  Now it's OK to decrement the active thread count.
    s_decrement_active_thread_count ();
#else
    smt_os_thread->shutdown_request = TRUE;
    smt_os_thread_wake (smt_os_thread);
#endif
}
/*  -------------------------------------------------------------------------
    smt_wait

    Type: Component method
    Wait until synchronisation condition variable is signalled.
    
    In single-threaded mode 'wait' means 'run the SMT engine'.
    -------------------------------------------------------------------------
 */

int
    smt_wait (
    qbyte msecs                         //  Timeout for wait
)
{
#if (!defined (BASE_THREADSAFE))
    smt_timer_request_t
        *timer_request;
#endif
    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    icl_mutex_lock (s_sync_mutex);
    while (icl_atomic_get32 (&s_os_thread_active_count)) {
        s_sync_cond_waiting++;
        if (msecs)
            icl_cond_timed_wait (s_sync_cond, s_sync_mutex, msecs * 1000);
        else
            icl_cond_wait       (s_sync_cond, s_sync_mutex);

        s_sync_cond_waiting--;
    }
    icl_mutex_unlock (s_sync_mutex);
#else
    if (msecs)
        timer_request = smt_timer_request_new (smt_os_thread, 
                                               smt_time_now () + 1000 * msecs,
                                               SMT_NULL_EVENT);

    s_execute (NULL, smt_os_thread);      //  Timer will interrupt this.
    if (msecs) {
        if (timer_request->expired)
            rc = SMT_TIMEOUT;
        smt_timer_request_destroy (&timer_request);
    }
#endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_wake

    Type: Component method
    Accepts a smt_os_thread_t reference and returns zero in case of success,
    1 in case of errors.
    Wake an OS thread to do some work.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_wake_ (
    smt_os_thread_t * self,             //  Reference to object
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if (defined (BASE_THREADSAFE)) && (defined (__WINDOWS__))
    size_t
        nbytes;
#endif
#if (defined (BASE_THREADSAFE))
    qbyte
        old_status;
#endif
    int
        rc = 0;                         //  Return code

SMT_OS_THREAD_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    //  Must increment active count first to avoid premature shutdown.  We'll
    //  decrement it later if it turns out this increment was incorrect.
    icl_atomic_inc32 (&s_os_thread_active_count);
    old_status = icl_atomic_swap (&self->status, SMT_OS_THREAD_ACTIVE);

    if (old_status == SMT_OS_THREAD_SLEEPING) {
        icl_mutex_lock (self->mutex);
        if (self->waiting)
            icl_cond_signal (self->cond);

        icl_mutex_unlock (self->mutex);

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
        icl_stats_inc (NULL, &s_active_stat);
#    endif
    }
    else {
        s_decrement_active_thread_count ();

        if (old_status == SMT_OS_THREAD_POLLING) {
#    if (defined (__WINDOWS__))
            nbytes = 1;
            icl_apr_assert (apr_socket_send (self->sock_out, "0", &nbytes));
            assert (nbytes == 1);
#    else
            icl_apr_assert (apr_file_putc (0, self->pipe_out));
#    endif
        }
    }

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    if (old_status != SMT_OS_THREAD_ACTIVE)
        icl_stats_inc (NULL, &self->state_wake_stat);
#    endif
#else
    icl_atomic_set32 (&self->status, SMT_OS_THREAD_ACTIVE);
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_assign

    Type: Component method
    Assigns a newly-created threadlet to an OS thread.  If the OS thread is NULL
    then the threadlet is assigned according to our scheduling strategy.
    -------------------------------------------------------------------------
 */

smt_os_thread_t *
    smt_os_thread_assign (
    smt_thread_t * thread,              //  The threadlet to assign
    smt_os_thread_t * self,             //  OS thread to assign to
    uint schedule                       //  The sleeping thread list index
)
{
#if (defined (BASE_THREADSAFE))
    smt_os_thread_t
        *self_link;
    uint
        os_thread_index;
#endif

#if (defined (BASE_THREADSAFE))
    assert (!thread->os_thread);

    //  Assignment strategy: Normal scheduling = try thread that has been 
    //  sleeping the longest, not for polling since this means that all slave
    //  threads end up in same OS thread.
    if (schedule == SMT_SCHEDULE_NORMAL
    &&  !self) {
        self = smt_os_thread_by_status_pop (s_sleeping_queue[schedule]);
        if (self) {
            assert (self->sleeping_queue == s_sleeping_queue[schedule]);
            self_link = self;
            smt_os_thread_unlink (&self_link);
        }
    }

    //  ...and if that doesn't work, cycle through threads.
    if (!self) {
        os_thread_index = icl_atomic_inc32 (&s_last_os_thread[schedule]);
        self = s_os_thread[s_os_thread_first[schedule] + s_last_os_thread[schedule] % s_os_thread_cnt[schedule]];
    }

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
        icl_stats_inc (NULL, &self->thread_count_stat);
#    endif
#endif

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_os_thread_detect_signal

    Type: Component method
    A normal shutdown should not take more than X seconds.  If we get
    a second signal after that time, we can assume the application is
    looping and will not halt normally; we can then abort it.  We do
    not abort for alarm signals.
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_detect_signal (
void)
{
static volatile apr_time_t
    last_interrupt = 0;             //  To trap looping code

if (icl_atomic_cas32 (&smt_signal_noted, TRUE, FALSE) == FALSE) {
    last_interrupt = smt_time_now ();
    icl_console_print ("W: process was interrupted... shutting down");
    if (!s_terminating)
        smt_shut_down ();
}
else {
    //  Spin until last_interrupt gets set.  We really should do an atomic
    //  assignment of last_interrupt, but don't have atomic 64-bit ops.
    while (last_interrupt == 0);
    if (smt_time_now () - last_interrupt > SMT_LOOP_DETECTION * 1000000) {
        icl_system_panic ("smt_os_thread", "E: application looping - aborted");
        icl_trace_dump (NULL);
        abort ();
    }
}
}
/*  -------------------------------------------------------------------------
    smt_set_polling_threads

    Type: Component method
    This thread sets the number of OS threads dedicated to polling.
    -------------------------------------------------------------------------
 */

void
    smt_set_polling_threads (
    int number                          //  Number of polling OS threads
)
{

#if (defined (BASE_THREADSAFE))
    s_os_thread_cnt [SMT_SCHEDULE_POLL] = number;
#endif
}
/*  -------------------------------------------------------------------------
    smt_set_working_threads

    Type: Component method
    This thread sets the number of OS threads dedicated to working, ie not 
    polling.
    -------------------------------------------------------------------------
 */

void
    smt_set_working_threads (
    int number                          //  Number of working OS threads
)
{

#if (defined (BASE_THREADSAFE))
    s_os_thread_cnt [SMT_SCHEDULE_NORMAL] = number;
#endif
}
/*  -------------------------------------------------------------------------
    smt_set_action_limit

    Type: Component method
    This method allows the number of actions that SMT will execute before
    breaking.
    
    Note that only actions executed in agents that are compiled with
    SMT_ACTION_LIMIT defined are counted.
    -------------------------------------------------------------------------
 */

void
    smt_set_action_limit (
    int action_limit                    //  Number of actions before break
)
{

smt_actions_remaining = action_limit;
}
/*  -------------------------------------------------------------------------
    smt_time_now

    Type: Component method
    Returns the global variable time_now which is maintained with a special
    thread.  The purpose of all this is to avoid too many expensive calls to
    apr_time_now.
    -------------------------------------------------------------------------
 */

apr_time_t
    smt_time_now (
void)
{
    apr_time_t
        time_now;                       //  Not documented

#if (defined (BASE_THREADSAFE))
    //  Spin to catch case where time rolls over, since fetch isn't atomic.
    do time_now = s_time_now;
    until (time_now == s_time_now);
#else
    time_now = apr_time_now ();;
#endif

    return (time_now);
}
/*  -------------------------------------------------------------------------
    smt_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_initialise (
void)
{
    char
        *strvalue = NULL;
#if (defined (BASE_THREADSAFE))
    uint
        os_thread_index,
        schedule_index,
        total_os_thread_cnt,
        os_thread_per_schedule_index;
    smt_os_thread_t
        *os_thread;
#    if (defined (__WINDOWS__))
    apr_status_t
        status;
    apr_sockaddr_t
        *sockaddr;
#    endif
    apr_threadattr_t
        *time_threadattr;
    apr_thread_t
        *time_thread;
#endif

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_smt_os_thread_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_smt_os_thread_mutex)
            s_smt_os_thread_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_smt_os_thread_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_smt_os_thread_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

    s_smt_os_thread_active = TRUE;      //  To prevent recursion

    //  Various classes have to be initialised in the right order.
    smt_signal_initialise ();
    smt_timer_request_initialise ();
    smt_socket_request_initialise ();
    smt_thread_initialise ();

#if (defined (BASE_THREADSAFE))
    //  Set global time before any other code tries to use it
    s_time_now = apr_time_now ();
    icl_apr_assert (apr_threadattr_create (&time_threadattr, icl_global_pool));
    icl_apr_assert (apr_thread_create (&time_thread, time_threadattr, s_time_update, NULL, icl_global_pool));

    //  Removed for now, this message should appear only when we run
    //  in some kind of verbose mode... it is distracting to have this
    //  on all console output.  - PH 2006/02/10
    //icl_console_print ("I: SMT will use %i OS threads.", s_os_thread_cnt);
    s_sync_cond  = icl_cond_new ();
    s_sync_mutex = icl_mutex_new ();

    s_sleeping_queue = icl_mem_alloc (SMT_SCHEDULE_COUNT * sizeof (smt_os_thread_by_status_t *));
    for (schedule_index = 0; schedule_index < SMT_SCHEDULE_COUNT; schedule_index++)
        s_sleeping_queue[schedule_index] = smt_os_thread_by_status_new ();

    s_os_thread_list = smt_os_thread_list_new ();

#    if (defined (__WINDOWS__))
    icl_apr_assert (apr_socket_create (
        &s_master_sock,  APR_INET, SOCK_STREAM, APR_PROTO_TCP, icl_global_pool));
    s_port = SMT_PORT_BASE;
    do {
        icl_apr_assert (apr_sockaddr_info_get (&sockaddr, NULL, APR_INET, s_port, 0, icl_global_pool));
        status = apr_socket_bind (s_master_sock, sockaddr);
        if (status != APR_SUCCESS)
            s_port++;
    } while (status != APR_SUCCESS);
    icl_apr_assert (apr_socket_listen (s_master_sock, 1));
//  Removed pending smt-level trace option
//    icl_console_print ("Thread communication master socket bound to port %lu", (long) s_port);
#    endif

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    s_count_stat      = icl_stats_new ("smt_os_thread count");
    s_active_stat     = icl_stats_new ("smt_os_thread active");
#    endif

#endif

    //  Initialise total number of actions before break
    if (!smt_actions_remaining) {
        apr_env_get (&strvalue, "SMT_ACTION_LIMIT", icl_global_pool);
        if (strvalue)
            smt_actions_remaining = atoi (strvalue);
    }

    smt_signal_raised = FALSE;
#if (defined (__WINDOWS__))
    //  Windows stuff to make signal handlers work
    SetConsoleCtrlHandler (s_handle_ctrl, TRUE);
#endif

    //  Set default signal handlers
    smt_signal_register_shut_down (SIGINT);
    smt_signal_register_shut_down (SIGTERM);

    //  On some systems we get a 'broken pipe' when a connection fails     
#   if defined (SIGPIPE)
    signal (SIGPIPE, SIG_IGN);
#   endif

#if (defined (BASE_THREADSAFE))
    //  Prepare OS threads and schedule structures

    total_os_thread_cnt = 0;
    for (schedule_index = 0; schedule_index < SMT_SCHEDULE_COUNT; schedule_index++) {
        total_os_thread_cnt += s_os_thread_cnt[schedule_index];
        s_last_os_thread[schedule_index] = 0;
    }

    s_os_thread = icl_mem_alloc (total_os_thread_cnt * sizeof (smt_os_thread_t *));

    schedule_index = 0;
    os_thread_per_schedule_index = 0;
    s_os_thread_first[schedule_index] = 0;
    for (os_thread_index = 0; os_thread_index < total_os_thread_cnt; os_thread_index++) {
        os_thread = smt_os_thread_new ();
        s_os_thread[os_thread_index] = os_thread;
        if (++os_thread_per_schedule_index > s_os_thread_cnt[schedule_index]) {
            schedule_index++;
            os_thread_per_schedule_index = 0;
            s_os_thread_first[schedule_index] = os_thread_index;
        }
        s_os_thread[os_thread_index]->sleeping_queue =
            s_sleeping_queue[schedule_index];
        smt_os_thread_start (os_thread);
    }

    smt_os_thread = s_os_thread[0];

    //  And wait for all the threads to settle.
    smt_wait (0);
#else
    smt_os_thread = smt_os_thread_new (); //  Destroyed in s_execute             
    smt_os_thread_start (smt_os_thread);
#endif
            s_smt_os_thread_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_smt_os_thread_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    smt_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_terminate (
void)
{
#if (defined (BASE_THREADSAFE))
    uint
        os_thread_index,
        schedule_index;
    smt_os_thread_t
        *os_thread,
        *os_thread_link,
        *os_thread_next;
#endif

if (s_smt_os_thread_active) {

    s_terminating = TRUE;

#if (defined (BASE_THREADSAFE))

    os_thread = smt_os_thread_list_first (s_os_thread_list);
    while (os_thread) {
        //  Need to do some tricky link management, as these threads are
        //  going to disappear the moment they are woken.
        os_thread_link = smt_os_thread_link      (os_thread);
        os_thread_next = smt_os_thread_list_next (&os_thread_link);
        os_thread->terminate_request = TRUE;
        smt_os_thread_wake (os_thread);
        smt_os_thread_unlink (&os_thread);
        os_thread = os_thread_next;
    }

    //  Now wait for the thread OS threads to finish
    for (schedule_index = 0; schedule_index < SMT_SCHEDULE_COUNT; schedule_index++)
        for (os_thread_index = 0; os_thread_index < s_os_thread_cnt[schedule_index]; os_thread_index++) {
            smt_os_thread_join (s_os_thread[s_os_thread_first[schedule_index] + os_thread_index]);
            smt_os_thread_unlink (&s_os_thread[s_os_thread_first[schedule_index] + os_thread_index]);
        }
    icl_mem_free (s_os_thread);

    for (schedule_index = 0; schedule_index < SMT_SCHEDULE_COUNT; schedule_index++)
        smt_os_thread_by_status_destroy (&s_sleeping_queue[schedule_index]);
    icl_mem_free (s_sleeping_queue);

    smt_os_thread_list_destroy      (&s_os_thread_list);

    icl_mutex_lock    (s_sync_mutex);
    icl_mutex_unlock  (s_sync_mutex);
    icl_mutex_destroy (&s_sync_mutex);
    icl_cond_destroy  (&s_sync_cond);
#else
    smt_os_thread->terminate_request = TRUE;
    smt_os_thread_wake (smt_os_thread);
    smt_wait (0);                       //  Let SMT terminate
#endif
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_smt_os_thread_mutex);
#endif
        s_smt_os_thread_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    smt_os_thread_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_remove_from_all_containers (
    smt_os_thread_t * self              //  The itemThe item
)
{

smt_os_thread_by_status_remove (self);
smt_os_thread_list_remove (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_os_thread_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->by_status_head)
   container_links++;
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_os_thread zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    if (self->history_last > SMT_OS_THREAD_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_OS_THREAD_HISTORY_LENGTH;
        self->history_last %= SMT_OS_THREAD_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_OS_THREAD_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_os_thread>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_os_thread_join

    Type: Component method
    Does a 'join' to the thread which waits until the thread terminates.
    
    Don't use 'function' template since it needs to be called with a zombie
    object.
    -------------------------------------------------------------------------
 */

int
    smt_os_thread_join (
    smt_os_thread_t * self              //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

    SMT_OS_THREAD_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    apr_thread_join (&rc, self->thread);
#endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_destroy

    Type: Component method
    Destroys a smt_os_thread_t object. Takes the address of a
    smt_os_thread_t reference (a pointer to a pointer) and nullifies the
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
    smt_os_thread_destroy_ (
    smt_os_thread_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_os_thread_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_os_thread_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_os_thread", "E: missing link on smt_os_thread object");
        smt_os_thread_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_os_thread_free_ ((smt_os_thread_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_os_thread_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_os_thread_t *
    smt_os_thread_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_os_thread_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_os_thread_cache_initialise ();

self = (smt_os_thread_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_os_thread_t *
    smt_os_thread_link_ (
    smt_os_thread_t * self,             //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    int
        history_last;
#endif

    if (self) {
        SMT_OS_THREAD_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_unlink_ (
    smt_os_thread_t * ( * self_p ),     //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
    int
        history_last;
#endif

    smt_os_thread_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        SMT_OS_THREAD_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_os_thread", "E: missing link on smt_os_thread object");
            smt_os_thread_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_OS_THREAD)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_OS_THREAD_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_os_thread_annihilate_ (self_p, file, line);
        smt_os_thread_free_ ((smt_os_thread_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    smt_os_thread_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_os_thread_t));
icl_system_register (smt_os_thread_cache_purge, smt_os_thread_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_os_thread_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_os_thread_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_os_thread_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_os_thread_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_os_thread_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_os_thread_animating = enabled;
}
#if (defined (BASE_THREADSAFE))

/*  -------------------------------------------------------------------------
 *  s_decrement_active_thread_count -- internal
 *
 *  Decrements the count of the number of active threads, triggering
 *  the synchronisation condition variable if required.
 */

static void
s_decrement_active_thread_count (void)
{
    assert (s_os_thread_active_count > 0);
    if (icl_atomic_dec32 (&s_os_thread_active_count) == 0) {
        icl_mutex_lock (s_sync_mutex);
        if (s_sync_cond_waiting)
            icl_cond_broadcast (s_sync_cond);

        icl_mutex_unlock (s_sync_mutex);
    }
}

/*  -------------------------------------------------------------------------
 *  s_time_update -- internal
 *
 *  Maintains the global time
 */

static void * APR_THREAD_FUNC
s_time_update (apr_thread_t *apr_thread, void *data)
{
    FOREVER {
        //  Precision is ~100 Hz
        apr_sleep (10000);
        s_time_now = apr_time_now();
    }
}

#endif

#if (defined (BASE_THREADSAFE))
//  s_clear_queues : Pick up all threadlets on the waiting and waking queues
//                   and put them on our private waiting and waking list.

static void
s_clear_queues (smt_os_thread_t *self)
{
    smt_thread_t
        *thread;
    smt_method_t
        *method;
    smt_priority_t
        priority;

    thread = smt_thread_queue_pop (self->thread_queue);
    while (thread) {
        //  Skip if thread has been destroyed or is already active.
        if (!thread->zombie
        &&   thread->status != SMT_THREAD_ACTIVE) {
            //  First deal with brand new thread.  Use the list that it is
            //  in as a way of determining whether thread is brand new.
            if (thread->list_head != self->thread_list) {
                thread->os_thread = self;
                thread->poll_tag  = self->last_poll_tag;

                //  Since smt_thread_list_t is the only container that makes
                //  links to the thread we need to be careful that the code
                //  below doesn't cause the thread to be freed. We add an
                //  extra link then remove it again.  Not a performance issue
                //  because this only occurs once per thread.
                smt_thread_link (thread);
                smt_thread_list_remove (thread);    //  From s_thread_newborn_list                    
                smt_thread_list_queue (self->thread_list, thread);
                smt_thread_insert_into_waiting_list (thread);
                smt_thread_unlink (&thread);
            }
            else
            //  If there are events in the reply queue then the thread is 
            //  definitely waking so we can put in the waking list.
            if (smt_thread_activity (thread)) {
                if (thread->monitor) {

                    //  Unless it has a high priority method...
                    smt_thread_clear_method_queue (thread);
                    method = smt_method_list_first (thread->reply_list);
                    priority = method ? method->priority : SMT_PRIORITY_NORMAL;
                    smt_method_unlink (&method);

                    //  Note that priority order is reversed...
                    if (priority < SMT_PRIORITY_NORMAL) {
                        smt_socket_request_destroy (&thread->monitor);
                        icl_atomic_set32 (&thread->status, SMT_THREAD_WAITING);
                        smt_thread_insert_into_waiting_list (thread);
                    }
                    else {
                        icl_apr_assert (apr_pollset_add (self->waking_pollset, &thread->monitor->socket->pollfd));
                        smt_thread_by_status_queue (self->waking_list, thread);
                    }
                }
                //  If no monitor then thread is ready to go.
                else
                    smt_thread_insert_into_waiting_list (thread);
            }
            else

            //  Otherwise if it has a monitor request then it is a polling thread
            if (thread->monitor && thread->module == SMT_TERM_MODULE) {
                icl_atomic_set32 (&thread->status, SMT_THREAD_POLLING);
                //  Unless it got woken in the meantime and is thus a waking thread
                if (smt_thread_activity (thread)) {
                    if (icl_atomic_cas32 (
                        &thread->status, SMT_THREAD_WAKING, SMT_THREAD_POLLING) == SMT_THREAD_POLLING) {
                        icl_apr_assert (apr_pollset_add (self->waking_pollset, &thread->monitor->socket->pollfd));
                        smt_thread_by_status_queue (self->waking_list, thread);
                    }
                }
            }

            //  Final case is simple
            else {
                icl_atomic_set32 (&thread->status, SMT_THREAD_WAITING);
                smt_thread_insert_into_waiting_list (thread);
            }
        }

        thread = smt_thread_queue_pop (self->thread_queue);
    }
}
#endif

/*  -------------------------------------------------------------------------
 *  s_execute -- internal
 *
 *  Executes SMT threads in a given OS thread.
 */

static void * APR_THREAD_FUNC
s_execute (apr_thread_t *apr_thread, void *data)
{
    smt_os_thread_t
        *self = data;
    smt_thread_t
        *thread,
        *thread_loop;
#if (defined (BASE_THREADSAFE))
    qbyte
        old_status;                     //  OS thread status                   
#else
    smt_thread_t
        *thread_link;
#endif
#if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
#   define BUFFER_LENGTH 64
    char
        buffer[BUFFER_LENGTH];;
    uint
        length = 0;
#endif
    int
        rc = 0;

#if (defined (BASE_THREADSAFE))
    //  Pick up our OS thread id
    self->apr_os_thread = apr_os_thread_current ();

#   if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    length = apr_snprintf (buffer, BUFFER_LENGTH, "smt_os_thread[%li] ", (long int) self->apr_os_thread);

    apr_snprintf (buffer + length, BUFFER_LENGTH - length, "thread_count");
    self->thread_count_stat = icl_stats_new (buffer);

    apr_snprintf (buffer + length, BUFFER_LENGTH - length, "wake");
    self->state_wake_stat = icl_stats_new (buffer);
    apr_snprintf (buffer + length, BUFFER_LENGTH - length, "poll");
    self->state_poll_stat = icl_stats_new (buffer);
    apr_snprintf (buffer + length, BUFFER_LENGTH - length, "sleep");
    self->state_sleep_stat = icl_stats_new (buffer);
    apr_snprintf (buffer + length, BUFFER_LENGTH - length, "active");
    self->state_active_stat = icl_stats_new (buffer);
#   endif

#else
    if (!smt_os_thread)                   //  Can't run if already shut down.    
        return NULL;

    smt_break_requested = FALSE;
#endif

    FOREVER {
#if (defined (BASE_THREADSAFE))

        s_clear_queues (self);
        thread = smt_thread_waiting_list_pop (self);

        //  If no thread to execute, work out what to do next.  The two almost
        //  identical blocks of code cover the case where we have socket or
        //  timer requests, and the case where we don't.
        if (!thread) {

            //  If we have socket or timer requests, get ready to wait on them.
            if (self->socket_request_count
            ||  self->timer_request_count) {
                //  First queue onto the sleeping OS thread queue
                smt_os_thread_by_status_queue (self->sleeping_queue, self);

                //  Set our OS thread status to polling
                icl_atomic_set32 (&self->status, SMT_OS_THREAD_POLLING);

                //  Now that status has changed, we can catch a race condition.
                s_clear_queues (self);
                thread = smt_thread_waiting_list_pop (self);

                //  If there is a thread to execute, set our status to active.
                if (thread) {
                    old_status = icl_atomic_swap (&self->status, SMT_OS_THREAD_ACTIVE);
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
                    if (old_status != SMT_OS_THREAD_ACTIVE)
                        icl_stats_inc (NULL, &self->state_active_stat);
#    endif
                }
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
                else
                    icl_stats_inc (NULL, &self->state_poll_stat);
#    endif
            }
            else {
                //  First queue onto the sleeping OS thread queue
                smt_os_thread_by_status_queue (self->sleeping_queue, self);

                //  We delay update of active count until below so that the
                //  count can't hit zero prematurely.

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
                old_status = icl_atomic_swap (&self->status, SMT_OS_THREAD_SLEEPING);
                if (old_status != SMT_OS_THREAD_SLEEPING)
                    icl_stats_inc (NULL, &self->state_sleep_stat);
#    else
                icl_atomic_set32 (&self->status, SMT_OS_THREAD_SLEEPING);
#    endif

                //  Now that status has changed, we can catch a race condition.
                s_clear_queues (self);
                thread = smt_thread_waiting_list_pop (self);

                //  If there is a thread to execute, set our status to active.
                if (thread) {
                    old_status = icl_atomic_swap (&self->status, SMT_OS_THREAD_ACTIVE);
                    //  Only need to adjust active count if contention occurred
                    if (old_status != SMT_OS_THREAD_SLEEPING)
                        s_decrement_active_thread_count ();
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
                    if (old_status != SMT_OS_THREAD_ACTIVE)
                        icl_stats_inc (NULL, &self->state_active_stat);
#    endif
                }
                else {
                    //  In this case we adjust active count down
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
                    icl_stats_dec (NULL, &s_active_stat);
#    endif
                    s_decrement_active_thread_count ();
                }
            }
        } 

        //  If we still don't have a thread to execute then now is the time
        //  to do the actual waiting.
        if (!thread) {           
            //  Skip it all if there was a shutdown request or raised signal.
            //  This is needed because on a race condition a thread can run 
            //  past the shutdown trap immediately below and be back here for
            //  a blocking poll without having detected the shutdown request.
            if (!self->shutdown_request) {

                //  Clear the queues and do a poll.  This lets us poll all 
                //  recently woken threads with outstanding monitor requests.
                s_clear_queues (self);

                //  Non-blocking poll first, then only blocking poll if still no
                //  threads waiting.
                self->last_poll_tag++;
                rc = smt_socket_request_poll (self, TRUE);
                if (rc== SMT_OK
                &&  self->status != SMT_OS_THREAD_ACTIVE
                &&  smt_thread_by_status_isempty (self->waiting_list)) {
                    if (self->socket_request_count
                    ||  self->timer_request_count) {
                        rc = smt_socket_request_wait (self);
                    }
                    else {
                        icl_mutex_lock (self->mutex);
                        while (self->status == SMT_OS_THREAD_SLEEPING) {
                            self->waiting = TRUE;
                            icl_cond_wait (self->cond, self->mutex);
                            self->waiting = FALSE;
                        }
                        icl_mutex_unlock (self->mutex);
                    }
                }
            }

            //   Change status to active if it isn't already.  Increment active
            //   count first, then decrement if required, to avoid premature
            //   shutdown.
            icl_atomic_inc32 (&s_os_thread_active_count);
            old_status = icl_atomic_swap (&self->status, SMT_OS_THREAD_ACTIVE);
            if (old_status != SMT_OS_THREAD_SLEEPING)
                s_decrement_active_thread_count ();
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
            else
                icl_stats_inc (NULL, &s_active_stat);
#    endif

#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
            if (old_status != SMT_OS_THREAD_ACTIVE)
                icl_stats_inc (NULL, &self->state_active_stat);
#    endif

            //  The following line removes the OS thread from the list of      
            //  sleeping threads, if indeed it is still there.
            smt_os_thread_by_status_remove (self);

            if (rc == SMT_OK) {
                s_clear_queues (self);
                thread = smt_thread_waiting_list_pop (self);
                if (thread) {
                    icl_atomic_set32 (&thread->status, SMT_THREAD_ACTIVE);

                    //  Update record of last poll for the threadlet.
                    thread->poll_tag = self->last_poll_tag;
                }
            }
        }
        else {
            icl_atomic_set32 (&thread->status, SMT_THREAD_ACTIVE);

            //  Clear the queues and do a poll.  This lets us poll all recently
            //  woken threads with outstanding monitor requests.
            s_clear_queues (self);
            rc = smt_socket_request_poll (self, FALSE);

            //  Update record of last poll for the threadlet.
            thread->poll_tag = self->last_poll_tag;
        }
#else
        //  Look for a thread to execute.
        thread = smt_thread_waiting_list_pop (self);
        //  Get threadlet that was just assigned
        if (thread
        &&  thread->list_head != self->thread_list) {
            thread->os_thread = smt_os_thread;
            thread->poll_tag  = smt_os_thread->last_poll_tag;

            //  Since smt_thread_list_t is the only container that makes
            //  links to the thread we need to be careful that the code
            //  below doesn't cause the thread to be freed. We add an
            //  extra link then remove it again.  Not a performance issue
            //  because this only occurs once per thread.
            thread_link = smt_thread_link (thread);
            smt_thread_list_remove (thread);    //  From s_thread_newborn_list                    
            smt_thread_list_queue (self->thread_list, thread);
            smt_thread_unlink (&thread_link);
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
            icl_stats_inc (NULL, &self->thread_count_stat);
#    endif
        }
        //  If no thread to execute, work out what to do next.                 
        if (!thread) {
            //  If we have a thread with requests, wait on them.  Don't wait
            //  on a timer if there is no thread.
            if (self->socket_request_count
            ||  self->timer_request_count) {
                if (self->terminate_request)
                    break;
                else
                if (smt_break_requested) {
                    smt_break_requested = FALSE;
                    rc = SMT_SLEEPING;
                    break;
                }
                else {
                    icl_atomic_set32 (&self->status, SMT_OS_THREAD_POLLING);

                    //  Non-blocking poll first, then only blocking poll if still no
                    //  threads waiting.
                    self->last_poll_tag++;
                    rc = smt_socket_request_poll (self, TRUE);
                    if (rc== SMT_OK
                    && (!smt_break_requested)
                    &&  smt_thread_by_status_isempty (self->waiting_list))
                        rc = smt_socket_request_wait (self);

                    //  Timer may request a break
                    if (rc == SMT_OK
                    &&  smt_break_requested) {
                        smt_break_requested = FALSE;
                        rc = SMT_SLEEPING;
                        break;
                    }
                }
            }
            else {
                icl_atomic_set32 (&self->status, SMT_OS_THREAD_SLEEPING);
                smt_break_requested = FALSE;
                break;
            }

            icl_atomic_set32 (&self->status, SMT_OS_THREAD_ACTIVE);

            if (rc == SMT_OK) {
                thread = smt_thread_waiting_list_pop (self);
                if (thread) {
                    thread->status = SMT_THREAD_ACTIVE;

                    //  Update record of last poll for the threadlet.
                    thread->poll_tag = self->last_poll_tag;
                }
            }
        }
        else {
            thread->status = SMT_THREAD_ACTIVE;

            //  Clear the queues and do a poll.  This lets us poll all recently
            //  woken threads with outstanding monitor requests.
            rc = smt_socket_request_poll (self, FALSE);

            //  Update record of last poll for the threadlet.
            thread->poll_tag = self->last_poll_tag;
        }
#endif

        //  Detect whether we were interrupted by a signal
        if (smt_signal_raised
        && !smt_signal_noted)
            smt_os_thread_detect_signal ();

        //  Now detect a shutdown request
        if (self->shutdown_request) {
            self->shutting_down = TRUE;
            self->shutdown_request = FALSE;
            thread_loop = smt_thread_list_first (self->thread_list);
            while (thread_loop) {
                smt_thread_shut_down (thread_loop);
                smt_thread_purge     (thread_loop);
                if (thread_loop != thread)
                    smt_thread_activate (thread_loop);
                thread_loop = smt_thread_list_next (&thread_loop);
            }
        }

        //  And finally a terminate request
        if (!thread
        &&  self->terminate_request)
            break;

        if (thread) {

//  JS: Assertion to try to figure out what's going on with these queues
//  TE: Assertion removed for delivery. 'JS: lists are robust against this'
#if ! (defined (BASE_THREADSAFE) && defined (BASE_LOCKFREE))
            //  assert (!thread->queue_head);
#endif

            //  Pass shutdown request to thread.  In theory there should be
            //  no threads that haven't already received a shutdown request,
            //  but we don't trust the existing threads not to have created
            //  a new thread since the shutdown request occurred.
            if (self->shutdown_request
            &&  !thread->shutdown_request
            &&  !thread->shutting_down)
                smt_thread_shut_down (thread);

            rc = (thread->manager) (&thread);

            if (thread && thread->write_request) {
                smt_socket_request_write_data (thread->write_request);
                thread->write_request = NULL;
            }

            if (rc == SMT_OK
            ||  rc == SMT_SLEEPING)
                smt_thread_continue (thread);

            if (rc != SMT_OK
            &&  rc != SMT_SLEEPING
            &&  rc != SMT_FINISHED)
                icl_console_print ("E: %s", smt_thread_error (thread));

#if defined (BASE_THREADSAFE)
            if (smt_break_requested)
                smt_break_requested = FALSE;
#else
            if ((rc != SMT_OK
            &&   rc != SMT_SLEEPING
            &&   rc != SMT_FINISHED)
            ||  smt_break_requested) {
                smt_break_requested = FALSE;
                break;
            }
#endif
        }
    }
#if (defined (BASE_THREADSAFE))
    old_status = icl_atomic_swap (&self->status, SMT_OS_THREAD_ZOMBIE);

    smt_os_thread_destroy (&self);

    //  If the thread was previously sleeping, increment the active thread
    //  count before decrementing it.  Just to ensure that master thread
    //  definitely gets woken up if required.
    if (old_status == SMT_OS_THREAD_SLEEPING)
        icl_atomic_inc32 (&s_os_thread_active_count);
#    if (defined (BASE_STATS) || defined (BASE_STATS_SMT_OS_THREAD))
    else
        icl_stats_dec (NULL, &s_active_stat);
#    endif

    s_decrement_active_thread_count ();
#else
    //  Detect whether we were interrupted by a signal
    if (smt_signal_raised
    && !smt_signal_noted)
        smt_os_thread_detect_signal ();

    //  Now detect a shutdown request
    if (self->shutdown_request) {
        self->shutting_down = TRUE;
        self->shutdown_request = FALSE;
        thread_loop = smt_thread_list_first (self->thread_list);
        while (thread_loop) {
            smt_thread_shut_down (thread_loop);
            smt_thread_purge     (thread_loop);
            if (thread_loop != thread)
                smt_thread_activate (thread_loop);
            thread_loop = smt_thread_list_next (&thread_loop);
        }
    }

    if (self->terminate_request) {
        smt_os_thread_destroy (&self);
        smt_os_thread = NULL;
    }
#endif
    return NULL;
}

/*  -------------------------------------------------------------------------
 *  handle_signals -- internal
 *
 *  Directs SIGINT and SIGTERM to handle_signal to allow an orderly shutdown.
 */

#if (defined (__WINDOWS__))
//  Under Windows, ^C causes this handler to be run in a new thread.  We
//  don't want to do anything too fancy because SMT is not re-entrant.  So
//  we just raise a SIGINT to be handled separately.

BOOL WINAPI s_handle_ctrl (DWORD ctrltype)
{
    raise (SIGINT);
    return TRUE;
}
#endif

//  Embed the version information in the resulting binary

char *EMBED__smt_os_thread_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_os_thread_component      = "smt_os_thread ";
char *EMBED__smt_os_thread_version        = "1.0 ";
char *EMBED__smt_os_thread_copyright      = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_os_thread_filename       = "smt_os_thread.icl ";
char *EMBED__smt_os_thread_builddate      = "2011/03/01 ";
char *EMBED__smt_os_thread_version_end    = "VeRsIoNeNd:ipc";

