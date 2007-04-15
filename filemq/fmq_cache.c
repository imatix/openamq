/*===========================================================================
 *
 *  FileMQ cache server
 *
 *  Registers as cache server for a specified FileMQ service, stores all
 *  files sent to the service, and responds to requests for missing files.
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

#define CLIENT_NAME "FileMQ-Cache/0.1"
#define COPYRIGHT "Copyright (c) 1996-2007 iMatix Corporation"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: fmq_cache [options...]\n"                                      \
    "Options:\n"                                                            \
    "  -n servicename   Service name (filemq)\n"                            \
    "  -s server[:port] Name or address, port of server (localhost)\n"      \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -r               Robust behaviour: retry if broker unavailable\n"    \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"     \
    "\n"

#include "fmq_cache.d"                  //  Include dialog data

//- Function prototypes -----------------------------------------------------

static void s_check_status (int rc, char *format, ...);


//- Global variables used in this source file only --------------------------

static int
    feedback = 0;                       //  Feedback for calling program
static char
    *opt_service,                       //  FileMQ service name
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
    queue_name;                         //  Name of service queue
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
    opt_service  = "filemq";
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
#   include "fmq_cache.i"               //  Start dialog engine
}


//************************   INITIALISE THE PROGRAM   ***********************

MODULE initialise_the_program (void)
{
    //  Prepare names for the exchange and queue we use
    icl_shortstr_fmt (exchange_name, "fmq.%s", opt_service);
    icl_shortstr_fmt (queue_name, "fmq.%s", opt_service);

    if (atoi (opt_trace) > 2) {
        amq_client_connection_animate (TRUE);
        amq_client_session_animate (TRUE);
    }
    icl_console_print ("I: starting file service '%s'", opt_service);
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
            token->value, "/", auth_data, "fmq_cache", atoi (opt_trace), 30000);
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


/**********************   DECLARE EXCHANGE FOR SERVICE   *********************/

MODULE declare_exchange_for_service (void)
{
    rc = amq_client_session_exchange_declare (
        session,                    //  Session reference
        access_ticket,              //  Access ticket
        exchange_name,              //  Exchange name
        "topic",                    //  Exchange type
        FALSE,                      //  Passive only
        FALSE,                      //  Durable exchange
        TRUE,                       //  Auto-delete when unused
        FALSE,                      //  Create internal exchange
        NULL                        //  Additional arguments
    );
    s_check_status (rc, "E: can't declare service exchange '%s'", exchange_name);
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


/***********************   REGISTER AS SERVICE CACHE   ***********************/

MODULE register_as_service_cache (void)
{
    rc = amq_client_session_queue_declare (
        session,                    //  Session reference
        access_ticket,              //  Access ticket
        queue_name,                 //  Queue name
        FALSE,                      //  Passive only
        FALSE,                      //  Durable queue
        TRUE,                       //  Exclusive queue
        TRUE,                       //  Auto-delete queue when unused
        NULL                        //  Additional arguments
    );
    s_check_status (rc, "E: can't declare service queue '%s'", queue_name);

    if (!exception_raised) {
        rc = amq_client_session_queue_bind (
            session,                //  Session reference
            access_ticket,          //  Access ticket
            queue_name,             //  Queue name
            exchange_name,          //  Exchange name
            "#",                    //  # = all channels
            NULL                    //  Additional arguments
        );
        s_check_status (rc, "E: can't bind service queue '%s'", queue_name);
    }
    if (!exception_raised) {
        rc = amq_client_session_basic_consume (
            session,                //  Session reference
            access_ticket,          //  Access ticket
            queue_name,             //  Queue name
            NULL,                   //  Consumer tag
            FALSE,                  //  Do not deliver own messages
            TRUE,                   //  No acknowledgement needed
            TRUE,                   //  Request exclusive access
            NULL                    //  Additional arguments
        );
        s_check_status (rc, "E: can't consume from service queue '%s'", queue_name);
    }
}


//************************   PARSE INCOMING MESSAGE   ***********************

MODULE parse_incoming_message (void)
{
    assert (content);
    //  These are the messages we accept
    if (streq (content->type, "HELLO"))
        the_next_event = hello_event;
    else
        the_next_event = other_event;
}


/*****************************   REPLY HELLO OK   ****************************/

MODULE reply_hello_ok (void)
{
    amq_content_basic_t
        *reply;

    //  Send message to service cache
    assert (content);
    reply = amq_content_basic_new ();
    amq_content_basic_set_type (reply, "HELLO-OK");
    rc = amq_client_session_basic_publish (
        session,                     //  Session reference
        reply,                       //  Message to sent
        access_ticket,               //  Access ticket
        NULL,                        //  Default exchange
        content->reply_to,           //  Route back to app
        TRUE,                        //  Mandatory routing
        TRUE);                       //  Immediate delivery
    s_check_status (rc, "E: can't send content to broker");
    amq_content_basic_destroy (&reply);
}


/*************************   SIGNAL UNKNOWN MESSAGE   ************************/

MODULE signal_unknown_message (void)
{
    icl_console_print ("W: unknown message received: '%s'", content->type);
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


//*************************   SIGNAL SHUTTING DOWN   ************************

MODULE signal_shutting_down (void)
{
    icl_console_print ("I: shutting down file service '%s'", opt_service);
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

