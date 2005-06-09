/*  -------------------------------------------------------------------------
 *
 *  topic_pub - topic publisher
 *
 *  tests topics and content-based routing
 *
 *  Copyright (c) 1991-2005 iMatix Corporation
 *  Copyright (c) 2004-2005 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_field.h"
#include "amq_sclient.h"
#include "version.h"

#define CLIENT_NAME "Simple producer - OpenAMQ/" VERSION "\n"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
#define USAGE                                                               \
    "Syntax: clientname [options...]\n"                                     \
    "Options:\n"                                                            \
    "  -c clientname    Client identifier (default: 'test client')\n"       \
    "  -s server        Name or address of server (localhost)\n"            \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -q               Quiet mode: no messages\n"                          \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"


static void
s_send_message (
    amq_sclient_t    *client,
    dbyte             handle,
    char             *topic,
    amq_field_list_t *headers)
{
    static int
        count = 0;
    ipr_shortstr_t
        identifier;
    amq_message_t
        *message;

    message = amq_message_new ();
    sprintf (identifier, "message-%05d", ++count);
    amq_message_set_identifier (message, identifier);
    amq_message_set_headers    (message, amq_field_list_flatten (headers));
    amq_message_testfill       (message, 100);
    if (amq_sclient_msg_send (client, handle, message, topic, FALSE))
        exit (0);
}


int
main (int argc, char *argv [])
{
    int
        argn;                           /*  Argument number                  */
    Bool
        args_ok = TRUE,                 /*  Were the arguments okay?         */
        quiet_mode = FALSE;             /*  -q means suppress messages       */
    char
        *opt_client,                    /*  Client identifier                */
        *opt_server,                    /*  Host to connect to               */
        *opt_trace,                     /*  0-3                              */
        **argparm;                      /*  Argument parameter to pick-up    */
    amq_sclient_t
        *amq_client;
    dbyte
        out_handle = 0;
    amq_field_list_t
        *headers1,
        *headers2,
        *headers3;

    /*  These are the arguments we may get on the command line               */
    opt_client   = "test producer";
    opt_server   = "localhost";
    opt_trace    = "0";

    console_send     (NULL, TRUE);
    console_set_mode (CONSOLE_DATETIME);

    argparm = NULL;                     /*  Argument parameter to pick-up    */
    for (argn = 1; argn < argc; argn++) {
        /*  If argparm is set, we have to collect an argument parameter      */
        if (argparm) {
            if (*argv [argn] != '-') {  /*  Parameter can't start with '-'   */
                *argparm = strdupl (argv [argn]);
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
                /*  These switches take a parameter                          */
                case 'c':
                    argparm = &opt_client;
                    break;
                case 's':
                    argparm = &opt_server;
                    break;
                case 't':
                    argparm = &opt_trace;
                    break;

                /*  These switches have an immediate effect                  */
                case 'q':
                    quiet_mode = TRUE;
                    break;
                case 'v':
                    puts (CLIENT_NAME);
                    puts (COPYRIGHT);
                    puts (NOWARRANTY);
                    puts (BUILDMODEL);
                    puts ("Built on: " BUILDDATE);
                    exit (EXIT_SUCCESS);
                case 'h':
                    puts (CLIENT_NAME);
                    puts (COPYRIGHT);
                    puts (NOWARRANTY);
                    puts (USAGE);
                    exit (EXIT_SUCCESS);

                /*  Anything else is an error                                */
                default:
                    args_ok = FALSE;
            }
        }
        else {
            args_ok = FALSE;
            break;
        }
    }
    /*  If there was a missing parameter or an argument error, quit          */
    if (argparm) {
        coprintf ("Argument missing - type 'topic_pub -h' for help");
        goto failed;
    }
    else
    if (!args_ok) {
        coprintf ("Invalid arguments - type 'topic_pub -h' for help");
        goto failed;
    }

    /*  Prepare a few different message headers                              */
    headers1 = amq_field_list_new (NULL);
    amq_field_new_string (headers1, "a1", "1");
    amq_field_new_string (headers1, "a2", "2");
    amq_field_new_string (headers1, "a3", "3");

    headers2 = amq_field_list_new (NULL);
    amq_field_new_string (headers2, "a1", "2");
    amq_field_new_string (headers2, "a2", "2");

    headers3 = amq_field_list_new (NULL);
    amq_field_new_string (headers3, "a3", "3");

    amq_sclient_trace (atoi (opt_trace));
    coprintf ("Connecting to %s...", opt_server);
    amq_client = amq_sclient_new (opt_client, "guest", "guest");
    if (amq_client == NULL
    ||  amq_sclient_connect (amq_client, opt_server, "/test", FALSE))
        goto failed;

    out_handle = amq_sclient_producer (amq_client, AMQP_SERVICE_TOPIC);

    s_send_message (amq_client, out_handle, "forex.usd", headers1);
    s_send_message (amq_client, out_handle, "forex.eur", headers1);
/*
    s_send_message (amq_client, out_handle, "forex.usd", headers2);
    s_send_message (amq_client, out_handle, "forex.usd", headers3);
    s_send_message (amq_client, out_handle, "forex.eur", headers2);
    s_send_message (amq_client, out_handle, "forex.eur", headers3);
    s_send_message (amq_client, out_handle, "stock.usd", headers1);
    s_send_message (amq_client, out_handle, "stock.usd", headers2);
    s_send_message (amq_client, out_handle, "stock.usd", headers3);
    s_send_message (amq_client, out_handle, "stock.eur", headers1);
    s_send_message (amq_client, out_handle, "stock.eur", headers2);
    s_send_message (amq_client, out_handle, "stock.eur", headers3);
*/
    amq_field_list_destroy (&headers1);
    amq_field_list_destroy (&headers2);
    amq_field_list_destroy (&headers3);

    amq_sclient_close   (amq_client, 0);
    amq_sclient_destroy (&amq_client);
    icl_system_destroy ();

    coprintf ("Allocs=%ld frees=%ld\n", icl_mem_allocs (), icl_mem_frees ());
    icl_mem_assert ();
    return (0);

    failed:
        exit (EXIT_FAILURE);
}
