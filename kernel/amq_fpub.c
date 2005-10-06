/*===========================================================================

    OpenAMQ client test program
    Field Header Publisher

    Copyright (c) 1991-2005 iMatix Corporation
 *===========================================================================*/

#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"
#include "version.h"

#define CLIENT_NAME "amqp_client/1.0\n\n"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: program [options...]\n"                                        \
    "Options:\n"                                                            \
    "  -s server        Name or address of server (localhost)\n"            \
    "  -x size          Size of each message (default = 1024)\n"            \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -q               Quiet mode: no messages\n"                          \
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
        quiet_mode = FALSE;             //  -q means suppress messages
    char
        *opt_server,                    //  Host to connect to
        *opt_trace,                     //  0-3
        *opt_msgsize,                   //  Message size
        **argparm;                      //  Argument parameter to pick-up
    amq_client_connection_t
        *connection = NULL;             //  Current connection
    amq_client_session_t
        *session = NULL;                //  Current session
    amq_content_jms_t
        *content = NULL;                //  Message content
    dbyte
        ticket = 0;                     //  Access ticket
    byte
        *test_data = NULL;              //  Test message data
    int
        msgsize;
    icl_longstr_t
        *headers;                       //  Subscribed fields

    //  These are the arguments we may get on the command line
    opt_server   = "localhost";
    opt_trace    = "0";
    opt_msgsize  = "1024";

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
                case 't':
                    argparm = &opt_trace;
                    break;
                case 'x':
                    argparm = &opt_msgsize;
                    break;

                //  These switches have an immediate effect
                case 'q':
                    quiet_mode = TRUE;
                    break;
                case 'v':
                    printf (CLIENT_NAME);
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
                    printf ("Built on: " BUILDDATE "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (CLIENT_NAME);
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
    //  Allocate a test message for publishing
    msgsize = atoi (opt_msgsize);
    test_data = icl_mem_alloc (msgsize);
    memset (test_data, 0xAB, msgsize);

    if (atoi (opt_trace) > 2) {
        amq_client_connection_animate (TRUE);
        amq_client_session_animate (TRUE);
    }
    connection = amq_client_connection_new (
        opt_server, "/",
        amq_client_connection_auth_plain ("guest", "guest"),
        atoi (opt_trace),
        30000);                         //  Timeout
    if (!connection) {
        icl_console_print ("E: could not connect to server");
        goto finished;
    }
    session = amq_client_session_new (connection);
    if (!session) {
        icl_console_print ("E: could not open session to server");
        goto finished;
    }
    content = amq_content_jms_new ();
    headers = asl_field_list_build ("currency", "eur", "stock", "ibm", NULL);
    amq_content_jms_set_body       (content, test_data, msgsize, NULL);
    amq_content_jms_set_message_id (content, "ID-1");
    amq_content_jms_set_headers    (content, headers);
    amq_client_session_jms_publish (session,
        content, ticket, "match", "stocks", FALSE, FALSE);
    icl_longstr_destroy (&headers);

    content = amq_content_jms_new ();
    headers = asl_field_list_build ("currency", "usd", "stock", "ibm", NULL);
    amq_content_jms_set_body       (content, test_data, msgsize, NULL);
    amq_content_jms_set_message_id (content, "ID-2");
    amq_content_jms_set_headers    (content, headers);
    amq_client_session_jms_publish (session,
        content, ticket, "match", "stocks", FALSE, FALSE);
    icl_longstr_destroy (&headers);

    content = amq_content_jms_new ();
    headers = asl_field_list_build ("currency", "eur", "stock", "ibm", "realtime", "yes", NULL);
    amq_content_jms_set_body       (content, test_data, msgsize, NULL);
    amq_content_jms_set_message_id (content, "ID-3");
    amq_content_jms_set_headers    (content, headers);
    amq_client_session_jms_publish (session,
        content, ticket, "match", "stocks", FALSE, FALSE);
    icl_longstr_destroy (&headers);

    finished:
    icl_mem_free (test_data);
    amq_client_session_destroy    (&session);
    amq_client_connection_destroy (&connection);

    icl_system_terminate ();
    return (0);
}

