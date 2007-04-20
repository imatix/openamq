/*===========================================================================
 *
 *  FileMQ file publisher
 *
 *  Accepts a single file from the application and sends it to the broker;
 *  expects a confirmation from the cache server.
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

#define CLIENT_NAME "FileMQ-Publisher/0.1"
#define COPYRIGHT "Copyright (c) 1996-2007 iMatix Corporation"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: fmq_pub [options...]\n"                                        \
    "Options:\n"                                                            \
    "  -f filename      Publish single file\n"                              \
    "  -n servicename   Service name (filemq)\n"                            \
    "  -c channel       Channel name to publish to (default)\n"             \
    "  -s server[:port] Name or address, port of server (localhost)\n"      \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -r               Robust behaviour: retry if broker unavailable\n"    \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"     \
    "\n"

#include "fmq_pub.d"                    //  Include dialog data

//- Function prototypes -----------------------------------------------------

static void s_check_status (int rc, char *format, ...);


//- Global variables used in this source file only --------------------------

static int
    feedback = 0;                       //  Feedback for calling program
static char
    *opt_filename,                      //  File to publish, if any
    *opt_service,                       //  FileMQ service name
    *opt_channel,                       //  FileMQ channel name
    *opt_hostname,                      //  Host to connect to
    *opt_trace;                         //  Trace level argument
static amq_client_connection_t
    *connection;                        //  Current connection
static amq_client_session_t
    *session;                           //  Current session
static amq_content_basic_t
    *content;                           //  Current content
static dbyte
    access_ticket = 0;                  //  Access ticket
static Bool
    opt_robust = FALSE;                 //  Retry if broker not available?
static icl_shortstr_t
    exchange_name,                      //  Name of service exchange
    queue_name,                         //  Our private queue
    cache_queue;                        //  Name of cache queue
static int
    rc;                                 //  Return code from WireAPI calls


//*******************************   M A I N   *******************************

int main (int argc, char *argv [])
{
    int
        argn;                           //  Argument number
    Bool
        args_ok = TRUE;                 //  Were the arguments okay?
    char
        **argparm;                      //  Argument parameter to pick-up

    //  These are the arguments we may get on the command line
    opt_filename = "";
    opt_service  = "filemq";
    opt_channel  = "default";
    opt_hostname = "localhost";
    opt_trace    = "0";

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
                case 'f':
                    argparm = &opt_filename;
                    break;
                case 'n':
                    argparm = &opt_service;
                    break;
                case 's':
                    argparm = &opt_hostname;
                    break;
                case 't':
                    argparm = &opt_trace;
                    break;
                //  These switches have an immediate effect
                case 'r':
                    opt_robust = TRUE;
                    break;
                case 'v':
                    printf (CLIENT_NAME "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
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
#   include "fmq_pub.i"                 //  Start dialog engine
}


//************************   INITIALISE THE PROGRAM   ***********************

MODULE initialise_the_program (void)
{
    //  Prepare names for the exchange and queue we use
    icl_shortstr_fmt (exchange_name, "fmq.%s", opt_service);
    icl_shortstr_fmt (cache_queue, "fmq.%s", opt_service);

    if (atoi (opt_trace) > 2) {
        amq_client_connection_animate (TRUE);
        amq_client_session_animate (TRUE);
    }
    icl_console_print ("I: starting up file publisher, service=%s", opt_service);
    the_next_event = ok_event;
}


//**********************   OPEN CONNECTION TO BROKER   **********************

MODULE open_connection_to_broker (void)
{
    icl_longstr_t
        *auth_data;                     //  Login authorisation
    ipr_token_list_t
        *host_list;                     //  List of known hosts
    ipr_token_t
        *token;                         //  Next host to try

    //  The file server implements a simple failover mechanism; it will look
    //  for one of a list of brokers and connect to the first available one.
    //  If the broker dies, the file server pauses for a few seconds and then
    //  re-tries the list.  If it cannot connect to any broker on the list it
    //  exits, or continues to retry, depending on a runtime switch (the -r
    //  robust option).

    assert (!connection && !session);
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    host_list = ipr_token_split (opt_hostname);

    token = ipr_token_list_first (host_list);
    while (token) {
        connection = amq_client_connection_new (
            token->value, "/", auth_data, "fmq_pub", atoi (opt_trace), 30000);
        if (connection) {
            ipr_token_unlink (&token);
            break;
        }
        token = ipr_token_list_next (&token);
    }
    ipr_token_list_destroy (&host_list);
    icl_longstr_destroy (&auth_data);

    if (connection)
        session = amq_client_session_new (connection);
    else {
        icl_console_print ("E: could not connect to %s", opt_hostname);
        raise_exception (no_broker_event);
    }
}


/*************************   DECLARE RESPONSE QUEUE   ************************/

MODULE declare_response_queue (void)
{
    rc = amq_client_session_queue_declare (
        session,                    //  Session reference
        access_ticket,              //  Access ticket
        NULL,                       //  Queue name
        FALSE,                      //  Passive only
        FALSE,                      //  Durable queue
        TRUE,                       //  Exclusive queue
        TRUE,                       //  Auto-delete queue when unused
        NULL                        //  Additional arguments
    );
    s_check_status (rc, "E: can't declare private response queue");
    if (!exception_raised) {
        icl_shortstr_cpy (queue_name, session->queue);
        rc = amq_client_session_basic_consume (
            session,                //  Session reference
            access_ticket,          //  Access ticket
            queue_name,             //  Private queue name
            NULL,                   //  Consumer tag
            FALSE,                  //  Do not deliver own messages
            TRUE,                   //  No acknowledgement needed
            TRUE,                   //  Request exclusive access
            NULL                    //  Additional arguments
        );
        s_check_status (rc, "E: can't consume from response queue '%s'", queue_name);
    }
}

static void
s_check_status (int rc, char *format, ...)
{
    va_list
        argptr;

    if (rc) {
        va_start (argptr, format);
        icl_console_print_v (format, argptr );
        va_end (argptr);
        if (session->reply_code)
            icl_console_print ("E: %d - %s", session->reply_code, session->reply_text);
        else
            icl_console_print ("E: %s", session->error_text);
        raise_exception (exception_event);
    }
}


/**********************   SEND HELLO TO SERVICE CACHE   **********************/

MODULE send_hello_to_service_cache (void)
{
    //  Send message to service cache
    amq_content_basic_destroy (&content);
    content = amq_content_basic_new ();
    amq_content_basic_set_type     (content, "HELLO");
    amq_content_basic_set_reply_to (content, queue_name);

    icl_console_print ("I: looking for FileMQ service '%s'", opt_service);
    rc = amq_client_session_basic_publish (
        session,                     //  Session reference
        content,                     //  Message to sent
        access_ticket,               //  Access ticket
        NULL,                        //  Default exchange
        cache_queue,                 //  Route to cache
        TRUE,                        //  Mandatory routing
        TRUE);                       //  Immediate delivery
    s_check_status (rc, "E: can't send content to broker");
}


/***********************   ASSERT MESSAGE IS HELLO OK   **********************/

MODULE assert_message_is_hello_ok (void)
{
    if (strneq (content->type, "HELLO-OK"))
        raise_exception (bad_message_event);
}


/**************************   DISCARD BAD MESSAGE   **************************/

MODULE discard_bad_message (void)
{
    icl_console_print ("W: ignoring bad message: '%s'", content->type);
}


/***************************   GET OPERATION MODE   **************************/

MODULE get_operation_mode (void)
{
    if (strnull (opt_filename))
        the_next_event = folder_event;
    else
        the_next_event = single_event;
}


/***********************   PUBLISH FILE AS REQUESTED   ***********************/

MODULE publish_file_as_requested (void)
{
    /*
        - open file
        - read buckets of 1Mb from fil
        - content message-id = PUBLISH
        - set
        - publish to exchange, routing-key = channel,
}


/***********************   ASSERT MESSAGE IS CONFIRM   ***********************/

MODULE assert_message_is_confirm (void)
{
}


/*************************   SIGNAL FILE PUBLISHED   *************************/

MODULE signal_file_published (void)
{
}


/************************   CHECK IF RETRIES WANTED   ************************/

MODULE check_if_retries_wanted (void)
{
    if (opt_robust)
        the_next_event = retry_event;
    else
        the_next_event = abandon_event;
}


/***********************   SIGNAL UNEXPECTED RETURNS   ***********************/

MODULE signal_unexpected_returns (void)
{
    icl_console_print ("E: messages returned from broker");
}


/************************   SIGNAL NO SERVICE CACHE   ************************/

MODULE signal_no_service_cache (void)
{
    icl_console_print ("E: file service '%s' is not running", opt_service);
}


//*************************   SIGNAL SHUTTING DOWN   ************************

MODULE signal_shutting_down (void)
{
    icl_console_print ("I: shutting down file publisher");
}


//**********************   CLOSE CONNECTION TO BROKER   *********************

MODULE close_connection_to_broker (void)
{
    amq_content_basic_destroy (&content);
    amq_client_session_destroy (&session);
    amq_client_connection_destroy (&connection);
}


/***************************   PAUSE FOR RECOVERY   **************************/

MODULE pause_for_recovery (void)
{
    icl_console_print ("I: retrying in 5 seconds...");
    sleep (5);
}


//**************************   GET EXTERNAL EVENT   *************************

MODULE get_external_event (void)
{
    assert (session);
    assert (connection);

    FOREVER {
        amq_client_session_wait (session, 1000);
        if (amq_client_session_get_basic_arrived_count (session)) {
            amq_content_basic_destroy (&content);
            content = amq_client_session_basic_arrived (session);
            the_next_event = arrived_event;
            break;
        }
        else
        if (amq_client_session_get_basic_returned_count (session)) {
            amq_content_basic_destroy (&content);
            content = amq_client_session_basic_returned (session);
            the_next_event = returned_event;
            break;
        }
        else
        if (connection->interrupt) {
            //  Our process was killed (Ctrl-C)
            icl_console_print ("I: interrupted");
            the_next_event = interrupt_event;
            break;
        }
        else
        if (!connection->alive && connection->reply_code == 100) {
            //  Remote server closed on us
            icl_console_print ("I: AMQP broker dropped connection");
            the_next_event = no_broker_event;
            break;
        }
        else
        if (!connection->alive) {
            //  Remote server kicked us, don't retry
            icl_console_print ("I: AMQP broker closed connection");
            the_next_event = exception_event;
            break;
        }
    }
}


//************************   TERMINATE THE PROGRAM    ***********************

MODULE terminate_the_program (void)
{
    icl_system_terminate ();
    the_next_event = terminate_event;
}


/**************************   PUBLISH SINGLE FILE   **************************/

MODULE publish_single_file (void)
{
}


//************************   CHECK PROGRAM OPERATION   ************************

MODULE check_program_operation (void)
{
}


//***********************   REGISTER AS FILE PUBLISHER   **********************

MODULE register_as_file_publisher (void)
{
}


//*************************   PREPARE TO STAGE FILE   *************************

MODULE prepare_to_stage_file (void)
{
}


//*************************   GET NEXT FILE FRAGMENT   ************************

MODULE get_next_file_fragment (void)
{
}


//***************************   STAGE THE FRAGMENT   **************************

MODULE stage_the_fragment (void)
{
}


//****************************   PUBLISH THE FILE   ***************************

MODULE publish_the_file (void)
{
}

