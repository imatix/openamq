/*===========================================================================
 *
 *  OpenAMQ client test program
 *
 *  Copyright (c) 2007 iMatix Corporation
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

#include "wireapi.h"
#include "version.h"

#define CLIENT_NAME "amqp_client/" VERSION
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
    "  -r repeat        Repeat test N times (1)\n"                          \
    "  -t level         Set trace level (default = 0)\n"                    \
    "  -e exchange      Exchange name (amq.direct)\n"                       \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -d               Use Direct Mode\n"                                  \
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
        direct_mode = FALSE,            //  -d means asynchronous mode
        args_ok = TRUE;                 //  Were the arguments okay?
    char
        *opt_server,                    //  Host to connect to
        *opt_trace,                     //  0-3
        *opt_messages,                  //  Size of test set
        *opt_msgsize,                   //  Message size
        *opt_repeats,                   //  Test repetitions
        *opt_exchange,                  //  Exchange to use
        **argparm;                      //  Argument parameter to pick-up
    amq_client_connection_t
        *connection = NULL;             //  Current connection
    amq_client_session_t
        *session = NULL;                //  Current session
    amq_content_basic_t
        *content = NULL;                //  Message content
    dbyte
        ticket = 0;                     //  Access ticket
    byte
        *test_data = NULL;              //  Test message data
    int
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
    opt_exchange = "amq.direct";

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
                case 't':
                    argparm = &opt_trace;
                    break;
                case 'x':
                    argparm = &opt_msgsize;
                    break;
                case 'r':
                    argparm = &opt_repeats;
                    break;
                case 'e':
                    argparm = &opt_exchange;
                    break;

                //  These switches have an immediate effect
                //  These switches have an immediate effect
                case 'a':
                    icl_console_print ("-a is deprecated, please use -d for direct mode");
                case 'd':
                    direct_mode = TRUE;
                    break;
                case 'v':
                    printf (CLIENT_NAME " - revision " SVN_REVISION "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
#if defined (CCOPTS)
                    printf ("Compiled with: " CCOPTS "\n");
#endif
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

    //  Allocate a test message for publishing
    test_data = icl_mem_alloc (msgsize);
    memset (test_data, 0xAB, msgsize);

    if (atoi (opt_trace) > 2) {
        amq_client_connection_show_animation (TRUE);
        amq_client_session_show_animation (TRUE);
    }
    //  Open the connection
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    connection = amq_client_connection_new (
        opt_server, "/", auth_data, "amq_client", atoi (opt_trace), 30000);
    session = connection? amq_client_session_new (connection): NULL;
    if (!session) {
        icl_console_print ("E: could not connect to %s", opt_server);
        goto finished;
    }
    icl_console_print ("I: opened connection to %s/%s",
        connection->server_product, connection->server_version);
    if (direct_mode)
        connection->direct = TRUE;

    if (amq_client_session_exchange_declare (
        session, ticket, opt_exchange, "direct", 0, 0, 1, 0, NULL))
        goto finished;

    //  Declare exclusive private queue
    if (amq_client_session_queue_declare (
        session, ticket, NULL, FALSE, FALSE, TRUE, TRUE, NULL))
        goto finished;

    if (amq_client_session_queue_bind (
        session, ticket, session->queue, opt_exchange, session->queue, NULL))
        goto finished;

    if (amq_client_session_basic_consume (session,
        ticket,                     //  Access ticket granted by server
        session->queue,             //  Queue name
        NULL,                       //  Client key
        FALSE,                      //  No local messages
        TRUE,                       //  Auto-acknowledge
        TRUE,                       //  Exclusive access to queue
        NULL))                      //  Arguments
        goto finished;

    while (repeats) {
        //  Send messages to server
        icl_console_print ("I: (%d) sending %d messages to server...",
            repeats, messages);

        for (out_count = 0; out_count < messages; out_count++) {
            content = amq_content_basic_new ();
            amq_content_basic_set_body (content, test_data, msgsize, NULL);

            icl_shortstr_fmt (message_id, "ID%09d", out_count);
            amq_content_basic_set_message_id (content, message_id);

            if (amq_client_session_basic_publish (
                session, content, ticket, opt_exchange, session->queue, FALSE, FALSE)) {
                icl_console_print ("E: [%s] could not send message to server - %s",
                    session->queue, session->error_text);
                goto finished;
            }
            amq_content_basic_unlink (&content);
        }
        //  Read messages back from server, discard them
        icl_console_print ("I: (%d) reading messages back from server...", repeats);
        expected = messages;
        while (expected) {
            content = amq_client_session_basic_arrived (session);
            if (content) {
                amq_content_basic_destroy (&content);
                expected--;
            }
            else
            if (expected)
                amq_client_session_wait (session, 1000);

            if (connection->interrupt) {
                icl_console_print ("I: Interrupted - ending test");
                icl_console_print ("I: %d messages not received", expected);
                goto finished;
            }
            if (!session->alive)
                goto finished;
        }
        icl_console_print ("I: received all messages back from server");
        if (repeats > 0)
            repeats--;
    }
    amq_client_session_basic_cancel (session, session->consumer_tag);

    finished:

    amq_client_session_destroy (&session);
    amq_client_connection_destroy (&connection);
    amq_content_basic_destroy (&content);
    icl_longstr_destroy (&auth_data);
    icl_mem_free (test_data);
    icl_system_terminate ();
    return (0);
}

