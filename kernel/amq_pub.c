/*===========================================================================

    OpenAMQ client test program
    Publisher

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
    "  -n number        Number of messages to send/receive (1)\n"           \
    "  -b batch         Size of each batch (100)\n"                         \
    "  -x size          Size of each message (default = 1024)\n"            \
    "  -r repeat        Repeat test N times (1)\n"                          \
    "  -X exchange      Name of exchange to work with (EXCHANGE)\n"         \
    "  -D destination   Destination to publish to(QUEUE)\n"                 \
    "  -Q queue         Queue to consume from (QUEUE)\n"                    \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -a               Use async consumers (default is browsing)\n"        \
    "  -m               Publish mandatory (default is no)\n"                \
    "  -I               Publish immediate (default is no)\n"                \
    "  -i               Show program statistics when ending (no)\n"         \
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
        delay_mode = FALSE,             //  -d means work slowly
        async_mode = FALSE,             //  -a means async consumers
        mandatory = FALSE,              //  -m means publish mandatory
        immediate = FALSE,              //  -I means publish immediate
        showinfo = FALSE,               //  -i means show information
        persistent = FALSE;             //  -p means persistent messages
    char
        *opt_server,                    //  Host to connect to
        *opt_trace,                     //  0-3
        *opt_messages,                  //  Size of test set
        *opt_exchange,                  //  Exchange to work with
        *opt_dest,                      //  Destination to publish to
        *opt_queue,                     //  Queue to consume from
        *opt_batch,                     //  Size of batches
        *opt_msgsize,                   //  Message size
        *opt_repeats,                   //  Test repetitions
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
        count,
        messages,
        batch_left,
        batch_size,
        msgsize,
        repeats;
    ipr_shortstr_t
        message_id;                     //  Message identifier
    char
        *topic;
    char
        *topics [] = {
            "forex.usd",
            "forex.eur",
            "stock.usd",
            "stock.eur"
        };

    //  These are the arguments we may get on the command line
    opt_server   = "localhost";
    opt_trace    = "0";
    opt_messages = "1";
    opt_exchange = "EXCHANGE";
    opt_queue    = "QUEUE";
    opt_dest     =  NULL;               //  Same as queue by default
    opt_batch    = "100";
    opt_msgsize  = "1024";
    opt_repeats  = "1";

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
                case 'b':
                    argparm = &opt_batch;
                    break;
                case 'X':
                    argparm = &opt_exchange;
                    break;
                case 'D':
                    argparm = &opt_dest;
                    break;
                case 'Q':
                    argparm = &opt_queue;
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
                case 'p':
                    persistent = TRUE;
                    icl_console_print ("W: -p option is not yet implemented");
                    break;
                case 'a':
                    async_mode = TRUE;
                    break;
                case 'm':
                    mandatory = TRUE;
                    break;
                case 'I':
                    immediate = TRUE;
                    break;
                case 'i':
                    showinfo = TRUE;
                    break;
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
        icl_console_print ("E: argument missing - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    else
    if (!args_ok) {
        icl_console_print ("E: invalid arguments - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    messages   = atoi (opt_messages);
    batch_size = atoi (opt_batch);
    msgsize    = atoi (opt_msgsize);
    repeats    = atoi (opt_repeats);
    if (batch_size > messages)
        batch_size = messages;
    if (repeats < 1)
        repeats = -1;                   //  Loop forever
    if (opt_dest == NULL)
        opt_dest = opt_queue;

    //  Allocate a test message for publishing
    test_data = icl_mem_alloc (msgsize);
    memset (test_data, 0xAB, msgsize);

    if (atoi (opt_trace) > 2) {
        amq_client_connection_animate (TRUE);
        amq_client_session_animate (TRUE);
    }
    randomize ();

    connection = amq_client_connection_new (
        opt_server,
        amq_client_connection_auth_plain ("guest", "guest"),
        atoi (opt_trace),
        30000);                         //  Timeout
    if (!connection) {
        icl_console_print ("E: could not connect to server");
        goto finished;
    }
    session = amq_client_session_new (connection, "/");
    if (!session) {
        icl_console_print ("E: could not open session to server");
        goto finished;
    }
    //  Declare exchange and queue
    if (amq_client_session_exchange_declare (session,
        ticket, opt_exchange, "dest-wild", FALSE, FALSE, FALSE, FALSE))
        goto finished;

    while (repeats) {
        //  Send messages to the exchange
        icl_console_print ("I: (%d) sending %d messages to server...", repeats, messages);
        batch_left = batch_size;
        for (count = 0; count < messages; count++) {
            content = amq_content_jms_new ();
            ipr_shortstr_fmt (message_id, "ID%d", count);
            amq_content_jms_set_body       (content, test_data, msgsize, NULL);
            amq_content_jms_set_message_id (content, message_id);
            topic = topics [randomof (tblsize (topics))];
            icl_console_print ("I: %s", topic);
            if (amq_client_session_jms_publish (
                    session,
                    content,
                    ticket,
                    opt_exchange,
                    topic,
                    mandatory,
                    immediate)) {
                icl_console_print ("E: Jms.Publish failed - %s", connection->error_text);
                icl_console_print ("E: ending test");
                goto finished;
            }
            if (--batch_left == 0) {
                if (!quiet_mode)
                    icl_console_print ("I: commit batch %d...", count / batch_size);
                batch_left = batch_size;
            }
        }
        if (repeats > 0)
            repeats--;
    }
    finished:

    icl_mem_free (test_data);
    amq_client_session_destroy     (&session);
    amq_client_connection_destroy  (&connection);

    if (showinfo)
        icl_stats_dump ();

    icl_system_destroy ();
    return (0);
}

