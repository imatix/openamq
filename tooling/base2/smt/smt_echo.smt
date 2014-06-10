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
<!--
    smt_echo.smt - Example echo server

    This is an example of a simple SMT application.

    For more details, see the SMT Guide included in the index.txt file
    in this directory.

    To build with animation enabled, do the following (assumes Boom is
    installed):
    
        gsl smt_echo.smt
        c -DBASE_ANIMATE -l smt_echo
-->

<agent 
    name   = "smt_echo" 
    script = "smt_gen.gsl" 
    >

<private name = "types">
#include "smt_socket.h"
</private>

<!--
    Main program
-->
<private name = "functions" >
int main (int argc, char *argv [])
{
    int
        rc;
        
    icl_system_initialise (argc, argv);
    if (argc != 2)
        icl_console_print ("Syntax: smt_echo &lt;service/port>");
    else {
        smt_initialise ();
        
        rc = smt_echo_init (argv [1]);
        smt_wait (0);
        smt_echo_term ();
        smt_wait (0);
    }
    icl_system_terminate ();
    return (0);
}
</private>

<!--  
    Handler called during agent initialization 
-->
<handler name = "agent init">
    <argument name = "port" type = "char *"/>
    smt_thread_t
        *thread;                        //  Master thread
    smt_echo_master_context_t
        *tcb;                           //  Master thread context
        
    smt_echo_animate (TRUE);
    //  Deregister SIGINT default shutdown signal
    smt_signal_deregister (SIGINT);
    //  Create and initialise master thread
    thread = smt_echo_master_thread_new ();
    if (thread) {
        tcb = thread->context;
        //  Create listening socket
        tcb->master_socket = smt_socket_new ();
        if (tcb->master_socket &&
            (smt_socket_passive (tcb->master_socket,
                thread, port, NULL, 5) == 0)) {
            icl_console_print ("I: opened echo port %s", port);
            //  Wake newly created thread and drop reference to it
            smt_thread_wake (thread);
            smt_thread_unlink (&thread);
        } else {
            icl_console_print ("Could not open ECHO port %s", port);
            icl_console_print ("Reason: %s", smt_thread_error (thread));
            smt_thread_destroy (&thread);
            rc = SMT_SOCKET_ERROR;
        }
    }
    else {
        icl_console_print ("Error starting echo agent.");
        rc = SMT_THREAD_INVALID;
    }
</handler>

<!--  
    Defaults state shared by all agent threads 
-->
<state name = "defaults">
    <event name = "shutdown" nextstate = ""  />
    <event name = "exception" nextstate = "" />
    <event name = "socket error" nextstate = "">
        <action name = "report error"/>
    </event>
    <event name = "other error" nextstate = "">
        <action name = "report error"/>
    </event>
</state>

<catch error = "SMT_SOCKET_ERROR"  event = "socket error" />
<catch                             event = "other error" />
    
<!--  
    Report Error
-->
<action name = "report error">
    icl_console_print (smt_thread_error (thread));
</action>

<!--  
    Master thread
-->
<thread name = "master" schedule = "poll">
    <context>
        smt_socket_t 
            *master_socket;             //  Master socket
        smt_socket_t 
            *client_socket;             //  Connected socket
    </context>

    <handler name = "thread init">
        //  Start the FSM
        smt_thread_set_next_event (thread, ok_event);
    </handler>
                        
    <handler name = "thread destroy">
        //  Deallocate socket created during initialisation
        smt_socket_destroy (&tcb->master_socket);
    </handler>

    <state name = "after init">
        <event name = "ok" nextstate = "new connection">
            <action name = "accept connection"/>
        </event>
    </state>

    <state name = "new connection">
        <event name = "ok" nextstate = "new connection">
            <action name = "create slave thread"/>
            <action name = "accept connection"/>
        </event>
    </state>
</thread>

<!--
    Accept Connection
-->
<action name = "accept connection">
    //  Create an ACCEPT request.
    //  SMT will accept new connections on master_socket and send our thread 
    //  an "ok" event on connection.
    smt_socket_request_accept (thread,
                               tcb->master_socket,
                               0,
                               ok_event,
                               &tcb->client_socket);
</action>

<!--  
    Create Slave Thread
-->
<action name = "create slave thread">
    smt_thread_t
        *slave_thread;                  //  Newly created slave thread
    smt_echo_slave_context_t
        *slave_tcb;                     //  And it's context block
    
    //  Create slave thread
    slave_thread = smt_echo_slave_thread_new ();
    if (slave_thread) {
        slave_tcb = slave_thread->context;
        //  Pass client_socket returned by ACCEPT request to slave thread
        slave_tcb->client_socket = tcb->client_socket;
        //  Wake the newly created thread
        smt_thread_wake (slave_thread);
        smt_thread_unlink (&slave_thread);
    }
</action>

<!--  
    Slave thread
-->
<thread name = "slave" schedule = "poll">
    <context>
        smt_socket_t 
            *client_socket;             //  Client socket from master thread 
        byte 
            buffer [4096];              //  Buffer for network data
    </context>
        
    <handler name = "thread init">
        //  Register to receive interrupt_event on SIGINT
        smt_signal_register (SIGINT, thread, interrupt_event);
        //  Start the FSM
        smt_thread_set_next_event (thread, ok_event);
    </handler>
        
    <handler name = "thread destroy">
        //  Deallocate client socket we got from the master thread
        smt_socket_destroy (&tcb->client_socket);
    </handler>

    <state name = "after init">
        <event name = "ok" nextstate = "active connection" >
            <action name = "request read from socket"/>
        </event>
        <event name = "interrupt" nextstate = "">
            <action name = "send interrupted message"/>
        </event>
    </state>

    <state name = "active connection">
        <event name = "ok" nextstate = "active connection" >
            <action name = "write socket data"/>
            <action name = "request read from socket"/>
        </event>
        <event name = "interrupt" nextstate = "">
            <action name = "send interrupted message"/>
        </event>
    </state>
</thread>    

<!--  
    Request read from socket
-->
<action name = "request read from socket">
    //  Create an asynchronous READ request.
    //  Requests that SMT read 1-4096 bytes from client_socket into tcb->buffer
    //  and deliver an "ok" event to us when done.
    smt_socket_request_read (thread, tcb->client_socket, 0,
                             1, 4096, tcb->buffer,
                             ok_event);
</action>

<!--  
    Write socket data
-->
<action name = "write socket data">
    //  Create a synchronous WRITE request.
    //  Requests that SMT write io_size bytes from tcb->buffer to
    //  client_socket.  Blocks the thread until the request completes.
    //  XXX io_size comes from the previously completed socket request
    //  XXX this is confusing, there should be a better way to get this
    //  XXX information.
    smt_socket_request_write (thread, tcb->client_socket, 0, 
                              tcb->client_socket->io_size, tcb->buffer, 
                              SMT_NULL_EVENT);
</action>
   
<!--  
    Send Interrupted Message
-->
<action name = "send interrupted message">
    //  Called on receipt of an "interrupt" event.
    //  Send a "server is shutting down" message to the client.
    #define INTERRUPTED_MESSAGE "Server interrupted!\n"
    smt_socket_request_write (thread, tcb->client_socket, 0, 
                              sizeof (INTERRUPTED_MESSAGE), 
                              (byte *)INTERRUPTED_MESSAGE, 
                              SMT_NULL_EVENT);
</action>
   
</agent>
