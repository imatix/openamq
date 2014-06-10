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
<agent name = "smt_demo_timer" script = "smt_gen.gsl" >

    <private name = "types">
#define TSTSOCK_PORT     "5001"
#define TSTSOCK_PORT_ERR "5002"
    </private>
    
    <handler name = "agent init">
    smt_thread_t
        *thread;

    thread = smt_demo_timer_main_thread_new ();
    thread->animate = TRUE;
    smt_thread_wake (thread);
    smt_thread_unlink (&thread);
    </handler>
        
    <thread name="main">
    
        <context>
    void *dummy;
        </context>
   
        <handler name = "thread init">
            smt_thread_set_next_event (thread, ok_event);
        </handler>
                 
        <state name="after init">
            <event name = "ok" nextstate = "">
                <action name = "request delay">
                    icl_console_print ("Requesting delay of 1 second");
                    smt_timer_request_delay (thread, 1000000, SMT_NULL_EVENT);
                </action>
                <action>
                    icl_console_print ("After delay");
                </action>
            </event>
        </state>
    
    </thread>
    
    <private name="functions">
int
main (int argc, char *argv [])
{
    icl_console_mode (ICL_CONSOLE_DATE, FALSE);
    icl_console_mode (ICL_CONSOLE_TIME, FALSE);
    icl_console_mode (ICL_CONSOLE_THREAD, TRUE);

    icl_system_initialise (argc, argv);
    smt_initialise ();
    smt_demo_timer_init ();
    smt_wait (0);
    icl_system_terminate ();
    return (0);
}
    </private>
    
</agent>
