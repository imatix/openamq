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
#include "fmq_session.h"

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
static fmq_session_t
    *session;                           //  Session to broker
static Bool
    opt_robust = FALSE;                 //  Retry if broker not available?
static icl_shortstr_t
    service_queue;                      //  Name of service queue
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
    icl_shortstr_fmt (service_queue, "fmq.%s", opt_service);
    icl_console_print ("I: starting up file publisher, service=%s", opt_service);
    the_next_event = ok_event;
}


//**********************   OPEN CONNECTION TO BROKER   **********************

MODULE open_connection_to_broker (void)
{
    assert (!session);
    session = fmq_session_new ();
    fmq_session_set_trace (session, atoi (opt_trace));
    fmq_session_set_login (session, "guest", "guest");
    rc = fmq_session_open (session, opt_hostname, "fmq_pub");
    if (rc) {
        icl_console_print ("E: could not connect to %s", opt_hostname);
        raise_exception (no_broker_event);
    }
}


//************************   CHECK PROGRAM OPERATION   ************************

MODULE check_program_operation (void)
{
    if (strnull (opt_filename))
        the_next_event = single_file_event;
    else
        the_next_event = empty_event;
}


//***********************   REGISTER AS FILE PUBLISHER   **********************

MODULE register_as_file_publisher (void)
{
    //  Create private queue and listen to it
    fmq_session_listen (session, NULL);
}


//*************************   PREPARE TO STAGE FILE   *************************

MODULE prepare_to_stage_file (void)
{
                                            //  bucket filename size time offset
    rc = fmq_session_request_stage (session, service_queue, NULL, "somefile", 0, 0, 0);
    if (rc == FMQ_SESSION_RETURNED) {
        icl_console_print ("E: file service '%s' is not running", opt_service);
        raise_exception (exception_event);
    }
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
    icl_console_print ("I: shutting down file publisher");
}


//**********************   CLOSE CONNECTION TO BROKER   *********************

MODULE close_connection_to_broker (void)
{
    fmq_session_close (session);
    fmq_session_destroy (&session);
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
}


//************************   TERMINATE THE PROGRAM    ***********************

MODULE terminate_the_program (void)
{
    icl_system_terminate ();
    the_next_event = terminate_event;
}

