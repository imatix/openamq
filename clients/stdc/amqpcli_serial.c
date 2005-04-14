/*  -------------------------------------------------------------------------
 *
 *  amqpcli_serial - AMQP/Fast prototype serial client
 *
 *  Sends 1000 messages in a batch and then reads them back in batches
 *  of 100 with a commit after each 100 messages.
 *
 *  Copyright (c) 1991-2005 iMatix Corporation
 *  Copyright (c) 2004-2005 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_sclient.h"
#include "version.h"

#define CLIENT_NAME "Serial test client - OpenAMQ/" VERSION "\n"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
#define USAGE                                                               \
    "Syntax: clientname [options...]\n"                                     \
    "Options:\n"                                                            \
    "  -c clientname    Client identifier (default: 'test client')\n"       \
    "  -s server        Name or address of server (localhost)\n"            \
    "  -m number        Number of messages to send/receive (1000)\n"        \
    "  -b batch         Size of each batch (100)\n"                         \
    "  -x size          Size of each message (default = 1024)\n"            \
    "  -r repeat        Repeat test N times (1)\n"                          \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -p               Use persistent messages (no)\n"                     \
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
        delay_mode = FALSE,             /*  -d means work slowly             */
        persistent = FALSE;             /*  Use persistent messages?         */
    char
        *opt_client,                    /*  Client identifier                */
        *opt_server,                    /*  Host to connect to               */
        *opt_trace,                     /*  0-3                              */
        *opt_messages,                  /*  Size of test set                 */
        *opt_batch,                     /*  Size of batches                  */
        *opt_msgsize,                   /*  Message size                     */
        *opt_repeats,                   /*  Test repetitions                 */
        **argparm;                      /*  Argument parameter to pick-up    */
    amq_sclient_t
        *amq_client;
    dbyte
        in_handle = 0,
        out_handle = 0;
    int
        messages,
        batch_size,
        batch_left,
        msgsize,
        repeats,
        count;
    amq_message_t
        *message;
    ipr_shortstr_t
        identifier;                     /*  Message identifier               */

    /*  These are the arguments we may get on the command line               */
    opt_client   = "test client";
    opt_server   = "localhost";
    opt_trace    = "0";
    opt_messages = "1000";
    opt_batch    = "100";
    opt_msgsize  = "1024";
    opt_repeats  = "1";

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
                case 'm':
                    argparm = &opt_messages;
                    break;
                case 'b':
                    argparm = &opt_batch;
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

                /*  These switches have an immediate effect                  */
                case 'p':
                    persistent = TRUE;
                    break;
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
        coprintf ("Argument missing - type 'amqpcli_serial -h' for help");
        goto failed;
    }
    else
    if (!args_ok) {
        coprintf ("Invalid arguments - type 'amqpcli_serial -h' for help");
        goto failed;
    }

    messages   = atoi (opt_messages);
    batch_size = atoi (opt_batch);
    msgsize    = atoi (opt_msgsize);
    repeats    = atoi (opt_repeats);
    if (batch_size > messages)
        batch_size = messages;
    if (repeats < 1)
        repeats = 1;

    amq_sclient_trace (atoi (opt_trace));

    coprintf ("Connecting to %s...", opt_server);
    amq_client = amq_sclient_new (opt_client, "guest", "guest");

    if (amq_client == NULL
    ||  amq_sclient_connect (amq_client, opt_server, "/test"))
        goto failed;

    in_handle  = amq_sclient_temporary (amq_client, "temp-client", batch_size, FALSE);
    out_handle = amq_sclient_producer  (amq_client, "temp-client");

    while (repeats) {
        /*  Pause consumption on temporary queue                             */
        if (amq_sclient_flow (amq_client, in_handle, TRUE))
            goto aborted;

        coprintf ("(%d) sending %d messages to server...", repeats, messages);
        batch_left = batch_size;
        for (count = 0; count < messages; count++) {
            ipr_shortstr_fmt (identifier, "ID%d", count);
            message = amq_message_new ();
            amq_message_set_persistent (message, persistent);
            amq_message_set_identifier (message, identifier);
            amq_message_testfill       (message, msgsize);
            if (amq_sclient_msg_send (amq_client, out_handle, message))
                goto aborted;
            /*  Commit as we go along                                        */
            if (--batch_left == 0) {
                if (!quiet_mode)
                    coprintf ("Commit batch %d...", count / batch_size);
                amq_sclient_commit (amq_client);
                batch_left = batch_size;
            }
        }
        /*  Commit any messages left over                                    */
        if (batch_left > 0)
            amq_sclient_commit (amq_client);

        coprintf ("(%d) reading back messages...", repeats);
        count = 0;
        if (amq_sclient_flow (amq_client, in_handle, FALSE))
            goto aborted;

        while (count < messages) {
            message = amq_sclient_msg_read (amq_client, 0);
            if (message) {
                if ((delay_mode || messages < 100) && !quiet_mode)
                    coprintf ("Message number %d arrived", amq_client->msg_number);
                amq_message_destroy (&amq_client->msg_object);
                count++;
                if (count % batch_size == 0) {
                    if (!quiet_mode)
                        coprintf ("Acknowledge batch %d...", count / batch_size);
                    if (amq_sclient_msg_ack (amq_client))
                        goto aborted;
                    if (amq_sclient_commit (amq_client))
                        goto aborted;
                }
                if (delay_mode)
                    sleep (1);
            }
            else {
                coprintf ("No message received - aborting");
                goto aborted;
            }
        }
        repeats--;
    }
    /*  Acknowledge any messages left over                                   */
    amq_sclient_msg_ack (amq_client);

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
