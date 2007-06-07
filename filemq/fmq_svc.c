/*===========================================================================
 *
 *  FileMQ service
 *
 *  Implements the FileMQ service protocol.
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

#define CLIENT_NAME "FileMQ-Service/0.1"
#define COPYRIGHT "Copyright (c) 1996-2007 iMatix Corporation"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: fmq_svc [options...]\n"                                        \
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

#include "fmq_svc.d"                    //  Include dialog data


//- Global variables used in this source file only --------------------------

static int
    feedback = 0;                       //  Feedback for calling program
static char
    *opt_service,                       //  FileMQ service name
    *opt_hostname,                      //  Host to connect to
    *opt_trace;                         //  Trace level argument
static fmq_session_t
    *session;                           //  Session to broker
static Bool
    opt_robust = FALSE;                 //  Retry if broker not available?
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
#   include "fmq_svc.i"                 //  Start dialog engine
}


//************************   INITIALISE THE PROGRAM   ***********************

MODULE initialise_the_program (void)
{
    icl_console_print ("I: starting file service '%s'", opt_service);
    the_next_event = ok_event;
}


//**********************   OPEN CONNECTION TO BROKER   **********************

MODULE open_connection_to_broker (void)
{
    assert (!session);
    session = fmq_session_new ();
    fmq_session_set_trace (session, atoi (opt_trace));
    fmq_session_set_login (session, "guest", "guest");
    rc = fmq_session_open (session, opt_hostname, "fmq_svc");
    if (rc) {
        icl_console_print ("E: could not connect to %s", opt_hostname);
        raise_exception (no_broker_event);
    }
}


//************************   REGISTER AS FILE SERVICE   ***********************

MODULE register_as_file_service (void)
{
    icl_shortstr_t
        queue_name;
        
    icl_shortstr_fmt (queue_name, "fmq.%s", opt_service);
    fmq_session_listen (session, queue_name);
}


//***********************   WAIT FOR INCOMING MESSAGE   ***********************

MODULE wait_for_incoming_message (void)
{
    assert (session);
    rc = fmq_session_wait (session);
    if (rc == FMQ_SESSION_INTERRUPT)
        raise_exception (interrupt_event);
    else
    if (rc == FMQ_SESSION_NO_BROKER)
        raise_exception (no_broker_event);
    else
    if (rc == FMQ_SESSION_EXCEPTION)
        raise_exception (exception_event);
    else
    if (rc == FMQ_SESSION_RETURNED)
        raise_exception (returned_event);
    else
    if (rc == FMQ_SESSION_ARRIVED) {
        //  These are the messages we accept
        if (streq (session->content->type, "STAGE"))
            the_next_event = stage_event;
        else
        if (streq (session->content->type, "PUBLISH"))
            the_next_event = publish_event;
        else
        if (streq (session->content->type, "SUBSCRIBE"))
            the_next_event = subscribe_event;
        else
        if (streq (session->content->type, "SYNC"))
            the_next_event = sync_event;
        else
            the_next_event = other_event;
    }
}


//**************************   STAGE FILE FRAGMENT   **************************

MODULE stage_file_fragment (void)
{
    char
        *filename;
    time_t
        filetime;
    size_t
        filesize,
        offset;

    filename =       asl_field_list_string (self->header_fields, "FILENAME");
    filesize = atol (asl_field_list_string (self->header_fields, "FILESIZE"));
    offset   = atol (asl_field_list_string (self->header_fields, "OFFSET"));

    file index
        id (1..n), name (string), hash (sha1), size (bytes), staged (bytes), version (1..n)

    FILE
        *file_stream;
    int
        ch;
    </local>
    //    
    assert (filename);
    file_stream = fopen (filename, "r");
    if (file_stream) {
        while ((ch = fgetc (file_stream)) != EOF)
            if (ch == '\\n')
                rc++;
        fclose (file_stream);
    }


    - text file with all file entries

    - unique entry key is name+hash
    - add new entry to index if needed
        append the fragment if any
    - assertions
        new staged is not greater than filesize
    - return amount of data staged

    amq_content_basic_set_headers_field (content, "FILENAME", filename);
    amq_content_basic_set_headers_field (content, "FILE-ID",  file_id);
    amq_content_basic_set_headers_field (content, "STAGED",   "%"PRId64, staged);
    

    Staging...
    - all files are staged uniquely, in a single directory
    - files can be published onto arbitrary channels
    - do we have the file staged somewhere?
    

    fmq_session_reply_stage_ok (session, "somefile", "fileid", 1234);
}


//**********************   PUBLISH FILE TO SUBSCRIBERS   **********************

MODULE publish_file_to_subscribers (void)
{
    fmq_session_reply_publish_ok (session, "fileid", 100);
}


//************************   REGISTER NEW SUBSCRIBER   ************************

MODULE register_new_subscriber (void)
{
    fmq_session_reply_subscribe_ok (session);
}


//*************************   SYNCHRONISE SUBSCRIBER   ************************

MODULE synchronise_subscriber (void)
{
    fmq_session_reply_sync_ok (session, "fileid");
}


//*************************   SIGNAL UNKNOWN MESSAGE   ************************

MODULE signal_unknown_message (void)
{
    icl_console_print ("W: unknown message received: '%s'", session->content->type);
    fmq_session_reply_exception (session, "Unknown message");
}


//***********************   CHECK IF RETRIES WANTED   ************************

MODULE check_if_retries_wanted (void)
{
    if (opt_robust)
        the_next_event = retry_event;
    else
        the_next_event = abandon_event;
}


//**********************   SIGNAL UNEXPECTED RETURNS   ***********************

MODULE signal_unexpected_returns (void)
{
    icl_console_print ("E: messages returned from broker");
}


//*************************   SIGNAL SHUTTING DOWN   *************************

MODULE signal_shutting_down (void)
{
    icl_console_print ("I: shutting down file service '%s'", opt_service);
}


//**********************   CLOSE CONNECTION TO BROKER   **********************

MODULE close_connection_to_broker (void)
{
    fmq_session_close (session);
    fmq_session_destroy (&session);
}


//**************************   PAUSE FOR RECOVERY   **************************

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

