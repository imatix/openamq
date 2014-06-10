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
 *  smt_demo_server - A demonstration of an SMT server
 *                    Accepts one type of request - reverse, which takes
 *                    two string arguments.  Returns same two arguments,
 *                    sorted into alphabetical order.
 *  ------------------------------------------------------------------------->

<agent name = "smt_demo_server" script = "smt_gen.gsl">

    <method name = "sort">
        <argument name = "reply_thread" type = "smt_thread_t *"/>
        <argument name = "reply_event"  type = "smt_event_t"/>
        <argument name = "string 1" type = "char *"/>
        <argument name = "string 2" type = "char *"/>
        <possess>
        string_1 = icl_mem_strdup (string_1);
        string_2 = icl_mem_strdup (string_2);
        </possess>
        <release>
        icl_mem_free (string_1);
        icl_mem_free (string_2);
        </release>
    </method>
    <method name = "destroy" event = "destroy"/>        

    <message name = "sort response">
        <argument name = "string 1" type = "char *"/>
        <argument name = "string 2" type = "char *"/>
        <possess>
        string_1 = icl_mem_strdup (string_1);
        string_2 = icl_mem_strdup (string_2);
        </possess>
        <release>
        icl_mem_free (string_1);
        icl_mem_free (string_2);
        </release>
    </message>

    <thread name = "main">

        <context>
    int dummy;
        </context>

        <state name = "start">
            <method name = "sort">
                <action name = "sort two strings"/>
                <action name = "reply sorted strings"/>
            </method>
            <event name = "destroy" nextstate = "" />
        </state>

    </thread>

    <action name = "sort two strings">
    char *swap;
    
    if (strcmp (sort_m->string_1,
                sort_m->string_2) > 0) {
        swap = sort_m->string_1;
        sort_m->string_1 =
            sort_m->string_2;
        sort_m->string_2 = swap;
    }
    </action>

    <action name = "reply sorted strings">
    smt_demo_server_sort_response_send (    
        sort_m->reply_thread->reply_queue,
        sort_m->reply_event,
        sort_m->string_1,
        sort_m->string_2);
    </action>
    
</agent>
