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
<!---------------------------------------------------------------------------
 *  iMatix SMT (State Machine Threadlets)
 *
 *  smt_demo_trivial - A demonstration of an SMT client.
 *  ------------------------------------------------------------------------->

<agent name = "smt_demo_trivial" script = "smt_gen.gsl">

    <thread name = "main">

        <context>
    int count;
        </context>

        <state name = "starting">
            <event name = "ok" nextstate = "counting">
                <action name = "increment count"/>
                <action name = "test count value"/>
            </event>
        </state>
        
        <state name = "counting">
            <event name = "ok" nextstate = "counting">
                <action name = "print count value"/>
                <action name = "increment count"/>
                <action name = "test count value"/>
            </event>
            <event name = "done" nextstate = "">
                <action name = "print done"/>
            </event>
        </state>
        
        
        <handler name = "thread init">
            tcb->count = 0;
            the_next_event = ok_event;
        </handler>
        
        <action name = "increment count">
            tcb->count++;
            
        </action>
        
        <action name = "test count value">
            if (tcb->count < 5)
                the_next_event = ok_event;
            else
                the_next_event = done_event;
        </action>

        <action name = "print count value">
            icl_console_print ("Value of count is %i", tcb->count);
        </action>
        
        <action name = "print done">
            icl_console_print ("DONE");
        </action>
        
    </thread>
    
</agent>
