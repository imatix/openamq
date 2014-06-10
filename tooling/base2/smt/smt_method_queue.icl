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
    name      = "smt_method_queue"
    comment   = "SMT method queue class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the queue container for SMT method queues.
</doc>

<inherit class = "icl_queue_head" >
    <option name = "nullify"         value = "0"/>
    <option name = "prefix"          value = "queue"/>
    <option name = "childname"       value = "smt_method" />
    <option name = "childtype"       value = "smt_method_t" />
    <option name = "count"           value = "1" />
</inherit>
<option name = "links"     value = "1" />
<option name = "alloc"     value = "cache" />

<import class = "smt_method" />

<context>
    smt_method_queue_notify_fn
        *notify;                        //  Callback on insert & destroy       
    void
        *data;                          //  For notify functions             
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *sync_mutex;                    //  Synchronisation mutex
    icl_cond_t
        *sync_cond;                     //  Synchronisation cond'n variable
    volatile qbyte
        sync_wait_cnt;                  //  Number of threads waiting
#else
    Bool
        sync;                           //  Is this a synchronising thread?
#endif
</context>

<public name = "header">
//  Type definitions for notify function                                      

typedef enum {
    SMT_METHOD_QUEUE_INSERT,
    SMT_METHOD_QUEUE_DESTROY
} smt_method_queue_notify_t;

typedef int (smt_method_queue_notify_fn) (void *data, smt_method_queue_notify_t event);
</public>

<method name = "new">
    <doc>
    Creates a simple method queue without synchrononisation methods.
    </doc>
    self->notify = NULL;
    self->data   = NULL;
#if (defined (BASE_THREADSAFE))
    self->sync_mutex    = NULL;
    self->sync_cond     = NULL;
    self->sync_wait_cnt = 0;
#else
    self->sync = FALSE;
#endif
</method>

<method name = "sync new" template = "constructor">
    <doc>
    Creates a method queue with synchrononisation methods (wait and trigger).
    </doc>
    self = $(selfname)_new ();
#if (defined (BASE_THREADSAFE))
    self->sync_mutex = icl_mutex_new ();
    self->sync_cond  = icl_cond_new ();
#else
    self->sync = TRUE;
#endif
</method>

<method name = "wait" template = "function">
    <argument name = "msecs"   type = "qbyte">Timeout for wait</argument>
    <doc>
    Wait until the method queue gets triggered or timeout occurs.

    In a multi-thread environment this means waiting on the thread's
    condition variable.  The trigger method will signal that condition
    variable, allowing this method to terminate.

    In a single-thread environment this means running the SMT engine as long
    as it wants to run.  The trigger method will signal to SMT that it's time
    to stop, allowing this method to terminate.

    The method returns zero if an event arrived or the request timed out
    and -1 if the queue was destroyed.
    </doc>
    <local>
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
    </local>
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
</method>

<method name = "isempty" template = "function">
    <doc>
    Returns true if the list is empty, else returns false.
    </doc>
    <!--inherit name = "rdlock"/-->
        rc = (self->count == 0);
</method>

<method name = "trigger" template = "function">
    <argument name = "event" type = "smt_method_queue_notify_t">What kind of trigger is this?</argument>
    <doc>
    Trigger the queue.  See the description of the 'wait' method.
    </doc>
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
</method>

<method name = "register" template = "function">
    <argument name = "notify" type = "smt_method_queue_notify_fn *">Function to call on arrival</argument>
    <argument name = "data" type = "void *">Data for notify</argument>
    self->notify = notify;
    self->data   = data;
</method>

<method name = "destroy">
    smt_method_queue_trigger (self, SMT_METHOD_QUEUE_DESTROY);
</method>

<method name = "free">
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
</method>

<method name = "selftest">
    <local>
    $(selftype) *self;
    </local>
    self = $(selfname)_new ();
    $(selfname)_destroy (&self);
</method>

</class>
