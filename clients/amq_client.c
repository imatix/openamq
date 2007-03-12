/*===========================================================================
 *
 *  OpenAMQ client test program
 *
 *  Copyright (c) 1996-2007 iMatix Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  For information on alternative licensing for OEMs, please contact
 *  iMatix Corporation.
 *
 *===========================================================================*/

#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"
#include "version.h"

#define CLIENT_NAME "amqp_client/1.0"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: program [options...]\n"                                        \
    "Options:\n"                                                            \
    "  -s server[:port] Name or address, port of server (localhost)\n"      \
    "  -n number        Number of messages to send/receive (1)\n"           \
    "  -x size          Size of each message (default = 1024)\n"            \
    "  -a connections   Open N active connections (1)\n"                    \
    "  -p connections   Open N passive connections (0)\n"                   \
    "  -r repeat        Repeat test N times (1)\n"                          \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"     \
    "\n"

int
main (int argc, char *argv [])
{
    int
        argn;                           //  Argument number
    Bool
        args_ok = TRUE;                 //  Were the arguments okay?
    char
        *opt_server,                    //  Host to connect to
        *opt_trace,                     //  0-3
        *opt_messages,                  //  Size of test set
        *opt_msgsize,                   //  Message size
        *opt_repeats,                   //  Test repetitions
        *opt_active,                    //  Active connections
        *opt_passive,                   //  Passive connections
        **argparm;                      //  Argument parameter to pick-up
    amq_client_connection_t
        **a_connections,                //  Active connection table
        **p_connections,                //  Passive connection table
        *connection;                    //  Current connection
    amq_client_session_t
        **a_sessions,                   //  Active session table
        **p_sessions,                   //  Passive session table
        *session;                       //  Current session
    amq_content_basic_t
        *content = NULL;                //  Message content
    dbyte
        ticket = 0;                     //  Access ticket
    byte
        *test_data = NULL;              //  Test message data
    int
        the_index,                      //  Current index into set
        nbr_active,                     //  Active connections
        nbr_passive,                    //  Passive connections
        out_count,                      //  Messages sent
        expected,                       //  Messages expected
        messages,
        msgsize,
        repeats;
    icl_shortstr_t
        message_id;                     //  Message identifier
    icl_longstr_t
        *auth_data;                     //  Authorisation data

    //  These are the arguments we may get on the command line
    opt_server   = "localhost";
    opt_trace    = "0";
    opt_messages = "1";
    opt_msgsize  = "1024";
    opt_repeats  = "1";
    opt_active   = "1";
    opt_passive  = "0";

    //  Initialise system in order to use console.
    icl_system_initialise (argc, argv);

    argparm = NULL;                     //  Argument parameter to pick-up
    for (argn = 1; argn < argc; argn++) {
        //  If argparm is set, we have to collect an argument parameter
        if (argparm) {
            if (*argv [argn] != '-') {  //  Parameter can't start with '-'
                *argparm = argv [argn];
                argparm = NULL;
            }
            else {
                args_ok = FALSE;
                break;
            }
        }
        else
        if (*argv [argn] == '-') {
            switch (argv [argn][1]) {
                //  These switches take a parameter
                case 's':
                    argparm = &opt_server;
                    break;
                case 'n':
                    argparm = &opt_messages;
                    break;
                case 'a':
                    argparm = &opt_active;
                    break;
                case 'p':
                    argparm = &opt_passive;
                    break;
                case 't':
                    argparm = &opt_trace;
                    break;
                case 'x':
                    argparm = &opt_msgsize;
                    break;
                case 'r':
                    argparm = &opt_repeats;
                    break;

                //  These switches have an immediate effect
                case 'v':
                    printf (CLIENT_NAME " - revision " SVN_REVISION "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
                    printf ("Compiled with: " CCOPTS "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (CLIENT_NAME "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (USAGE);
                    exit (EXIT_SUCCESS);

                //  Anything else is an error
                default:
                    args_ok = FALSE;
            }
        }
        else {
            args_ok = FALSE;
            break;
        }
    }
    //  If there was a missing parameter or an argument error, quit
    if (argparm) {
        icl_console_print ("E: argument missing - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    else
    if (!args_ok) {
        icl_console_print ("E: invalid arguments - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    messages   = atoi (opt_messages);
    msgsize    = atoi (opt_msgsize);
    repeats    = atoi (opt_repeats);
    if (repeats < 1)
        repeats = -1;                   //  Loop forever
    nbr_passive = atoi (opt_passive);
    nbr_active  = atoi (opt_active);
    
    //  Allocate a test message for publishing
    test_data = icl_mem_alloc (msgsize);
    memset (test_data, 0xAB, msgsize);

    //  Allocate connection & session tables
    a_connections = icl_mem_alloc (sizeof (void *) * nbr_active);
    p_connections = icl_mem_alloc (sizeof (void *) * nbr_passive);
    a_sessions    = icl_mem_alloc (sizeof (void *) * nbr_active);
    p_sessions    = icl_mem_alloc (sizeof (void *) * nbr_passive);
    memset (a_connections, 0, sizeof (void *) * nbr_active);
    memset (p_connections, 0, sizeof (void *) * nbr_passive);
    memset (a_sessions,    0, sizeof (void *) * nbr_active);
    memset (p_sessions,    0, sizeof (void *) * nbr_passive);
    
    if (atoi (opt_trace) > 2) {
        amq_client_connection_animate (TRUE);
        amq_client_session_animate (TRUE);
    }
    //  Open all connections
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    for (the_index = 0; the_index < nbr_active; the_index++) {
        connection = amq_client_connection_new (
            opt_server, "/", auth_data, "amq_client", atoi (opt_trace), 30000);
        if (connection) {
            a_sessions [the_index] = amq_client_session_new (connection);
            a_connections [the_index] = connection;
        }
        else {
            icl_console_print ("E: could not connect to %s", opt_server);
            goto finished;
        }
    }        
    for (the_index = 0; the_index < nbr_passive; the_index++) {
        connection = amq_client_connection_new (
            opt_server, "/", auth_data, "amq_client", atoi (opt_trace), 30000);
        session = connection? amq_client_session_new (connection): NULL;
        if (session) {
            p_sessions    [the_index] = session;
            p_connections [the_index] = connection;
        }
        else {
            icl_console_print ("E: could not connect to %s", opt_server);
            goto finished;
        }
    }        
    if (nbr_active && a_connections [0])
        icl_console_print ("I: opened %d connection%s to %s/%s",
            nbr_active + nbr_passive,
           (nbr_active + nbr_passive) > 1? "s": "",
            a_connections [0]->server_product,
            a_connections [0]->server_version,
            a_connections [0]->server_platform,
            a_connections [0]->server_information);
    
    //  Declare automatic queues
    for (the_index = 0; the_index < nbr_active; the_index++) {
        session = a_sessions [the_index];
        if (amq_client_session_queue_declare (
            session, ticket, NULL, FALSE, FALSE, TRUE, TRUE, NULL))
            goto finished;
        amq_client_session_basic_consume (session,
            ticket,                     //  Access ticket granted by server
            session->queue,             //  Queue name
            NULL,                       //  Client key
            FALSE,                      //  No local messages
            TRUE,                       //  Auto-acknowledge
            FALSE);                     //  Exclusive access to queue
    }
    while (repeats) {
        //  Send messages to server
        icl_console_print ("I: (%d) sending %d messages to server...",
            repeats, messages);
            
        for (out_count = 0; out_count < messages; out_count++) {
            content = amq_content_basic_new ();
            amq_content_basic_set_body (content, test_data, msgsize, NULL);

            icl_shortstr_fmt (message_id, "ID%d", out_count);
            amq_content_basic_set_message_id (content, message_id);
    
            for (the_index = 0; the_index < nbr_active; the_index++) {
                session = a_sessions [the_index];
                if (amq_client_session_basic_publish (
                    session, content, ticket, "", session->queue, FALSE, FALSE)
                ) {
                    icl_console_print ("E: [%s] could not send message to server - %s",
                        session->queue, session->error_text);
                    goto finished;
                }
            }
            amq_content_basic_unlink (&content);
        }
        //  Read messages back from server, discard them
        expected = messages * nbr_active;
        while (expected) {
            for (the_index = 0; the_index < nbr_active; the_index++) {
                session = a_sessions [the_index];
                content = amq_client_session_basic_arrived (session);
                if (content) {
                    amq_content_basic_unlink (&content);
                    expected--;
                }
                else
                    amq_client_session_wait (session, 1000);

                if (!session->alive)
                    goto finished;
                if (smt_signal_raised) {
                    icl_console_print ("I: SMT signal raised - ending test");
                    icl_console_print ("I: %d messages not received");
                    goto finished;
                }
            }
        }
        if (repeats > 0)
            repeats--;
    }
    for (the_index = 0; the_index < nbr_active; the_index++) {
        session = a_sessions [the_index];
        amq_client_session_basic_cancel (session, session->consumer_tag);
    }
    finished:

    for (the_index = 0; the_index < nbr_active; the_index++) {
        if (a_sessions [the_index])
            amq_client_session_destroy (&a_sessions [the_index]);
        if (a_connections [the_index])
            amq_client_connection_destroy (&a_connections [the_index]);
    }
    for (the_index = 0; the_index < nbr_passive; the_index++) {
        if (p_sessions [the_index])
            amq_client_session_destroy (&p_sessions [the_index]);
        if (p_connections [the_index])
            amq_client_connection_destroy (&p_connections [the_index]);
    }
    amq_content_basic_unlink (&content);
    icl_longstr_destroy (&auth_data);
    icl_mem_free (test_data);
    icl_mem_free (a_connections);
    icl_mem_free (p_connections);
    icl_mem_free (a_sessions);
    icl_mem_free (p_sessions);

    icl_system_terminate ();
    return (0);
}

