<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 -->
<class
    name      = "smt_os_thread"
    comment   = "SMT OS thread management class"
    version   = "1.0"
    script    = "icl_gen"
    >

<doc>
    The smt_os_thread class manipulates OS threads for SMT.

    It is for SMT internal use only.
</doc>

<option name = "links" value = "1"/>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc"  value = "cache" />
</inherit>
<inherit class = "icl_init" />

<inherit name = "icl_thread"/>
<inherit name = "icl_list_item">
    <option name = "prefix" value = "by_status" />
</inherit>
<inherit name = "icl_list_item" />

<import class = "smt_thread"/>

<public name = "header">
//  Scheduling macros

#define SMT_SCHEDULE_NORMAL     0
#define SMT_SCHEDULE_POLL       1

//  Types

typedef qbyte smt_poll_tag_t;

//  OS thread status

#define SMT_OS_THREAD_NEWBORN   0       //  Obvious
#define SMT_OS_THREAD_SLEEPING  1       //  Idle
#define SMT_OS_THREAD_POLLING   2       //  Waiting on socket activity
#define SMT_OS_THREAD_ACTIVE    3       //  Busy
#define SMT_OS_THREAD_ZOMBIE    4       //  Dead

</public>

<public>
//  Global variables

extern volatile qbyte
    smt_signal_raised;                  //  Set in handler
extern Bool
    smt_break_requested;                //  Interrupt SMT operation?
extern volatile qbyte
    smt_actions_remaining;              //  Action count for debugging
extern smt_os_thread_t
    *smt_os_thread;                     //  One of the threads
</public>

<private>
#define SMT_SCHEDULE_COUNT      2       //  Number of sleeping thread queues
#define SMT_SCHEDULE_DEFAULT    {2, 2}  //  Number of OS threads per schedule

#define SMT_LOOP_DETECTION      1       //  Abort after this no. of seconds
#define SMT_MAX_SOCKETS         1024    //  Maximum number of busy sockets
#if !defined(HAVE_KQUEUE) && !defined(HAVE_EPOLL) && !defined(HAVE_POLL) \
 &&  defined(FD_SETSIZE)  && FD_SETSIZE < SMT_MAX_SOCKETS
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

#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
</private>

<context>
#if (defined (BASE_THREADSAFE))
    apr_os_thread_t
        apr_os_thread;                  //  System dependent OS thread id
#endif
    Bool
        shutting_down;                  //  Thread is shutting down            
    Bool
        shutdown_request;               //  Signal thread to shut down
    Bool
        terminate_request;              //  Signal thread to terminate
    int
        socket_request_count,
        timer_request_count;
    smt_timer_request_list_t
        *timer_request_list;
    smt_poll_tag_t
        last_poll_tag;                  //  Incremented at each socket poll  
    apr_pollset_t
        *pollset,
        *waking_pollset;
    volatile qbyte
        status;                         //  OS thread status
    smt_thread_list_t
        *thread_list;                   //  All threadlets for this OS thread
        
//  The dynamic lists and queues of threadlets.  There are four of these:
//      1. waiting_list:  private (non-threadsafe) list of threadlets that are
//                        waiting to be serviced; pointers are maintained so
//                        that list can be maintained sorted by priority.
//      2. waking_list:   private (non-threadsafe) list of threadlets that are
//                        are waiting to be serviced but have socket handles
//                        to be checked first.
//      3. thread_queue:  threadsafe queue of threadlets that have been woken
//                        from another thread.
    smt_thread_by_status_t
        *waiting_list,                  //  Waiting threadlets
        *waking_list;                   //  Waking threadlets
    smt_thread_t
        *waiting_list_entry             //  Insertion point by priority
            [SMT_PRIORITY_LAST];
#if (defined (BASE_THREADSAFE))
    smt_thread_queue_t
        *thread_queue;                  //  Waiting & waking threadlets
    smt_os_thread_by_status_t
        *sleeping_queue;                //  The queue where this thread sleeps
#    if (defined (__WINDOWS__))
    apr_socket_t
        *sock_out,
        *sock_in;
#    else
    apr_file_t
        *pipe_in,                       //  Pipe is used to wake up            
        *pipe_out;                      //  a sleeping thread.                 
#    endif
//  Condition variable and mutex for thread synchronisation when no socket
//  requests.
    icl_cond_t
        *cond;
    icl_mutex_t
        *mutex;
    Bool
        waiting;
#endif
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_t
        *thread_count_stat,
        *state_wake_stat,
        *state_poll_stat,
        *state_sleep_stat,
        *state_active_stat;
#endif
</context>

<method name = "new" private = "1">
    <doc>
    Internal method.
    
    Creates a new OS thread and inserts it into the list of OS threads.
    </doc>
    <local>
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
    </local>
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

#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    self->thread_count_stat = NULL;
    self->state_wake_stat   = NULL;
    self->state_poll_stat   = NULL;
    self->state_sleep_stat  = NULL;
    self->state_active_stat = NULL;
#endif


    icl_apr_assert (apr_pollset_add (self->pollset, &pollfd));
    icl_apr_assert (apr_pollset_add (self->waking_pollset, &pollfd));
    
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_inc (NULL, &s_count_stat);
#    endif

    smt_os_thread_list_queue (s_os_thread_list, self);
#endif

    icl_atomic_set32 (&self->status, SMT_OS_THREAD_NEWBORN);
</method>

<method name = "start" private = "1">
    <local>
    qbyte
        old_status;
    </local>
#if (defined (BASE_THREADSAFE))
    icl_atomic_inc32 (&s_os_thread_active_count);
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_inc (NULL, &s_active_stat);
#    endif
#endif

    //  Set status to active and assert that it was newborn.
    old_status = icl_atomic_cas32 (&self->status, SMT_OS_THREAD_ACTIVE, SMT_OS_THREAD_NEWBORN);
    assert (old_status == SMT_OS_THREAD_NEWBORN);
</method>

<method name = "destroy">
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
    
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_dec (NULL, &s_count_stat);
#    endif
#endif
</method>

<method name = "free">
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
</method>

<method name = "remove thread" inline = "1">
    <argument name = "thread" type = "smt_thread_t *"/>
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    if (thread->os_thread)
        icl_stats_dec (NULL, &thread->os_thread->thread_count_stat);
#    endif
</method>

<method name = "request break" base = "smt">
    <doc>
    This method requests a break in the SMT engine.
    </doc>
    smt_break_requested = TRUE;
</method>

<method name = "shut down" base = "smt">
    <doc>
    Requests all threads to shut down, thus eventually terminating the SMT
    engine.
    </doc>
    <local>
#if (defined (BASE_THREADSAFE))
    smt_os_thread_t
        *os_thread;
#endif
    </local>
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
</method>

<method name = "wait" return = "rc" inherit = "none" base = "smt">
    <argument name = "msecs"   type = "qbyte">Timeout for wait</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <doc>
    Wait until synchronisation condition variable is signalled.
    
    In single-threaded mode 'wait' means 'run the SMT engine'.
    </doc>
    <local>
#if (!defined (BASE_THREADSAFE))
    smt_timer_request_t
        *timer_request;
#endif
    </local>
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
</method>

<method name = "wake" template = "function">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <doc>
    Wake an OS thread to do some work.
    </doc>
    <local>
#if (defined (BASE_THREADSAFE)) && (defined (__WINDOWS__))
    size_t
        nbytes;
#endif
#if (defined (BASE_THREADSAFE))
    qbyte
        old_status;
#endif
    </local>
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
            
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
            
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    if (old_status != SMT_OS_THREAD_ACTIVE)
        icl_stats_inc (NULL, &self->state_wake_stat);
#    endif
#else
    icl_atomic_set32 (&self->status, SMT_OS_THREAD_ACTIVE);
#endif
</method>

<method name = "assign" return = "self">
    <argument name = "thread" type = "smt_thread_t *">The threadlet to assign</argument>
    <argument name = "self" type = "smt_os_thread_t *">OS thread to assign to</argument>
    <argument name = "schedule" type = "uint">The sleeping thread list index</argument>
    <doc>
    Assigns a newly-created threadlet to an OS thread.  If the OS thread is NULL
    then the threadlet is assigned according to our scheduling strategy.
    </doc>
    <local>
#if (defined (BASE_THREADSAFE))
    smt_os_thread_t
        *self_link;
    uint
        os_thread_index;
#endif
    </local>
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
    
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
        icl_stats_inc (NULL, &self->thread_count_stat);
#    endif
#endif
</method>

<method name = "selftest" inherit = "none"/>

<private name = "footer">
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
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
    
#   if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
                    if (old_status != SMT_OS_THREAD_ACTIVE)
                        icl_stats_inc (NULL, &self->state_active_stat);
#    endif
                }
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
                else
                    icl_stats_inc (NULL, &self->state_poll_stat);
#    endif
            }
            else {
                //  First queue onto the sleeping OS thread queue
                smt_os_thread_by_status_queue (self->sleeping_queue, self);
                
                //  We delay update of active count until below so that the
                //  count can't hit zero prematurely.
                
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
                    if (old_status != SMT_OS_THREAD_ACTIVE)
                        icl_stats_inc (NULL, &self->state_active_stat);
#    endif
                }
                else {
                    //  In this case we adjust active count down
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
            else
                icl_stats_inc (NULL, &s_active_stat);
#    endif

#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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
</private>

<method name = "detect signal" private = "1">    
    <doc>
    A normal shutdown should not take more than X seconds.  If we get
    a second signal after that time, we can assume the application is
    looping and will not halt normally; we can then abort it.  We do
    not abort for alarm signals.
    </doc>
    <local>
    static volatile apr_time_t
        last_interrupt = 0;             //  To trap looping code
    </local>
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
</method>

<method name = "set polling threads" base = "smt">
    <argument name = "number" type = "int">Number of polling OS threads</argument>
    <doc>
    This thread sets the number of OS threads dedicated to polling.
    </doc>
#if (defined (BASE_THREADSAFE))
    s_os_thread_cnt [SMT_SCHEDULE_POLL] = number;
#endif
</method>

<method name = "set working threads" base = "smt">
    <argument name = "number" type = "int">Number of working OS threads</argument>
    <doc>
    This thread sets the number of OS threads dedicated to working, ie not 
    polling.
    </doc>
#if (defined (BASE_THREADSAFE))
    s_os_thread_cnt [SMT_SCHEDULE_NORMAL] = number;
#endif
</method>

<method name = "set action limit" base = "smt">
    <argument name = "action limit" type = "int">Number of actions before break</argument>
    <doc>
    This method allows the number of actions that SMT will execute before
    breaking.
    
    Note that only actions executed in agents that are compiled with
    SMT_ACTION_LIMIT defined are counted.
    </doc>
    smt_actions_remaining = action_limit;
</method>

<method name = "time now" base = "smt" return = "time now">
    <declare name = "time now" type = "apr_time_t"/>
    <doc>
    Returns the global variable time_now which is maintained with a special
    thread.  The purpose of all this is to avoid too many expensive calls to
    apr_time_now.
    </doc>
#if (defined (BASE_THREADSAFE))
    //  Spin to catch case where time rolls over, since fetch isn't atomic.
    do time_now = s_time_now;
    until (time_now == s_time_now);
#else
    time_now = apr_time_now ();;
#endif
</method>

<method name = "initialise" private = "0" base = "smt">
    <local>
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
    </local>
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

#    if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
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

</method>

<method name = "terminate" private = "0" base = "smt">
    <local>
#if (defined (BASE_THREADSAFE))
    uint
        os_thread_index,
        schedule_index;
    smt_os_thread_t
        *os_thread,
        *os_thread_link,
        *os_thread_next;
#endif
    </local>
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
</method>

</class>
