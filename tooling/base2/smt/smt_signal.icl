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
    name      = "smt_signal"
    comment   = "SMT signal handling class"
    version   = "1.0b0"
    script    = "icl_gen"
    >
<doc>
    The smt_signal class is used to manage signal handlers in an SMT
    application.  
</doc>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
    <option name = "links"  value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "int" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "by_thread" />
</inherit>

<inherit class = "icl_init" />

<import class = "smt_thread"/>

<context>
    smt_event_t
        event;                          //  The event to deliver on signal
</context>

<private>
smt_thread_t
    *s_signal_thread;                   //  Only one thread may receive signals

smt_signal_table_t
    *s_signal_table;
</private>

<method name = "initialise" private = "0">
    s_signal_thread = NULL;
    s_signal_table  = smt_signal_table_new ();
</method>

<method name = "terminate">
    smt_signal_table_destroy (&s_signal_table);
</method>

<method name = "new" private = "1">
    self->event = SMT_NULL_EVENT;
</method>

<method name = "destroy">
    <local>
    int                                 //  Need to save key value because it
        the_signal = (*self_p)->key;    //  gets lost on removal from table.
    </local>
    if (the_signal)                     //  Will be zero when purging signals
        signal (the_signal, SIG_DFL);
    
    //  If thread no longer has any signals registered, reset s_signal_thread
    if (s_signal_thread
    &&  smt_signal_by_thread_isempty (s_signal_thread->signal_list))
        s_signal_thread = NULL;
</method>

<method name = "register" return = "rc">
    <inherit name = "auto init"/>
    <argument name = "the_signal" type = "int">The signal to register</argument>
    <argument name = "thread" type = "smt_thread_t *">The thread to register the signal with</argument>
    <argument name = "event"  type = "smt_event_t">The event to be sent on signal</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <doc>
    Register a signal handler.  Upon receipt of the_signal, SMT will deliver
    the requested event to the specified thread.  If the signal was already
    registered by a different thread, returns -1.
    </doc>
    <local>
    smt_signal_t
        *self;
    </local>
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
</method>

<method name = "register shut down">
    <inherit name = "auto init"/>
    <argument name = "the_signal" type = "int">The signal to register</argument>
    <doc>
    Register a shutdown signal.  Upon receipt of the_signal, SMT will proceed
    with a graceful shutdown of the application by sending a shutdown event to
    all agents.
    </doc>
    <local>
    smt_signal_t
        *self;
    </local>
    //  Check whether signal is already registered with some thread
    self = smt_signal_table_search (s_signal_table, the_signal);
    if (!self)
        self = smt_signal_new (s_signal_table, the_signal);

    if (self->event != SMT_NULL_EVENT)
        smt_signal_by_thread_remove (self);

    self->event = SMT_NULL_EVENT;
    
    //  And register the signal handler.
    signal (the_signal, smt_signal_handler);
</method>

<method name = "deregister">
    <inherit name = "auto init"/>
    <argument name = "the_signal" type = "int">The signal to deregister</argument>
    <doc>
    Deregister the specified shutdown signal.
    </doc>
    <local>
    smt_signal_t
        *self;
    </local>
    //  Check whether signal is already registered with some thread
    self = smt_signal_table_search (s_signal_table, the_signal);
    if (self)
        smt_signal_destroy (&self);
</method>

<method name = "handler" private = "1">
    <argument name = "the_signal" type = "int" />
    <local>
    smt_signal_t
        *self;
    </local>
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
</method>

<method name = "selftest" >
</method>

</class>
