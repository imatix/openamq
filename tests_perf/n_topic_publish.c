/*===========================================================================
    topic_publish.c - C version of PAL script

    Generated for amq protocol using iMatix ASL/PAL.
    Don't modify this by hand, it's written by a robot that is better at
    writing AMQP clients in C than you are.  See topic_publish.pal for the
    instructions that caused the robot to produce this file.
 *===========================================================================*/

#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                                  \
    "Syntax: program [options...]\n"                                           \
    "Options:\n"                                                               \
    "  -s server        Server:port to connect to (localhost)\n"               \
    "  -t level         Set trace level (0)\n"                                 \
    "                   0=none, 1=low, 2=medium, 3=high\n"                     \
    "  -r count         Repeat script count times, 0=forever (1)\n"            \
    "  -C value         Specify count variable (10000)\n"                      \
    "  -S value         Specify size variable (512)\n"                         \
    "  -L value         Specify log_interval variable (0)\n"                   \
    "  -c               Clock the script (1)\n"                                \
    "  -q               Quiet mode: no messages\n"                             \
    "  -v               Show version information\n"                            \
    "  -h               Show summary of command-line options\n"                \
    "\n"                                                                       \
    "The order of arguments is not important. Switches and filenames\n"        \
    "are case sensitive. See documentation for detailed information.\n"        \
    "\n"

//  Local function prototypes

//  Establish or reestablish connection and session
static int s_establish_session (
    char *opt_server,
    int   trace_level,
    char *virtual_host);

//  Report an error
static void s_report_error (void);

//  Static source-global variables
static amq_client_connection_t
    *s_connection = NULL;               //  Current connection
static amq_client_session_t
    *s_session = NULL;                  //  Current session
static size_t
    s_body_size = 0;                    //  Content body size
static apr_time_t
    s_start_time,                       //  Start time, microseconds
    s_elapsed = 0;                      //  Elapsed time, microseconds

int
main (int argc, char *argv [])
{
    int
        argn;                           //  Argument number
    Bool
        args_ok = TRUE;                 //  Were the arguments okay?
    char
        **argparm;                      //  Argument parameter to pick-up
    char
        *opt_trace = "0",               //  Trace level argument
        *opt_repeat = "1",              //  Repeat count argument
        *server_name = "localhost";     //  Host to connect to
    Bool
        opt_clock = TRUE;               //  Clock the script?
    int
        trace_level,                    //  Trace level
        repeat_count;                   //  Repeat count
    long
        send_count = 0,                 //  Total messages sent
        recv_count = 0;                 //  Total messages received
    int64_t
        send_bytes = 0,                 //  Total bytes sent
        recv_bytes = 0;                 //  Total bytes received
    ipr_stat_t
        *latencies;                     //  Message latency statistics
    char
        *opt_count = "10000";           //  Default setting for count
    char
        *opt_size = "512";              //  Default setting for size
    char
        *opt_log_interval = "0";        //  Default setting for log_interval
    amq_content_basic_t
        *basic_content = NULL;          //  Last content sent/recieved
    int
        id = 0;                         //  Integer variable
    int
        total_sent = 0;                 //  Integer variable
    int
        sent = 0;                       //  Integer variable
    int
        count = 0;                      //  Integer variable
    int
        size = 0;                       //  Integer variable
    int
        log_interval = 0;               //  Integer variable
    int
        content_processed = 0;
    int
        main_rc = 0;                    //  Main return code
    int
        rc;                             //  Return code from API call

    //  Avert grumpy compiler warnings for unused variables
    content_processed = 0;

    //  Initialise iCL system
    icl_console_mode (ICL_CONSOLE_QUIET, TRUE);
    icl_system_initialise (argc, argv);
    icl_console_mode (ICL_CONSOLE_QUIET, FALSE);
    s_start_time = apr_time_now ();

    randomize ();
    latencies = ipr_stat_new ();

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
                    argparm = &server_name;
                    break;
                case 't':
                    argparm = &opt_trace;
                    break;
                case 'r':
                    argparm = &opt_repeat;
                    break;
                case 'C':
                    argparm = &opt_count;
                    break;
                case 'S':
                    argparm = &opt_size;
                    break;
                case 'L':
                    argparm = &opt_log_interval;
                    break;

                //  These switches have an immediate effect
                case 'c':
                    opt_clock = TRUE;
                    break;
                case 'q':
                    icl_console_mode (ICL_CONSOLE_QUIET, TRUE);
                    break;
                case 'v':
                    printf ("Copyright (c) 2005 iMatix Corporation\n");
                    printf (NOWARRANTY);
                    printf ("Compiled with: " CCOPTS "\n");
                    goto finished;
                case 'h':
                    printf ("topic_publish\n");
                    printf ("Copyright (c) 2005 iMatix Corporation\n");
                    printf (NOWARRANTY);
                    printf (USAGE);
                    goto finished;

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
        goto exit_failure;
    }
    else
    if (!args_ok) {
        icl_console_print ("E: invalid arguments - use '-h' option for help");
        goto exit_failure;
    }
    trace_level  = atoi (opt_trace);
    repeat_count = atoi (opt_repeat);

    FOREVER {

    count = atoi (opt_count);
    size = atoi (opt_size);
    log_interval = atoi (opt_log_interval);
    sent = 0;
    total_sent = 0;
    icl_console_print ("I: Sending %d messages of %d bytes ...", count, size);

    if (s_establish_session (server_name, trace_level, "/"))
        goto exit_failure;

    //
    //  Create content size=size fill=random
    {
        byte
            *content_data;              //  Content body as memory block
        icl_shortstr_t
            timestamp;                  //  Formatted timestamp value
        asl_field_list_t
            *headers_list = NULL;
        icl_longstr_t
            *headers_table = NULL;

        //  Create new content object
        amq_content_basic_unlink (&basic_content);
        basic_content = amq_content_basic_new ();

        //  Set content properties
        headers_list = asl_field_list_new (NULL);
        amq_content_basic_set_reply_to (basic_content, s_session->queue);
        amq_content_basic_set_message_id (basic_content, "xxx");

        //  Set content body as block of data
        content_data = icl_mem_alloc (size);
        amq_content_basic_set_body (
            basic_content, content_data, size, icl_mem_free);

        s_body_size = (size_t) basic_content->body_size;
        //  Insert timestamps
        icl_shortstr_fmt (timestamp, "%ld", (long) (s_start_time / 1000000));
        asl_field_new_string (headers_list, "X-Timestamp-Base", timestamp);
        icl_shortstr_fmt (timestamp, "%ld", (long) ((apr_time_now () - s_start_time) / 1000));
        asl_field_new_string (headers_list, "X-Timestamp-Delta", timestamp);
        //  Save headers field table
        headers_table = asl_field_list_flatten (headers_list);
        asl_field_list_unlink (&headers_list);
        amq_content_basic_set_headers (basic_content, headers_table);
        icl_longstr_destroy (&headers_table);
    }

    for (id = 0; id < count; id++) {

    //
    //  basic.publish
    rc = amq_client_session_basic_publish (
        s_session,                  //  Session reference
        basic_content,              //  Message content
        0,                          //  access ticket granted by server
        "amq.topic",                //  exchange name
        "test.publish",             //  Message routing key
        0,                          //  indicate mandatory routing
        0);                         //  request immediate delivery

    send_count++;                   //  For transfer speed calculations
    send_bytes += s_body_size;
    if (rc) {
        s_report_error ();
        goto exit_failure;
    }

    total_sent++;
    if (log_interval != 0) {
        sent++;
        if (sent == log_interval) {
            icl_console_print ("I: Sent %d messages", total_sent);
            sent = 0;
        }
    }

    } /* repeat */

    amq_client_session_destroy (&s_session);

    icl_console_print ("I: Sent all messages");

    if (repeat_count && --repeat_count == 0)
        break;

    } /* FOREVER */

    goto finished;
    exit_failure:
        main_rc = EXIT_FAILURE;
    finished:

    //  Stop the clock, we're not doing any more work
    s_elapsed += apr_time_now () - s_start_time;
    s_elapsed /= 1000;                  //  Get into msecs
    if (opt_clock && s_elapsed) {
        send_bytes = send_bytes / 1024;
        recv_bytes = recv_bytes / 1024;
        icl_console_print ("I: elapsed time:%ld msecs", (long) s_elapsed);
        icl_console_print ("I: outgoing messages:%ld (%ld Kbytes)", send_count, (long) send_bytes);
        icl_console_print ("I: incoming messages:%ld (%ld Kbytes)", recv_count, (long) recv_bytes);
        if (send_count || recv_count)
            icl_console_print ("I: total messages:%ld (%ld Kbytes) average:%ld/sec (%ld Kbytes/sec)",
                (long) (send_count + recv_count),
                (long) (send_bytes + recv_bytes),
                (long) (((send_count + recv_count) * 1000) / s_elapsed),
                (long) (((send_bytes + recv_bytes) * 1000) / s_elapsed));
        if (ipr_stat_count (latencies))
            icl_console_print ("I: message latency min=%d max=%d mean=%d dev=%d msecs",
                (int) ipr_stat_min  (latencies),
                (int) ipr_stat_max  (latencies),
                (int) ipr_stat_mean (latencies),
                (int) ipr_stat_dev  (latencies));
    }
    //  Shutdown
    amq_content_basic_unlink (&basic_content);
    amq_client_session_destroy (&s_session);
    amq_client_connection_destroy (&s_connection);

    ipr_stat_destroy       (&latencies);
    icl_system_terminate ();

    return (main_rc);
}


//  Establish connection and session
//
static int
s_establish_session (
    char *server_name,
    int   trace_level,
    char *virtual_host)
{
    icl_longstr_t
        *auth_data;                     //  Login authorisation

    //  If session or connection died, clean-up
    if (s_session && !s_session->alive)
        amq_client_session_destroy (&s_session);
    if (s_connection && !s_connection->alive)
        amq_client_connection_destroy (&s_connection);

    //  Connect if needed
    if (!s_connection) {
        //  Pause the clock, as we're going to wait for the server
        //  to start-up and this could take a few microts...
        s_elapsed += apr_time_now () - s_start_time;
        if (ipr_net_ping (server_name, "5672")) {
            auth_data = amq_client_connection_auth_plain ("guest", "guest");
            s_connection = amq_client_connection_new (
                server_name, virtual_host, auth_data, "pal script", trace_level, 30000);
            icl_longstr_destroy (&auth_data);

        }
        if (s_connection) {
            //  Restart the clock
            s_start_time = apr_time_now ();
            s_connection->silent = TRUE;
        }
        else {
            icl_console_print ("E: could not connect to %s", server_name);
            return (-1);
        }
    }
    if (!s_session) {
        s_session = amq_client_session_new (s_connection);
        if (s_session)
            s_session->silent = TRUE;
        else {
            icl_console_print ("E: could not open session to server");
            amq_client_connection_destroy (&s_connection);
            return (-1);
        }
    }
    return (0);
}

static void
s_report_error (void)
{
    if (s_session->reply_code) 
        icl_console_print ("E: %d - %s", s_session->reply_code, s_session->reply_text);
    else
        icl_console_print ("E: %s", s_session->error_text);
}
