/*===========================================================================

    Demo client main program

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
 *===========================================================================*/

#include "asl.h"
#include "demo_client_connection.h"
#include "demo_client_session.h"
#include "version.h"

#define CLIENT_NAME "demo_cli/1.0"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: program [options...]\n"                                        \
    "Options:\n"                                                            \
    "  -s server        Name or address of server (localhost)\n"            \
    "  -n number        Number of messages to send/receive (1)\n"           \
    "  -x size          Size of each message (default = 1024)\n"            \
    "  -r repeat        Repeat test N times (1)\n"                          \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -d               Delayed mode; sleeps after receiving a message\n"   \
    "  -a               Asynchronous mode; for testing direct protocol\n"   \
    "  -q               Quiet mode: no messages\n"                          \
    "  -i               Show program statistics when ending\n"              \
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
        args_ok = TRUE,                 //  Were the arguments okay?
        quiet_mode = FALSE,             //  -q means suppress messages
        delay_mode = FALSE,             //  -d means work slowly
        async_mode = FALSE,             //  -a means asynchronous mode
        show_info  = FALSE;             //  -i means show information
    char
        *opt_server,                    //  Host to connect to
        *opt_trace,                     //  0-3
        *opt_messages,                  //  Size of test set
        *opt_msgsize,                   //  Message size
        *opt_repeats,                   //  Test repetitions
        **argparm;                      //  Argument parameter to pick-up
    demo_client_connection_t
        *connection = NULL;             //  Current connection
    demo_client_session_t
        *session = NULL;                //  Current session
    demo_content_basic_t
        *content;                       //  Message content
    icl_shortstr_t
        queue;                          //  Queue name
    byte
        *test_data = NULL;              //  Test message data
    int
        rc,                             //  Return code from methods
        count,
        messages,
        msgsize,
        repeats;
    icl_longstr_t
        *auth_data;                     //  Authorisation data
    icl_shortstr_t
        message_id;                     //  Message identifier

    //  Initialise basic iCL
    icl_system_initialise (argc, argv);

    //  These are the arguments we may get on the command line
    opt_server   = "localhost";
    opt_trace    = "0";
    opt_messages = "1";
    opt_msgsize  = "1024";
    opt_repeats  = "1";

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

                //  These switches have an immediate effect
                case 'i':
                    show_info = TRUE;
                    break;
                case 'q':
                    quiet_mode = TRUE;
                    break;
                case 'd':
                    delay_mode = TRUE;
                    break;
                case 'a':
                    async_mode = TRUE;
                    break;
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
    messages = atoi (opt_messages);
    msgsize  = atoi (opt_msgsize);
    repeats  = atoi (opt_repeats);
    if (repeats < 1)
        repeats = -1;                   //  Loop forever

    //  Allocate a test message for publishing
    test_data = icl_mem_alloc (msgsize);
    memset (test_data, 0xAB, msgsize);

    auth_data  = demo_client_connection_auth_plain ("guest", "guest");
    connection = demo_client_connection_new (
        opt_server, "/", auth_data, "demo_cli", atoi (opt_trace), 30000);
    icl_longstr_destroy (&auth_data);

    if (connection)
        icl_console_print ("I: connected to %s/%s - %s",
            connection->server_product,
            connection->server_version,
            connection->server_platform);
    else {
        icl_console_print ("E: could not connect to server");
        goto finished;
    }
    if (async_mode)
        connection->direct = TRUE;

    session = demo_client_session_new (connection);
    if (!session) {
        icl_console_print ("E: could not open session to server");
        goto finished;
    }
    randomize ();
    icl_shortstr_fmt (queue, "QUEUE-%04X-%04X", randomof (0xFFFF), randomof (0xFFFF));
    if (demo_client_session_exchange_declare (session, "myexchange", "fanout", FALSE, FALSE, FALSE))
        goto finished;
    if (demo_client_session_queue_declare (session, queue, FALSE, FALSE, TRUE, FALSE))
        goto finished;
    if (demo_client_session_queue_bind (session, queue, "myexchange", NULL))
        goto finished;
    if (async_mode && demo_client_session_basic_consume (session, queue))
        goto finished;

    while (repeats) {
        //  Send messages to the test queue
        icl_console_print ("I: (%d) sending %d messages to server...", repeats, messages);
        for (count = 0; count < messages; count++) {
            content = demo_content_basic_new ();
            icl_shortstr_fmt (message_id, "ID%d", count);
            demo_content_basic_set_body       (content, test_data, msgsize, NULL);
            demo_content_basic_set_message_id (content, message_id);
            rc = demo_client_session_basic_publish (
                session, content, "myexchange", "test routing key");
            demo_content_basic_unlink (&content);
            if (rc)
                break;
        }
        if (!session->alive) {
            icl_console_print ("E: could not send message to server - %s",
                session->error_text);
            goto finished;
        }
        //  Now read messages off the test queue
        icl_console_print ("I: (%d) reading back messages...", repeats);
        while (count) {
            if (!async_mode
            &&  demo_client_session_basic_get (session, queue))
                break;

            content = demo_client_session_basic_arrived (session);
            if (content) {
                assert (content->body_size == msgsize);
                if ((delay_mode || messages < 100) && !quiet_mode)
                    icl_console_print ("I: message number %s arrived", content->message_id);
                demo_content_basic_unlink (&content);
                count--;
                if (delay_mode)
                    sleep (1);
            }
            else
            if (async_mode)
                demo_client_session_wait (session, 1000);
            else {
                icl_console_print ("I: %d messages lost in transit", count);
                break;
            }
            if (!session->alive)
                goto finished;
            if (connection->interrupt) {
                icl_console_print ("I: Interrupted - ending test");
                icl_console_print ("I: %d messages not received", count);
                goto finished;
            }
        }
        icl_console_print ("I: completed");
        if (repeats > 0)
            repeats--;
    }
    finished:
    if (session)
        demo_client_session_queue_delete (session, queue, FALSE, FALSE);

    demo_client_session_destroy (&session);
    demo_client_connection_destroy (&connection);

    icl_mem_free (test_data);
    if (show_info)
        icl_stats_dump ();
    icl_system_terminate ();
    return (0);
}
