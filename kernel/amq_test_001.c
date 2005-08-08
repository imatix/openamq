/*===========================================================================

    OpenAMQ test program - 001

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
    "  -c clientname    Client identifier (default: 'test client')\n"       \
    "  -s server        Name or address of server (localhost)\n"            \
    "  -n number        Number of messages to send/receive (1)\n"           \
    "  -b batch         Size of each batch (100)\n"                         \
    "  -x size          Size of each message (default = 1024)\n"            \
    "  -r repeat        Repeat test N times (1)\n"                          \
    "  -E exchange      Name of exchange to work with (EXCHANGE)\n"         \
    "  -D destination   Destination to publish to(QUEUE)\n"                 \
    "  -Q queue         Queue to consume from (QUEUE)\n"                    \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -a               Use async consumers (default is browsing)\n"        \
    "  -m               Publish mandatory (default is no)\n"                \
    "  -i               Publish immediate (default is no)\n"                \
    "  -p               Use persistent messages (no)\n"                     \
    "  -d               Delayed mode; sleeps after receiving a message\n"   \
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
        quiet_mode = FALSE,             //  -q means suppress messages
        delay_mode = FALSE;             //  -d means work slowly
    char
        *opt_client,                    //  Client identifier
        *opt_server,                    //  Host to connect to
        *opt_trace,                     //  0-3
        **argparm;                      //  Argument parameter to pick-up
    amq_client_connection_t
        *connection;                    //  Current connection
    amq_client_session_t
        *session;                       //  Current session
    amq_content_basic_t
        *content;                       //  Message content
    dbyte
        ticket = 0;                     //  Access ticket
    ipr_xml_t
        *xml_root,                      //  OTL structure
        *xml_object,
        *xml_field;
    char
        *test_data;                     //  Test message data

    //  These are the arguments we may get on the command line
    opt_client   = "test client";
    opt_server   = "localhost";
    opt_trace    = "0";

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
                case 'c':
                    argparm = &opt_client;
                    break;
                case 's':
                    argparm = &opt_server;
                    break;
                case 't':
                    argparm = &opt_trace;
                    break;

                //  These switches have an immediate effect
                case 'q':
                    quiet_mode = TRUE;
                    break;
                case 'd':
                    delay_mode = TRUE;
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
        icl_console_print ("Argument missing - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    else
    if (!args_ok) {
        icl_console_print ("Invalid arguments - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    icl_system_initialise (argc, argv);

    connection = amq_client_connection_new (
        opt_server, opt_client, "guest", "guest", atoi (opt_trace));
    if (!connection) {
        icl_console_print ("Could not connect to server");
        goto finished;
    }
    session = amq_client_session_new (connection, "/", 0, NULL);
    if (!session) {
        icl_console_print ("Could not open session to server");
        goto finished;
    }

    //  Build an OTL test message
    xml_root   = ipr_xml_new (NULL,       "otl",    "");
    xml_object = ipr_xml_new (xml_root,   "object", "");
    ipr_xml_attr_set (xml_object, "name", "property");

    xml_field  = ipr_xml_new (xml_object, "field",  "name");
    ipr_xml_attr_set (xml_field, "name", "name");
    ipr_xml_unlink (&xml_field);
    
    xml_field  = ipr_xml_new (xml_object, "field",  "value");
    ipr_xml_attr_set (xml_field, "name", "value");
    ipr_xml_unlink (&xml_field);
    ipr_xml_unlink (&xml_object);
    test_data = ipr_xml_save_string (xml_root);
    ipr_xml_destroy (&xml_root);

    content = amq_content_basic_new ();
    amq_content_basic_set_body (content, test_data, strlen (test_data) + 1, NULL);
    amq_content_basic_set_destination (content, "propset");
    if (amq_client_session_basic_publish (session, content, ticket, "$system", TRUE, TRUE))
        goto finished;
    amq_content_basic_destroy (&content);
    icl_mem_free (test_data);

    //  Terminate SMT
    smt_os_thread_shut_down ();
    smt_os_thread_wait ();
    
    amq_client_session_destroy    (&session);
    amq_client_connection_destroy (&connection);
    icl_system_destroy ();
    return (0);

    finished:
        return (1);
}

