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
 *  smt_demo_client - A demonstration of an SMT client.
 *  ------------------------------------------------------------------------->

<agent name = "smt_demo_client" script = "smt_gen.gsl">

    <private name = "types" >
#include "smt_demo_server.h"
    </private>
    
    <private name = "functions" >
int main (int argc, char *argv [])
{
    int
        count;
        
    icl_system_initialise (argc, argv);
    icl_console_mode (ICL_CONSOLE_THREAD, TRUE);
    if (argc != 3) {
        icl_console_print ("Syntax: smt_demo_client &lt;string1> &lt;string2>");
        return (1);
    }
    smt_initialise ();
    smt_demo_server_init ();
    for (count = 0; count < 1; count++) {
        smt_demo_client_init (argv [1], argv [2]);
    }
    smt_wait (0);
    
    icl_system_terminate ();
    return (0);
}
    </private>
    
    <handler name = "agent init">
        <argument name = "string_1" type = "char *" />
        <argument name = "string_2" type = "char *" />
    smt_thread_t * client_thread;
    smt_demo_client_main_context_t * client_context;
    
    smt_demo_client_animate (TRUE);
    smt_demo_server_animate (TRUE);
    
    client_thread = smt_demo_client_main_thread_new ();
    if (client_thread) {
        client_context = client_thread->context;
        client_context->string_1 = string_1;
        client_context->string_2 = string_2;
        smt_thread_set_next_event (client_thread, ok_event);
        smt_thread_wake (client_thread);
        smt_thread_unlink (&client_thread);
    }
    </handler>    
    
    <thread name = "main">

        <context>
    char * string_1;
    char * string_2;
    smt_thread_t *server_thread;
        </context>

        <state name = "start">
            <event name = "ok" nextstate = "after sort">
                <action name = "create server thread"/>
                <action name = "request string sort"/>
            </event>
        </state>
        
        <state name = "after sort">
            <event name = "ok" nextstate = "">
                <action name = "display sorted strings"/>
                <action name = "destroy server thread"/>
            </event>
        </state>

    </thread>

    <action name = "create server thread">
    tcb->server_thread = smt_demo_server_main_thread_new ();
    if (tcb->server_thread)
        smt_thread_wake (tcb->server_thread);
    </action>

    <action name = "request string sort">
    smt_demo_server_sort (tcb->server_thread,
                          thread,
                          ok_event,
                          tcb->string_1,
                          tcb->string_2);
    </action>
    
    <action name = "display sorted strings">
    icl_console_print ("Response: <%s> <%s>", 
              smt_demo_server_sort_response_m->string_1,
              smt_demo_server_sort_response_m->string_2);
    </action>
    
    <action name = "destroy server thread">
        smt_demo_server_destroy (tcb->server_thread);
        smt_thread_unlink       (&tcb->server_thread);
    </action>
    
</agent>
