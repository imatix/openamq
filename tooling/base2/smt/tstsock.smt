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
<agent name = "tstsock" script = "smt_gen.gsl" >

    <private name = "types">
#define TSTSOCK_PORT     "5001"
#define TSTSOCK_PORT_ERR "5002"

#define LARGE_SIZE 100000
    </private>
    <handler name = "agent init">
    smt_thread_t
        *thread;

    thread = tstsock_main_thread_new ();
    if (thread) {
        smt_thread_wake (thread);
        smt_thread_unlink (&thread);
    }
    </handler>
        
    <thread name="main">
    
        <handler name = "thread init">
    tcb->upbuf   = icl_mem_alloc (BUFFER_MAX);
    tcb->downbuf = icl_mem_alloc (BUFFER_MAX);
    thread->animate = TRUE;
    the_next_event = ok_event;
        </handler>
    <handler name = "thread destroy">
        icl_mem_free (tcb->upbuf);
        icl_mem_free (tcb->downbuf);
        smt_socket_destroy (&master_sock);
        smt_socket_destroy (&slave_sock);
        smt_socket_destroy (&conn_sock);
    </handler>
        
        <context>
    byte *upbuf;
    byte *downbuf;
        </context>
                        
        <state name="after init">
            <event name = "ok" nextstate = "state 2">
                <action name = "test failed connect">
    icl_console_print ("Test failed connect.");
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "free conn socket">
    smt_socket_destroy (&conn_sock);
                </action>

                <!--action name = "test closing passive socket">
    icl_console_print ("Test closing passive socket.");
    master_sock = smt_socket_new ();
    smt_socket_passive (master_sock, thread, TSTSOCK_PORT, NULL, 5);
    smt_socket_request_input (thread, master_sock, 0, SMT_NULL_EVENT);
    smt_socket_request_close (thread, &master_sock, 0, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/-->

                <action name = "test successful connect">
    icl_console_print ("Test successful connect.");
    master_sock = smt_socket_new ();
    smt_socket_passive (master_sock, thread, TSTSOCK_PORT, NULL, 5);
                </action>
                <action name = "report result"/>
                <action>
    smt_socket_request_accept (thread, master_sock, 0, SMT_NULL_EVENT, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "free conn socket"/>
                <action name = "free slave socket">
    smt_socket_destroy (&slave_sock);
                </action>
     
                <!--action name = "test close after input">
    icl_console_print ("Test close after input.");
    smt_socket_request_input (thread, conn_sock, 1000, SMT_NULL_EVENT);
    smt_socket_request_close (thread, &conn_sock, 0, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/-->
     
                <action name = "test sending & receiving data part 1">
    smt_socket_request_accept (thread, master_sock, 0, SMT_NULL_EVENT, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "test sending & receiving data part 2">
    icl_console_print ("Test sending & receiving data.");
    smt_socket_request_read  (thread, conn_sock, 1000, 1, 255, tcb->downbuf, SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 2, (byte *) "Up", SMT_NULL_EVENT);
 
    smt_socket_request_write (thread, slave_sock, 2000, 4, (byte *) "Down", SMT_NULL_EVENT);
    smt_socket_request_read  (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "display buffers"/>
                 
                <action name = "test sending large data">
    static byte
        data [LARGE_SIZE];
        
    icl_console_print ("Test sending large data.");
    smt_socket_request_write (thread, conn_sock, 2000, LARGE_SIZE, data, SMT_NULL_EVENT);
 
    smt_socket_request_read  (thread, slave_sock, 1000, LARGE_SIZE, LARGE_SIZE, data, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                 
                <action name = "test sending with cork">
    icl_console_print ("Test sending with cork.");
    smt_socket_request_write (thread, conn_sock, 2000, 3, (byte *) "One", SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 3, (byte *) "Two", SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 5, (byte *) "Three", SMT_NULL_EVENT);
 
    smt_socket_request_read  (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "display buffers"/>
                 
                <action name = "test timeout">
    icl_console_print ("Test timeout.");
    smt_socket_request_read  (thread, conn_sock, 2000, 255, 255, tcb->upbuf, SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 1000, 5, (byte *) "Start", SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
 
                <action name = "test write then close">
    icl_console_print ("Test write then close.");
    smt_socket_request_write (thread, conn_sock, 2000, 4, (byte *) "Data", SMT_NULL_EVENT);
    smt_socket_request_close (thread, &conn_sock, 5000, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "free slave socket"/>
 
                <action name = "test write to invalid socket">
    icl_console_print ("Test write to invalid socket.");
    slave_sock = smt_socket_new ();
    smt_socket_passive (slave_sock, thread, TSTSOCK_PORT_ERR, NULL, 5);
    smt_socket_request_write (thread, slave_sock, 2000, 4, (byte *) "Data", SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>

                <action name = "test read on broken pipe">
    icl_console_print ("Test read on broken pipe.");
    smt_socket_request_read (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "free slave socket"/>

                <action name = "read then close part 1">
    icl_console_print ("Test read then close.");
    smt_socket_request_accept (thread, master_sock, 0, SMT_NULL_EVENT, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
 
                <action name = "read then close part 2">
    smt_socket_request_read  (thread, conn_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
    smt_socket_request_close (thread, &conn_sock, 5000, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                <action name = "free slave socket"/>
                
                <action name = "input with event part 1">
    icl_console_print ("Input with event.");
    smt_socket_request_accept (thread, master_sock, 0, input_event, &slave_sock);
    conn_sock = smt_socket_new ();
    smt_socket_connect (conn_sock, thread, 0, "127.0.0.1", TSTSOCK_PORT, SMT_NULL_EVENT);
                </action>
                <action name = "report result"/>
                                
            </event>
        </state>
        
        <state name="state 2">
            <event name = "input" nextstate = "state 3">
                <action name = "report result"/>
                
                <action name = "read with event">
    smt_socket_request_read (thread, slave_sock, 1000, 1, 255, tcb->upbuf, read_event);
    smt_socket_request_write (thread, conn_sock, 2000, 4, (byte *) "Data", SMT_NULL_EVENT);
                </action>
            </event>
        </state>
        
        <state name="state 3">
            <event name = "read" nextstate = "state 4">
                <action name = "report result"/>
                
                <action name = "write with event">
    smt_socket_request_read (thread, slave_sock, 1000, 1, 255, tcb->upbuf, SMT_NULL_EVENT);
    smt_socket_request_write (thread, conn_sock, 2000, 4, (byte *) "Data", write_event);
                </action>
            </event>
            
        </state>
        
        <state name="state 4">
            <event name = "write" nextstate = "">
                <action name = "report result"/>
                <action name = "free conn socket"/>
                <action name = "free slave socket"/>
            </event>
        </state>
        
        <state name="defaults">
            <event name = "shutdown">
                <action name = "free conn socket"/>
                <action name = "free slave socket"/>
            </event>
        </state>
    
    </thread>
    
    <private name="functions">
#define BUFFER_MAX      1024

smt_socket_t
    *master_sock = NULL,
    *slave_sock = NULL,
    *conn_sock = NULL;

int
main (int argc, char *argv [])
{
    icl_console_mode (ICL_CONSOLE_DATE, FALSE);
    icl_console_mode (ICL_CONSOLE_TIME, FALSE);
    icl_console_mode (ICL_CONSOLE_THREAD, TRUE);

    icl_system_initialise (argc, argv);
    smt_socket_show_animation         (TRUE);
    smt_socket_request_show_animation (TRUE);
    smt_initialise ();
    tstsock_init ();
    smt_wait (0);
    icl_system_terminate ();
    return (0);
}
    </private>
    
    <action name = "report result">
    icl_console_print ("%s", smt_thread_error (thread));
    </action>

    <action name = "display buffers">
        tcb->upbuf [slave_sock->io_size] = 0;
        icl_console_print ("Up buffer: &lt;%s>", tcb->upbuf);
        tcb->downbuf [conn_sock->io_size] = 0;
        icl_console_print ("Down buffer: &lt;%s>", tcb->downbuf);
    </action>
</agent>
