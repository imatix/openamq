/*  -------------------------------------------------------------------------
 *
 *  simple_cons - simple consumer
 *
 *  consumes on dynamic queue 'mytest'
 *  - no-ack, no transaction
 *
 *  Copyright (c) 1991-2005 iMatix Corporation
 *  Copyright (c) 2004-2005 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_sclient.h"
#include "version.h"

#define CLIENT_NAME "Simple consumer - OpenAMQ/" VERSION "\n"
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
    "  -d               Delayed mode; sleeps after receiving a message\n"   \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"


int
main (int argc, char *argv [])
{
    int
        argn;                           /*  Argument number                  */
    Bool
        args_ok = TRUE,                 /*  Were the arguments okay?         */
        quiet_mode = FALSE,             /*  -q means suppress messages       */
        delay_mode = FALSE;             /*  -d means work slowly             */
    char
        *opt_client,                    /*  Client identifier                */
        *opt_server,                    /*  Host to connect to               */
        *opt_trace,                     /*  0-3                              */
        **argparm;                      /*  Argument parameter to pick-up    */
    amq_sclient_t
        *amq_client;
    dbyte
        in_handle = 0;
    amq_message_t
        *message;

    /*  These are the arguments we may get on the command line               */
    opt_client   = "test consumer";
    opt_server   = "localhost";
    opt_trace    = "0";

    console_send     (NULL, TRUE);
    console_capture  ("amqpcli_serial.log", 'w');
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
                case 'd':
                    delay_mode = TRUE;
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
        coprintf ("Argument missing - type 'simple_cons -h' for help");
        goto failed;
    }
    else
    if (!args_ok) {
        coprintf ("Invalid arguments - type 'simple_cons -h' for help");
        goto failed;
    }
    amq_sclient_trace (atoi (opt_trace));

    coprintf ("Connecting to %s...", opt_server);
    amq_client = amq_sclient_new (opt_client, "guest", "guest");

    if (amq_client == NULL
    ||  amq_sclient_connect (amq_client, opt_server, "/test", FALSE))
        goto failed;

    in_handle  = amq_sclient_consumer (amq_client, AMQP_SERVICE_QUEUE,
        "mytest",
        0,                              /*  Prefetch size                    */
        FALSE,                          /*  No-local                         */
        TRUE,                           /*  No-ack                           */
        TRUE,                           /*  Dynamic                          */
        TRUE,                           /*  Exclusive                        */
        NULL);                          /*  Selector                         */

    while (TRUE) {
        message = amq_sclient_msg_read (amq_client, 0);
        if (message) {
            if (!quiet_mode)
                coprintf ("Message number %d arrived", amq_client->msg_number);
            amq_message_destroy (&amq_client->msg_object);
            if (delay_mode)
                sleep (1);
        }
        else {
            coprintf ("No message received - aborting");
            goto aborted;
        }
    }
    amq_sclient_close   (amq_client, 0);
    amq_sclient_destroy (&amq_client);
    icl_system_destroy ();

    coprintf ("Allocs=%ld frees=%ld\n", icl_mem_allocs (), icl_mem_frees ());
    icl_mem_assert ();
    return (0);

    aborted:
        exit (0);
    failed:
        exit (EXIT_FAILURE);
}
