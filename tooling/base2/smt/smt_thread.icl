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
    name      = "smt_thread"
    comment   = "SMT thread management class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    The smt_thread class manipulates SMT threads.
    For SMT internal use only.
</doc>

<option name = "links" value = "1"/>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc" value = "cache" />
</inherit>
<inherit name = "icl_list_item">
    <option name = "prefix" value = "by_status" />
    <option name = "rwlock" value = "0" />
    <option name = "child_has_links" value = "0"/>
</inherit>

<inherit name = "icl_list_item">
    <option name = "prefix" value = "queue"/>
    <option name = "child_has_links" value = "0"/>
</inherit>

<inherit name = "icl_list_item">
    <option name = "rwlock" value = "1"/>
    <option name = "dont_create_rwlock" value = "1"/>
</inherit>

<inherit class = "icl_init" />

<import class = "icl_trace" />
<import class = "smt_os_thread" />
<import class = "smt_position"/>
<import class = "smt_method"/>
<import class = "smt_timer_request"/>
<import class = "smt_socket"/>
<import class = "smt_socket_request"/>
<import class = "smt_signal"/>

<public name = "header">
//  Macros                                                                     

#define SMT_NULL_STATE       0          //  Thread not initialised

#define SMT_NULL_EVENT       0          //  No event provided
#define SMT_TERM_EVENT      -1          //  Terminate the thread               
#define SMT_TERM_PLACE      (qbyte) -1  //  Last place
#define SMT_TERM_MODULE     (qbyte) -1  //  End of module list

//  Return codes

#define SMT_OK                0
#define SMT_THREAD_ERROR     -1
#define SMT_SLEEPING         -2
#define SMT_FINISHED         -3
#define SMT_BREAK            -4
#define SMT_EMPTY            -5
#define SMT_ILLEGAL_ARGUMENT  1
#define SMT_SELECT_ERROR      2
#define SMT_SOCKET_ERROR      3
#define SMT_TIMEOUT           4
#define SMT_THREAD_INVALID    5
#define SMT_FILEIO_ERROR      6
#define SMT_SOCKET_CLOSING    7

//  Priorities, ordered highest to lowest.
//  JS: I might change the ordering of priorities to make it more intuitive,
//      but the code is simpler with the reverse ordering.

#define SMT_PRIORITY_LAST    SMT_PRIORITY_NORMAL    //  Last defined priority

typedef enum {
    SMT_PRIORITY_HIGH,
    SMT_PRIORITY_NORMAL
} smt_priority_t;

//  Types                                                                      

//  Thread status

#define SMT_THREAD_NEWBORN   0          //  Thread just created
#define SMT_THREAD_SLEEPING  1          //  Thread with nothing happening
#define SMT_THREAD_WAITING   2          //  Thread waiting for attention
#define SMT_THREAD_WAKING    3          //  Thread with monitor and response
#define SMT_THREAD_POLLING   4          //  Thread polling for response
#define SMT_THREAD_PENDING   5          //  Thread asleep pending response
#define SMT_THREAD_ACTIVE    6          //  Thread receiving attention
#define SMT_THREAD_ZOMBIE    7          //  Thread closed down

typedef qbyte smt_thread_id_t;
typedef qbyte smt_state_t;
typedef qbyte smt_place_t;
typedef qbyte smt_event_t;
typedef qbyte smt_module_t;

typedef int (smt_thread_p_fn)  (smt_thread_t **thread);
typedef int (smt_thread_fn)    (smt_thread_t  *thread);

typedef smt_thread_t * (smt_agent_thread_new_fn)  (void);
</public>

<private>
//  Global variables local to this source file.                                

static volatile smt_thread_id_t
    s_last_thread_id   = 0;             //  Incrementing number of last id     
static smt_thread_list_t
    *s_thread_newborn_list;             //  List of newborn threads
    
//  Function prototypes                                                        

static int 
    s_method_queue_trigger (void *data, smt_method_queue_notify_t event);
</private>

<context>
    smt_os_thread_t      *os_thread;        //  OS thread running this thread  
    smt_thread_p_fn      *manager;          //  Agent manager function         
    smt_thread_fn        *catcher;          //  Agent error catch function     
    smt_thread_fn        *destructor;       //  Agent error catch function     
    smt_thread_id_t      id;                //  Thread ID
    smt_priority_t       priority;          //  Thread priority
    smt_poll_tag_t       poll_tag;          //  Poll tag at last thread service
    smt_place_t          place;             //  Current location in dialog     
    smt_module_t         module;            //  Index in module list
    smt_method_t         *method;           //  Last method received
    smt_event_t          _next_event;       //  Next event                     
    smt_event_t          _exception_event;  //  Exception event                
    Bool                 _exception_raised; //  Did an exception occur?        
    Bool                 animate;           //  Animation enabled?             
    Bool                 shutting_down;     //  Thread is shutting down        
    Bool                 shutdown_request;  //  Request thread to shut down
    smt_signal_by_thread_t
                         *signal_list;      //  Signal registered with thread
    volatile Bool        signal_raised;     //  Set when a signal was sent
    smt_event_t          signal_event;      //  Event to send on signal
    volatile qbyte       status;            //  Thread status 
    int                  schedule;          //  Scheduler for this thread
    void                 *context;          //  Thread-specific data           
    int                  result;            //  Result of last operation       
    int                  error;             //  System error number on error   
    smt_socket_request_t *monitor;          //  Monitor socket request         
    qbyte                pending_count;     //  No. io/timer op'ns pending     
    smt_method_queue_t   *reply_queue;      //  Queued replies for thread
    smt_method_list_t    *reply_list;       //  Local sorted list of replies
    volatile qbyte        reply_backlog;    //  Local count of reply backlog
    smt_position_list_t  *position_stack;   //  Stack of dialog positions
    smt_thread_t         *watched_thread;   //  Thread we are watching
    smt_thread_t         *watching_thread;  //  Thread watching us
    smt_timer_request_by_thread_t
                         *timer_request_list;   //  List of timer requests     
    smt_socket_request_by_thread_t
                         *socket_request_list;  //  List of socket requests    
    smt_socket_request_t
                         *write_request;    //  To optimise on polling
    const char           **event_name;      //  Event names for debugging
    icl_trace_t          *trace;
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_t
        *method_same_os_thread_stat,
        *method_other_os_thread_stat;
#endif
</context>

<method name = "new" trace = "self?self->trace:NULL">
    <argument name = "manager" type = "smt_thread_p_fn *" >The thread's manager function</argument>
    <argument name = "catcher" type = "smt_thread_fn *" >The thread's catcher function</argument>
    <argument name = "destructor" type = "smt_thread_fn *" >The thread's destructor function</argument>
    <doc>
    This method is for internal use only.
    </doc>
    <local>
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
#   define BUFFER_LENGTH 64
    char
        buffer[BUFFER_LENGTH];;
    uint
        length = 0;
#endif
    </local>
    self->id = icl_atomic_inc32 (&s_last_thread_id) + 1;
    if (self->id == 0)
        self->id = icl_atomic_inc32 (&s_last_thread_id) + 1;

#if defined (BASE_THREADSAFE)
    self->os_thread           = NULL;
    self->poll_tag            = 0;
#else
    self->os_thread           = smt_os_thread;
    self->poll_tag            = smt_os_thread->last_poll_tag;
#endif
    self->manager             = manager;
    self->catcher             = catcher;
    self->destructor          = destructor;
    self->priority            = SMT_PRIORITY_NORMAL;
    self->place               = 0;
    self->module              = 0;
    self->method              = NULL;
    self->_next_event         = SMT_NULL_EVENT;
    self->_exception_event    = SMT_NULL_EVENT;
    self->_exception_raised   = FALSE;
    self->animate             = TRUE;
    self->shutting_down       = FALSE;
    self->shutdown_request    = FALSE;
    self->signal_list         = NULL;
    self->signal_raised       = FALSE;
    self->signal_event        = SMT_NULL_EVENT;
    self->status              = SMT_THREAD_NEWBORN;
    self->schedule            = SMT_SCHEDULE_NORMAL;
    self->context             = NULL;
    self->result              = 0;
    self->error               = 0;
    self->monitor             = NULL;
    self->pending_count       = 0;
    self->reply_queue         = smt_method_queue_new  ();
    self->reply_list          = smt_method_list_new   ();
    self->reply_backlog       = 0;
    self->position_stack      = smt_position_list_new ();
    self->watched_thread      = NULL;
    self->watching_thread     = NULL;
    self->timer_request_list  = smt_timer_request_by_thread_new  ();
    self->socket_request_list = smt_socket_request_by_thread_new ();
    self->write_request       = NULL;
    self->event_name          = NULL;
    self->trace               = NULL;

#   if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    length = apr_snprintf (buffer, BUFFER_LENGTH, "smt_thread %s", file);
    
    apr_snprintf (buffer + length, BUFFER_LENGTH - length, " method_same_os_thread");
    self->method_same_os_thread_stat = icl_stats_new (buffer);

    apr_snprintf (buffer + length, BUFFER_LENGTH - length, " method_other_os_thread");
    self->method_other_os_thread_stat = icl_stats_new (buffer);
#   endif

    smt_thread_list_queue  (s_thread_newborn_list, self);

    //  Set trigger last so that thread doesn't start before we're ready    
    smt_method_queue_register  (self->reply_queue, s_method_queue_trigger, self);
</method>

<method name = "destroy" trace = "*self_p?(*self_p)->trace:NULL">
    <doc>
    This method is for internal use only.
    </doc>
    <local>
    smt_method_queue_t
        *reply_queue_link;
    </local>
    //  Dump trace if there is one
    if (self->trace)
        icl_trace_dump (self->trace);
        
    if (self->destructor)
        (self->destructor) (self);
        
    icl_atomic_set32 (&self->status, SMT_THREAD_ZOMBIE);
   
    //  Stop watching them
    if (self->watched_thread) {
        self->watched_thread->watching_thread = NULL;
        self->watched_thread = NULL;
    }
    //  Stop watching us
    if (self->watching_thread) {
        self->watching_thread->watched_thread = NULL;
        self->watching_thread = NULL;
    }
    
    //  Get rid of method
    smt_method_destroy (&self->method);

    //  If any signals are registered with us, deregister them.
    smt_signal_by_thread_destroy (&self->signal_list);
    
    smt_position_list_destroy (&self->position_stack);
    
    //  Just destroying the timer request list isn't enough because the
    //  requests will remain in the per-os thread list and still be 
    //  triggered.
    smt_thread_purge (self);
    
    smt_timer_request_by_thread_destroy  (&self->timer_request_list);
    smt_socket_request_by_thread_destroy (&self->socket_request_list);
    
    //  Force reply queue to be destroyed in two stages so that other          
    //  other threads don't try to access a NULL pointer.                      
    
    //  We won't be requiring notification.                                    
    smt_method_queue_register (self->reply_queue, NULL, NULL);

    //  Keep a link to the zombified queue so that other threads can
    //  detect that we're gone.    
    reply_queue_link = smt_method_queue_link (self->reply_queue);
    smt_method_queue_destroy (&reply_queue_link);
    
    smt_method_list_destroy (&self->reply_list);

    smt_os_thread_remove_thread (self);
</method>

<method name = "free" trace = "self?self->trace:NULL">
    //  finally allow the reply queue to be freed.                             
    smt_method_queue_unlink (&self->reply_queue);
    
    //  Save trace destruction until free so it is still available on leak.
    icl_trace_destroy (&self->trace);
</method>

<method name = "share" template = "function">
    <argument name = "share" type = "smt_thread_t *" />
    <doc>
    Implement thread sharing by setting the OS thread of the new thread to
    that of the share thread, assigning its context block, and incrementing
    the context block link count.
    </doc>
    self->os_thread = share->os_thread;
    self->poll_tag  = share->poll_tag;
    self->context   = share->context;
    (* (int *) self->context)++;
</method>

<method name = "watch" template = "function">
    <argument name = "watch" type = "smt_thread_t *" />
    <doc>
    A simple form of load-balancing is performed by allowing a thread to
    "watch" another thread (which must be in the same OS thread).  If the
    watched thread has outstanding socket requests then the sockets will be
    polled regularly when the watching thread received incoming methods.
    
    Note that a thread is always "watching" itself, but that this can be
    insufficient if a different thread is processing incoming socket data.
    
    Note that a thread can only watch one other thread, and can only be watched
    by one other thread.
    </doc>
    //  If we are watching another thread then stop watching it.
    if (self->watched_thread)
        self->watched_thread->watching_thread = NULL;
                
    self->watched_thread = watch;
    watch->watching_thread = self;
</method>

<method name = "show">
    if (self->trace) {
        icl_console_redirect (trace_file);
        icl_trace_dump (self->trace);
        icl_console_redirect (NULL);
    }
</method>

<method name = "clear method queue" template = "function" trace = "self?self->trace:NULL">
    <doc>
    This method takes incoming methods from the reply queue, sorts them by
    priority and inserts them into the reply list.
    
    It may only be called from the thread's OS thread.
    </doc>
    <local>
    smt_method_t
        *method,
        *search,
        *after;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == self->os_thread->apr_os_thread);
#endif
    //  JS: We could improve this in a number of ways...
    //      It isn't even required for a ST build.
    method = smt_method_queue_pop (self->reply_queue);
    
    //  Perform a crude load-balance by polling if there are any incoming
    //  methods.
    //  JS: zombie test could be eliminated by splitting purge from destroy
    //      of thread's socket request list.
    if (method
    &&  self->poll_tag == self->os_thread->last_poll_tag
    && (! smt_socket_request_by_thread_isempty (self->socket_request_list)
    || (  self->watched_thread
    &&  ! self->watched_thread->zombie
    &&  ! smt_socket_request_by_thread_isempty (self->watched_thread->socket_request_list)))) {
        self->os_thread->last_poll_tag++;
        rc = smt_socket_request_poll (self->os_thread, TRUE);
    }
    
    while (method) {
        after = NULL;
        search = smt_method_list_last (self->reply_list);
        //  NB priority order is reversed...
        while (search && method->priority < search->priority) {
            after = search;
            search = smt_method_list_prev (&search);
        }
        if (after)
            smt_method_list_relink_before (method, after);
        else
            smt_method_list_queue (self->reply_list, method);
        self->reply_backlog++;

        smt_method_unlink (&search);
        smt_method_unlink (&method);    

        method = smt_method_queue_pop (self->reply_queue);
    }
</method>

<method name = "method accept" template = "function" trace = "self?self->trace:NULL">
    <doc>
    Accepts an method from a thread's method queue, if there are any.
    The method event is delivered along with its result code (to 
    thread->result).
    
    If there was no method, the thread is sent to sleep.
    
    The method returns SMT_OK or SMT_EMPTY.
    
    This method is called by the dialog manager.  It should not
    be called by application code.
    </doc>
    //  Should never be called if there are pending socket/timer requests  
    assert (self->pending_count == 0);
    
    smt_thread_clear_method_queue (self);
    
    //  Now actually grab the method
    smt_method_destroy (&self->method);
    self->method = smt_method_list_pop (self->reply_list);
    if (self->method) {
        //  Get everything ready to go    
        self->reply_backlog--;
        smt_thread_set_next_event (self, self->method->event);
        self->result = self->method->result;
        if (self->result != SMT_OK)
            self->catcher (self);
    }
    else
        rc = SMT_EMPTY;
</method>

<method name = "reply backlog" template = "function" return = "backlog">
    <declare  name = "backlog" type = "qbyte" default = "0"  >The backlog</declare>
    <doc>
    This method returns the amount of methods left to process in the threadlet's
    reply list.
    </doc>
    rc = 0;
    backlog = icl_atomic_get32 (&self->reply_backlog);
</method>

<method name = "insert into waiting list" template = "function">
    <doc>
    This method insert a threadlet into its OS thread's waiting list according
    to the thread's priority.
    </doc>
    <local>
    smt_os_thread_t *
        os_thread = self->os_thread;
    smt_thread_t *
        insert_point = NULL;
    smt_priority_t
        priority_index;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == os_thread->apr_os_thread);
#endif
    //  Test first that the thread isn't already in the waiting list.
    if (self->by_status_head == NULL) {
        if (self->priority < SMT_PRIORITY_LAST)
            insert_point = os_thread->waiting_list_entry [self->priority];
            
        if (insert_point)
            smt_thread_by_status_relink_before (self, insert_point);
        else
            smt_thread_by_status_queue (os_thread->waiting_list, self);
        
        //  Adjust insertion points for higher priority
        if (self->priority > 0) {
            priority_index = self->priority - 1;
            while (os_thread->waiting_list_entry [priority_index] == NULL
               ||  os_thread->waiting_list_entry [priority_index]->priority > self->priority) {
                os_thread->waiting_list_entry [priority_index] = self;
                if (priority_index > 0)
                    priority_index--;
                else
                    break;
            }
        }
    }
</method>

<method name = "waiting list pop" return = "self">
    <argument name = "os thread" type = "smt_os_thread_t *">The OS thread</argument>
    <declare  name = "self"      type = "smt_thread_t *"   >The threadlet</declare>
    <doc>
    Pops a thread from a waiting list, maintaining the after pointers for rapid
    insertion by priority.
    
    NB: For optimisation reasons, this function returns a thread pointer, but
        NO LINK to the thread object.
    </doc>
    <local>
    smt_thread_t
        *next_thread;
    smt_priority_t
        priority_index;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == os_thread->apr_os_thread);
#endif
    self = smt_thread_by_status_pop (os_thread->waiting_list);
    if (self) {
        next_thread = smt_thread_by_status_first (os_thread->waiting_list);
        
        //  Adjust insertion points for higher priority
        priority_index = self->priority;
        while (priority_index > 0) {
            priority_index--;
            os_thread->waiting_list_entry [priority_index] = next_thread;
        }
    }
</method>

<method name = "wake" template = "function" trace = "self?self->trace:NULL">
    <doc>
    This method is used to force a thread to wake.  It can be used to
    interrupt monitor requests and to force a thread to shut down.
    
    The method may be called from an OS thread other than the one in which
    the thread is being processed.
    </doc>
    <local>
#if (defined (BASE_THREADSAFE))
    smt_os_thread_t
        *os_thread;
#endif
    </local>
#if (defined (BASE_THREADSAFE))
    if (icl_atomic_cas32 (
            &self->status, SMT_THREAD_WAITING, SMT_THREAD_SLEEPING) == SMT_THREAD_SLEEPING
    ||  icl_atomic_cas32 (
            &self->status, SMT_THREAD_WAKING,  SMT_THREAD_POLLING)  == SMT_THREAD_POLLING
    ||  icl_atomic_cas32 (
            &self->status, SMT_THREAD_WAITING, SMT_THREAD_PENDING)  == SMT_THREAD_PENDING) {
        os_thread = self->os_thread;

        smt_thread_queue_queue (os_thread->thread_queue, self);
        smt_os_thread_wake (os_thread);     //  Wake the OS thread
    }
    else
    if (icl_atomic_cas32 (
            &self->status, SMT_THREAD_WAITING, SMT_THREAD_NEWBORN) == SMT_THREAD_NEWBORN) {
        os_thread = self->os_thread;
        if (!os_thread)
            os_thread = smt_os_thread_assign (self, NULL, self->schedule);
        smt_thread_queue_queue (os_thread->thread_queue, self);
        smt_os_thread_wake (os_thread);     //  Wake the OS thread
    }
    else
        rc = -1;
#else        
    if (self->status == SMT_THREAD_NEWBORN
    ||  self->status == SMT_THREAD_SLEEPING) {
        self->status = SMT_THREAD_WAITING;
        
        smt_thread_insert_into_waiting_list (self);
        smt_os_thread_wake (smt_os_thread);     //  Wake the OS thread
    }
    else
    if (self->status == SMT_THREAD_POLLING) {
        self->status = SMT_THREAD_WAKING;
        
        icl_apr_assert (apr_pollset_add (smt_os_thread->waking_pollset, &self->monitor->socket->pollfd));

        //  Threadlet can go straight to waking list
        smt_thread_by_status_queue (smt_os_thread->waking_list, self);
        smt_os_thread_wake (smt_os_thread);     //  Wake the OS thread
    }
    else
    if (self->status == SMT_THREAD_PENDING) {
        self->status = SMT_THREAD_WAITING;
        
        if (self->monitor) {
            icl_apr_assert (apr_pollset_add (smt_os_thread->waking_pollset, &self->monitor->socket->pollfd));
    
            //  Threadlet can go straight to waking list
            smt_thread_by_status_queue (smt_os_thread->waking_list, self);
        }
        else
            smt_thread_insert_into_waiting_list (self);
        
        smt_os_thread_wake (smt_os_thread);     //  Wake the OS thread
    }
    else
        rc = -1;
#endif
</method>

<method name = "activate" template = "function" trace = "self?self->trace:NULL">
    <doc>
    This method is used by smt_socket_request and smt_timer_request to
    immediately activate a thread.  In other words the thread is made
    active regardless of its prior status (except SMT_THREAD_ZOMBIE).

    This method assumes that the thread has already been given work to do.
    
    This method may only be called from the thread's OS thread.
    </doc>
    assert (self->os_thread);
    assert (!self->monitor);
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == self->os_thread->apr_os_thread);
#endif
    //  Test that thread isn't already active; no race condition possible.
    if (self->status != SMT_THREAD_ACTIVE) {
        icl_atomic_set32 (&self->status, SMT_THREAD_WAITING);
        
        smt_thread_by_status_remove (self);             //  From waking list
        smt_thread_insert_into_waiting_list (self);
    }
</method>

<method name = "activity" template = "function" inline = "1" trace = "self?self->trace:NULL">
    rc = (self->_exception_raised                   //  Exception was raised               
       || self->shutdown_request                    //  Shutdown was requested             
       || self->signal_raised                       //  Signal was raised
       || self->_next_event != SMT_NULL_EVENT       //  Already have an event      
       || self->module != SMT_TERM_MODULE           //  Inside action list
       || !smt_method_list_isempty  (self->reply_list)      //  Incoming
       || !smt_method_queue_isempty (self->reply_queue));   //  Incoming
</method>

<method name = "continue" trace = "self?self->trace:NULL">
    <doc>
    This method works out what to do with a thread after it has been active.
    
    It may only be called from the thread's OS thread.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <local>
    smt_method_t
        *method;
    smt_priority_t
        priority;
    </local>
    assert (self->os_thread);
#if (defined (BASE_THREADSAFE))
#   if (defined (DEBUG))
    assert (apr_os_thread_current () == self->os_thread->apr_os_thread);
#   endif
    //  If thread is already queued, leave it be.
    if (!self->by_status_head) {
        //  If thread is pending then let it go
        if (self->pending_count
        && !self->shutdown_request          //  Shutdown overrides pending
        && !self->signal_raised)            //  Signal overrides pending
            //  Pending can only be resolved from same OS thread, so no race here
            icl_atomic_cas32 (&self->status, SMT_THREAD_PENDING, SMT_THREAD_ACTIVE);
        else
        if (smt_thread_activity (self)) {
            if (self->module == SMT_TERM_MODULE
            &&  self->monitor) {
                //  Monitor can only be resolved from same OS thread, so no race
                
                //  Check for priority on incoming method
                smt_thread_clear_method_queue (self);
                method = smt_method_list_first (self->reply_list);
                priority = method ? method->priority : SMT_PRIORITY_NORMAL;
                smt_method_unlink (&method);
                
                //  Note that priority order is reversed...
                if (priority < SMT_PRIORITY_NORMAL) {
                    if (icl_atomic_cas32 (
                        &self->status, SMT_THREAD_WAITING, SMT_THREAD_ACTIVE) == SMT_THREAD_ACTIVE) {
                        smt_socket_request_destroy (&self->monitor);
                        smt_thread_insert_into_waiting_list (self);
                    }
                }
                else {
                    if (icl_atomic_cas32 (
                        &self->status, SMT_THREAD_WAKING, SMT_THREAD_ACTIVE) == SMT_THREAD_ACTIVE) {
                        smt_thread_by_status_queue (self->os_thread->waking_list, self);
                        icl_apr_assert (apr_pollset_add (self->os_thread->waking_pollset, &self->monitor->socket->pollfd));
                    }
                }
            }
            else {
                if (icl_atomic_cas32 (
                    &self->status, SMT_THREAD_WAITING, SMT_THREAD_ACTIVE) == SMT_THREAD_ACTIVE)
                    smt_thread_insert_into_waiting_list (self);
            }
        }
        else {
            if (self->module == SMT_TERM_MODULE 
            &&  self->monitor) {
                //  Race condition can occur if an incoming event arrives now
                if (icl_atomic_cas32 (
                    &self->status, SMT_THREAD_POLLING, SMT_THREAD_ACTIVE) == SMT_THREAD_ACTIVE) {
                    if (smt_thread_activity (self))
                        //  Catch race condition here
                        if (icl_atomic_cas32 (&self->status, SMT_THREAD_WAKING, SMT_THREAD_POLLING) == SMT_THREAD_POLLING) {
                            smt_thread_by_status_queue (self->os_thread->waking_list, self);
                            icl_apr_assert (apr_pollset_add (self->os_thread->waking_pollset, &self->monitor->socket->pollfd));
                        }
                }
            }
            else {
                //  Race condition can occur if an incoming event arrives now
                if (icl_atomic_cas32 (
                    &self->status, SMT_THREAD_SLEEPING, SMT_THREAD_ACTIVE) == SMT_THREAD_ACTIVE) {
                    if (smt_thread_activity (self))
                        //  Catch race condition here
                        if (icl_atomic_cas32 (&self->status, SMT_THREAD_WAITING, SMT_THREAD_SLEEPING) == SMT_THREAD_SLEEPING)
                            smt_thread_insert_into_waiting_list (self);
                }
            }
        }
    }
#else
    if (self->pending_count
    && !self->shutdown_request          //  Shutdown overrides pending
    && !self->signal_raised) {          //  Signal overrides pending
        icl_atomic_set32 (&self->status, SMT_THREAD_PENDING);
        smt_thread_by_status_remove (self);
    }
    else
    if (smt_thread_activity (self)) {
        if (self->module == SMT_TERM_MODULE
        &&  self->monitor) {
            //  Check for priority on incoming method
            smt_thread_clear_method_queue (self);
            method = smt_method_list_first (self->reply_list);
            priority = method ? method->priority : SMT_PRIORITY_NORMAL;
            smt_method_unlink (&method);
            
            //  Note that priority order is reversed...
            if (priority < SMT_PRIORITY_NORMAL) {
                smt_socket_request_destroy (&self->monitor);
                if (self->status == SMT_THREAD_ACTIVE) {
                    self->status = SMT_THREAD_WAITING;
                    smt_thread_insert_into_waiting_list (self);
                }
            }
            else {
                if (self->status == SMT_THREAD_ACTIVE) {
                    self->status = SMT_THREAD_WAKING;
                    smt_thread_by_status_queue (smt_os_thread->waking_list, self);
                    icl_apr_assert (apr_pollset_add (self->os_thread->waking_pollset, &self->monitor->socket->pollfd));
                }
            }
        }
        else {
            if (self->status == SMT_THREAD_ACTIVE) {
                self->status = SMT_THREAD_WAITING;
                smt_thread_insert_into_waiting_list (self);
            }
        }
    }
    else {
        if (self->module == SMT_TERM_MODULE
        &&  self->monitor) {
            if (self->status == SMT_THREAD_ACTIVE)
                self->status = SMT_THREAD_POLLING;
        }
        else
            if (self->status == SMT_THREAD_ACTIVE)
                self->status = SMT_THREAD_SLEEPING;
    }
#endif
</method>

<method name = "purge" return = "rc" trace = "self?self->trace:NULL">
    <doc>
    Purges all socket and timer requests for a given thread.
    
    Don't use 'function' template since it needs to be called with a zombie
    object.
    
    This method may only be called from the thread's OS thread and is for
    internal use only.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    smt_socket_request_t
        *socket_request;
    //  We increment pending count so that purging socket and timer
    smt_timer_request_t
        *timer_request;
    </local>
    $(selfname:upper)_ASSERT_SANE (self);
    
    //  Possible that thread is purged before it has even started.
    if (self->os_thread) {
    
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
        assert (apr_os_thread_current () == self->os_thread->apr_os_thread);
#endif

        self->monitor = NULL;
        
        //  Purge socket requests
        socket_request = smt_socket_request_by_thread_first (self->socket_request_list);
        while (socket_request) {
            smt_socket_request_destroy (&socket_request);
            socket_request = smt_socket_request_by_thread_first (self->socket_request_list);
        }
        
        //  Purge timer requests
        timer_request = smt_timer_request_by_thread_first (self->timer_request_list);
        while (timer_request) {
            smt_timer_request_destroy (&timer_request);
            timer_request = smt_timer_request_by_thread_first (self->timer_request_list);
        }
    }
</method>

<method name = "shut down" template = "function">
    <doc>
    Requests thread to shut down.
    </doc>
    <animate name = "id" value = "self->id" format = "%lu"/>
    self->shutdown_request = TRUE;
    smt_thread_wake (self);
</method>

<method name = "shut down all newborn">
    <doc>
    Requests all threads to shut down.
    </doc>
    <local>
#if (defined (BASE_THREADSAFE))
    smt_thread_t
        *thread,
        *next_thread;
#endif
    </local>
    //  Request shut-down all the sleeping threads that haven't been 
    //  assigned an OS thread, ie the newborn threads.  Need to grab next
    //  thread from list before starting the thread because the thread will
    //  become volatile once it has started.
#if (defined (BASE_THREADSAFE))
    thread = smt_thread_list_first (s_thread_newborn_list);
    while (thread) {
        next_thread = smt_thread_link (thread);
        next_thread = smt_thread_list_next (&next_thread);
        if (!thread->os_thread
        ||  thread->list_head != thread->os_thread->thread_list) {
            smt_thread_shut_down (thread);
            smt_thread_wake (thread);       //  To start the thread
        }
        smt_thread_unlink (&thread);
        thread = next_thread;
    }
#endif
</method>

<method name = "position push" trace = "self?self->trace:NULL">
    <argument name = "self" type = "$(selftype) *" />
    <doc>
    This method pushes the current position in the dialog of a thread.
    
    This method is for internal use only.
    </doc>
    <local>
    smt_position_t *position;
    </local>
    position = smt_position_new ();
    position->place  = self->place;
    position->module = self->module;
    smt_position_list_push (self->position_stack, position);
    smt_position_unlink (&position);
</method>

<method name = "position pop" return = "rc" trace = "self?self->trace:NULL">
    <argument name = "self" type = "$(selftype) *" />
    <declare  name = "rc"     type = "int"            default = "SMT_OK"/>
    <doc>
    This method pops the current position in the dialog of a thread.
    
    Returns SMT_OK or SMT_EMPTY.
    
    This method is for internal use only.
    </doc>
    <local>
    smt_position_t *position;
    </local>
    position = smt_position_list_pop (self->position_stack);
    if (position == NULL)
        rc = SMT_EMPTY;
    else {
        self->place  = position->place;
        self->module = position->module;
        smt_position_destroy (&position);
    }
</method>

<method name = "set next event" trace = "self?self->trace:NULL">
    <argument name = "self"  type = "$(selftype) *" />
    <argument name = "event" type = "smt_event_t"   />
    <doc>
    This method sets the next event for a given thread.  Note that it does
    not queue the event, but simply copies the value.
    
    This method may only be called from the thread's OS thread, unless the
    thread has yet to start.
    </doc>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (!self->os_thread || apr_os_thread_current () == self->os_thread->apr_os_thread);
#endif
    self->_next_event = event;
</method>

<method name = "raise exception" trace = "self?self->trace:NULL">
    <argument name = "self"  type = "$(selftype) *" />
    <argument name = "event" type = "smt_event_t"   />
    <doc>
    This method raises an exception for a given thread.
    </doc>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (!self->os_thread || apr_os_thread_current () == self->os_thread->apr_os_thread);
#endif
    self->_exception_raised = TRUE;
    self->_exception_event = event;
</method>

<method name = "exception raised" template = "function" trace = "self?self->trace:NULL">
    <doc>
    This method returns TRUE if the thread had an exception raised; FALSE
    otherwise.
    </doc>
    rc = self->_exception_raised;
</method>

<method name = "error" return = "text">
    <argument name = "self"  type = "$(selftype) *" />
    <declare  name = "text"  type = "char *" default = "buffer" />
    <inherit name = "rdlock"/>
    <local>
    static icl_shortstr_t
        buffer;
    icl_shortstr_t
        remote_buffer;
    </local>
    switch (self->result) {
        case SMT_OK:               
            icl_shortstr_cpy (buffer, "Success");                break;
        case SMT_THREAD_ERROR:
            icl_shortstr_cpy (buffer, "Thread error (SYSTEM)");  break;
        case SMT_SLEEPING:
            icl_shortstr_cpy (buffer, "Thread sleeping (INTERNAL)"); break;
        case SMT_FINISHED:
            icl_shortstr_cpy (buffer, "Thread finished (INTERNAL)"); break;
        case SMT_BREAK:
            icl_shortstr_cpy (buffer, "Break (INTERNAL)");           break;
        case SMT_EMPTY:
            icl_shortstr_cpy (buffer, "Empty stack (INTERNAL)");     break;
        case SMT_ILLEGAL_ARGUMENT:
            icl_shortstr_cpy (buffer, "Illegal Argument");           break;
        case SMT_SELECT_ERROR:
            icl_shortstr_cpy (buffer, "Select error (INTERNAL");     break;
        case SMT_SOCKET_ERROR:
            apr_strerror (self->error, remote_buffer, ICL_SHORTSTR_MAX);
            icl_shortstr_cpy (buffer, "Socket error: ");
            icl_shortstr_cat (buffer, remote_buffer);                break;
        case SMT_SOCKET_CLOSING:
            icl_shortstr_cpy (buffer, "Socket closing");
            if (self->error != APR_SUCCESS) {
                apr_strerror (self->error, remote_buffer, ICL_SHORTSTR_MAX);
                icl_shortstr_cat (buffer, ": ");
                icl_shortstr_cat (buffer, remote_buffer);
            }
            break;
        case SMT_TIMEOUT:
            icl_shortstr_cpy (buffer, "Timeout");                    break;
        case SMT_THREAD_INVALID:
            icl_shortstr_cpy (buffer, "Invalid thread");             break;
        case SMT_FILEIO_ERROR:
            apr_strerror (self->error, remote_buffer, ICL_SHORTSTR_MAX);
            icl_shortstr_cpy (buffer, "File i/o error: ");
            icl_shortstr_cat (buffer, remote_buffer);                break;
        default:
            icl_shortstr_cpy (buffer, "Unknown error");              break;
    }
</method>

<method name = "selftest" inherit = "none"/>

<method name = "initialise" private = "0">
    s_last_thread_id = 0;

    //  Newborn thread list needs to be rwlock-protected, but other thread
    //  lists are thread-local, so thread lists are defined with 
    //  dont_create_rwlock.  Here we create newborn thread list and its rwlock.
    s_thread_newborn_list         = smt_thread_list_new ();
#if (defined (BASE_THREADSAFE))
    s_thread_newborn_list->rwlock = icl_rwlock_new ();
#endif
</method>

<method name = "terminate">
    smt_shut_down ();                   //  So OS threads terminate
    smt_wait (0);                       //  Wait for SMT to shut down

    smt_thread_list_destroy (&s_thread_newborn_list);
</method>

<private name = "footer">
//  Functions                                                                  

static int
s_method_queue_trigger (void *data, smt_method_queue_notify_t event)
{
    smt_thread_t
        *self = data;

    //  Only wake thread if event is insertion.  Other possibility is
    //  destruction which means thread is being destroyed.
    if (event == SMT_METHOD_QUEUE_INSERT)
        return smt_thread_wake (self);
    else
        return 0;
}
</private>

</class>
