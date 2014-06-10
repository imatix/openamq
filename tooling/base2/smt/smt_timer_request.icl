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
    name      = "smt_timer_request"
    comment   = "SMT timer class"
    version   = "1.0"
    script    = "icl_gen"
    >

<doc>
    The smt_timer_request class handles timing requests made either directly
    by application code or indirectly via socket requests with timeouts.
</doc>

<option name = "links"  value = "0"/>
<option name = "rwlock" value = "0"/>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc"   value = "local" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "rwlock" value = "0"/>
    <option name = "alloc"  value = "cache" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "prefix" value = "by_thread"/>
    <option name = "rwlock" value = "0"/>
    <option name = "alloc"  value = "cache" />
</inherit>

<import class = "smt_os_thread"/>
<import class = "smt_thread"/>
<import class = "smt_socket"/>
<import class = "smt_socket_request"/>

<public name = "header" >
#include "base_apr.h"
</public>

<context>
    smt_thread_t         *thread;       //  Thread to wake up                 
    smt_os_thread_t      *os_thread;    //  OS thread holding pollset          
    apr_time_t           time;          //  Expiry time                        
    smt_event_t          event;         //  Event to deliver                   
    smt_socket_request_t *socket_request;   //  Associated socket request      
#if !defined (BASE_THREADSAFE)
    Bool                 expired;       //  For timed SMT execution
#endif
</context>

<method name = "initialise" private = "0"/>

<method name = "new">
    <argument name = "os thread"  type = "smt_os_thread_t *" >OS thread managing this request</argument>
    <argument name = "time"    type = "apr_time_t"     >Time at which to deliver reply</argument>
    <argument name = "event"   type = "smt_event_t"    >Event to deliver on alarm; SMT_NULL_EVENT for simple thread-blocking</argument>
    <doc>
    Sets an alarm for a specific time.  When that time is reached, the event
    is delivered, and/or the thread is woken.
    </doc>
    <declare name = "self" type = "smt_timer_request_t *"      />
    <local>
    smt_timer_request_t 
        *insert_point,
        *prev_point;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    assert (apr_os_thread_current () == os_thread->apr_os_thread);
#endif

    self->thread         = NULL;
    self->os_thread      = os_thread;
    self->time           = time;
    self->event          = event;
    self->socket_request = NULL;
#if !defined (BASE_THREADSAFE)
    self->expired        = FALSE;
#endif
    
    //  Start searching from back of list
    insert_point = NULL;
    prev_point = smt_timer_request_list_last (os_thread->timer_request_list);
    while (prev_point
       && (self->time < prev_point->time)) {
        insert_point = prev_point;
        prev_point = smt_timer_request_list_prev (&prev_point);
    }
    if (insert_point)
        smt_timer_request_list_relink_before (self, insert_point);
    else
        smt_timer_request_list_queue (os_thread->timer_request_list, self);
        
    os_thread->timer_request_count++;
</method>

<method name = "destroy">
    <doc>
    The purge method is for internal use only.
    
    It destroys a timer request but does not wake a sleeping thread.
    </doc>
    self->os_thread->timer_request_count--;

    if (self->event == SMT_NULL_EVENT
    && !self->socket_request
    &&  self->thread) {
        assert (self->thread->pending_count);
        icl_atomic_dec32 (&self->thread->pending_count);
    }
</method>

<method name = "wakeup" return = "request">
    <argument name = "thread"  type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "time"    type = "apr_time_t"     >Time at which to deliver reply</argument>
    <argument name = "event"   type = "smt_event_t"    >Event to deliver on alarm; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "request" type = "smt_timer_request_t *" default = "NULL">The timer request</declare>
    <local>
    int
        rc = SMT_OK;
    </local>
    request = smt_timer_request_new (thread->os_thread, time, event);
    if (request) {
        request->thread = thread;
        if (event == SMT_NULL_EVENT)
            icl_atomic_inc32 (&thread->pending_count);

        smt_timer_request_by_thread_queue (thread->timer_request_list, request);
    }
    else
        rc = SMT_ILLEGAL_ARGUMENT;

    if (rc != SMT_OK) {
        thread->result = rc;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
        thread->catcher (thread);
    }
</method>

<method name = "delay" return = "request">
    <argument name = "thread"  type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "usecs"   type = "int64_t"        >Delay timeout</argument>
    <argument name = "event"   type = "smt_event_t"    >Event to deliver on timeout; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "request" type = "smt_timer_request_t *" default = "NULL">The timer request</declare>
    <doc>
    Set a timer; on timeout, the event is delivered, and/or the thread is woken.
    </doc>    
    <local>
    apr_time_t time;
    </local>
    time = smt_time_now ();
    time += usecs;

    request = smt_timer_request_wakeup (thread, time, event);
</method>

<method name = "expire all" return = "expired">
    <argument name = "os_thread" type = "smt_os_thread_t *" />
    <argument name = "time_now" type = "apr_time_t"/>
    <declare  name = "expired"  type = "Bool"    default = "FALSE" />
    <doc>
    This method detects expired timers, activating their corresponding threads.
    It returns TRUE if a timer was expired.
    
    This method is for internal use only.
    </doc>
    <local>
    smt_timer_request_t
        *request,
        *next_request;
    smt_socket_request_t
        *socket_request;
    smt_event_t
        event;
    smt_thread_t
        *thread;
    </local>
    request = smt_timer_request_list_first (os_thread->timer_request_list);
    while (request
       && (time_now >= request->time)) {
        expired = TRUE;
        next_request = request;
        next_request = smt_timer_request_list_next (&next_request);
        if (request->socket_request) {       //  ie a socket timer      
            //  Take charge of the socket request
            socket_request = request->socket_request;
            //  Set pointer back here to NULL to avoid unwanted recursion.
            socket_request->timer_request = NULL;
            smt_timer_request_destroy (&request);
            
            smt_socket_request_return (&socket_request,
                                       SMT_TIMEOUT, APR_SUCCESS);
        }
        else {                          //  ie self is a timer request         
            thread = request->thread;
            if (request->event == SMT_NULL_EVENT) {
                if (!request->thread) { //  To implement timed SMT operation
#if (defined (BASE_THREADSAFE))
                    smt_timer_request_destroy (&request);
#else
                    //  Don't destroy timer request if this was a timed SMT
                    //  run; it will be destroyed elsewhere.  Just remove it
                    //  from its containers.
                    request->expired = TRUE;
                    smt_timer_request_remove_from_all_containers (request);
                    smt_request_break ();
#endif
                }
                else {
                    smt_timer_request_destroy (&request);
                    if (thread->pending_count == 0)
                        smt_thread_activate (thread);
                }
            }
            else {
                event    = request->event;
                smt_timer_request_destroy (&request);
                smt_method_send (thread->reply_queue,
                                 event,
                                 SMT_PRIORITY_HIGH,
                                 SMT_OK,
                                 NULL,
                                 NULL);
            }
        }
        request = next_request;
    }
</method>

<method name = "first" return = "time">
    <argument name = "os_thread" type = "smt_os_thread_t *" />
    <declare name = "time" type = "apr_time_t" default = "0" />
    <doc>
    This method returns the time when the first timer expires,
    or { 0, 0 } if no timers are set.
    
    This method is for internal use only.
    </doc>
    <local>
    $(selftype)
        *self;
    </local>
    self = smt_timer_request_list_first (os_thread->timer_request_list);
    if (self)
        time = self->time;
</method>    
    
<method name = "selftest" inherit = "none" />

</class>
