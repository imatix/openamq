/*===========================================================================
 *
 *  Main set-up for amq_servers
 *
 *  Copyright (c) 2007-2009 iMatix Corporation
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

//  This file is defined as an include file so that it can be invoked
//  directly from the main, setting up static variables that are used
//  at different points in the initialisation process.  The separation
//  of this code from the main code is intended to ease the creation
//  of customised main programs without duplication of this code.

#include "asl.h"
#include "version.h"
#include "amq_server_classes.h"

static Bool
    s_opt_silent = FALSE,               //  -q means suppress messages
    s_opt_bkground = FALSE,             //  -s means run in background
    s_opt_showinfo = FALSE;             //  -i means show information
static char
    *s_opt_port = NULL,                 //  -p specifies server port
    *s_opt_workdir = NULL,              //  -w specifies working directory
    *s_opt_config = "amq_server.cfg",   //  -s specifies configuration file
    *s_opt_snapshot = NULL,             //  -S specifies snapshot filename
    *s_opt_trace = NULL,                //  -t defines trace level
    *s_opt_monitor = NULL;              //  -m means monitor server
static int
    s_argc;
static char
    **s_argv;

#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"

#define USAGE                                                                \
    "Syntax: %s [options...]\n"                                              \
    "   Starts the " SERVER_NAME "/" VERSION " server\n"                     \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -w directory     Working directory for server (current)\n"            \
    "  -s filename      Load custom settings from file (amq_server.cfg)\n"   \
    "  -S filename      Snapshot server settings to file\n"                  \
    "  -X comment       Comment, has no effect\n"                            \
    "  -q               Quiet mode: no messages (no)\n"                      \
    "  -b               Run as background server process (no)\n"             \
    "  -f               Run as foreground console process (yes)\n"           \
    "  -i               Show program statistics when ending (no)\n"          \
    "  -v               Show version information\n"                          \
    "  -h               Show summary of command-line options\n"              \
    "  --help           Show detailed configuration help\n"                  \
    "\n"

//  Parse arguments, return 0 if OK, 1 if error

static void
s_start (int argc, char *argv [])
{
    Bool
        arg_error = FALSE;
    int
        argn;                           //  Argument number
    char
        *myname,                        //  Executable's name
        *comment,                       //  We ignore this argument
        **argparm;                      //  Argument parameter to pick-up
    ipr_config_t
        *config;                        //  Current server config file
    ipr_bucket_t
        *bucket;

    //  Save argument line for later use
    s_argc = argc;
    s_argv = argv;

    icl_system_initialise (argc, argv);
    icl_console_mode (ICL_CONSOLE_DATE, TRUE);
    myname  = argv [0];
    argparm = NULL;                     //  Argument parameter to pick-up
    for (argn = 1; argn < argc; argn++) {
        //  If argparm is set, we have to collect an argument parameter
        if (argparm) {
            if (*argv [argn] != '-') {  //  Parameter can't start with '-'
                *argparm = argv [argn];
                argparm = NULL;
            }
            else {
                arg_error = TRUE;
                break;
            }
        }
        else
        if (ipr_str_prefixed (argv [argn], "--"))
            argn++;                     //  Extended argument + value
        else
        if (*argv [argn] == '-') {
            switch (argv [argn][1]) {
                //  These switches take a parameter
                case 'p':
                    icl_console_print ("W: deprecated argument, suggest you use '--port'");
                    argparm = &s_opt_port;
                    break;
                case 'w':
                    argparm = &s_opt_workdir;
                    break;
                case 's':
                    argparm = &s_opt_config;
                    break;
                case 'S':
                    argparm = &s_opt_snapshot;
                    break;
                case 't':
                    icl_console_print ("W: deprecated argument, suggest you use '--trace'");
                    argparm = &s_opt_trace;
                    break;
                case 'm':
                    icl_console_print ("W: deprecated argument, suggest you use '--monitor'");
                    argparm = &s_opt_monitor;
                    break;
                case 'X':
                    argparm = &comment;
                    break;

                //  These switches have an immediate effect
                case 'i':
                    s_opt_showinfo = TRUE;
                    break;
                case 'q':
                    s_opt_silent = TRUE;
                    break;
                case 'b':
                    s_opt_bkground = TRUE;
                    break;
                case 'f':
                    s_opt_bkground = FALSE;
                    break;
                case 'v':
                    printf (SERVER_NAME "/" VERSION " - revision " SVN_REVISION "\n");
                    printf (BUILDMODEL "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf ("Build model:" BUILDMODEL "\n");
                    printf ("Memory model: %s\n",
                        icl_mem_allocator == ICL_MEM_THIN?   "thin":
                        icl_mem_allocator == ICL_MEM_FAT?    "fat":
                        icl_mem_allocator == ICL_MEM_DIRECT? "direct": "unknown");
#if (defined (BASE_THREADSAFE))
                    printf ("Threading model: multithreaded\n");
#else
                    printf ("Threading model: single threaded\n");
#endif
                    printf ("\nCompiler: " CCOPTS "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (SERVER_NAME "/" VERSION "\n");
                    printf (BUILDMODEL "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (USAGE, argv [0]);
                    amq_server_config_cmdline_help ();
                    exit (EXIT_SUCCESS);

                //  Anything else is an error
                default:
                    arg_error = TRUE;
            }
        }
        else {
            arg_error = TRUE;
            break;
        }
    }
    if (arg_error) {
        icl_console_print ("E: invalid arguments - type '%s -h' for help", myname);
        exit (EXIT_FAILURE);
    }
    else
    if (argparm) {
        icl_console_print ("E: argument missing - type '%s -h' for help", myname);
        exit (EXIT_FAILURE);
    }

    if (s_opt_silent)
        icl_console_mode (ICL_CONSOLE_QUIET, TRUE);
    else {
        printf (SERVER_NAME "/" VERSION " -- for bugs and updates, go to www.openamq.com\n");
        printf (BUILDMODEL "\n\n");
        printf (COPYRIGHT "\n");
        printf (NOWARRANTY);
    }
    //  Set server working directory if necessary
    if (s_opt_workdir && apr_filepath_set (s_opt_workdir, icl_global_pool) != APR_SUCCESS) {
        icl_console_print ("E: can't work in '%s' - %s", s_opt_workdir, strerror (errno));
        exit (EXIT_FAILURE);
    }
    //  Load configuration data, if any, into the config_table
    amq_server_config = amq_server_config_new ();
    bucket = amq_server_resource_get ("amq_server_base.cfg");
    amq_server_config_load_bucket   (amq_server_config, bucket);
    ipr_bucket_destroy (&bucket);
    amq_server_config_load_xmlfile  (amq_server_config, s_opt_config, FALSE);
    amq_server_config_cmdline_parse (amq_server_config, SERVER_NAME, s_argc, s_argv);

    //  Snapshot configuration into debug file, if wanted
    if (s_opt_snapshot) {
        if (streq (s_opt_snapshot, s_opt_config))
            icl_console_print ("E: unwise snapshot to '%s' prevented", s_opt_snapshot);
        else {
            amq_server_config_commit (amq_server_config, s_opt_snapshot);
            icl_console_print ("I: saved configuration snapshot to '%s'", s_opt_snapshot);
        }
    }
    //  Check that server security is reasonably defined
    config = ipr_config_dup (amq_server_config->config);
    ipr_config_locate (config, "/config/security", "plain");
    if (!config->located) {
        icl_console_print ("E: no 'plain' security defined in server config");
        exit (EXIT_FAILURE);
    }
    //  Now check console login/password
    ipr_config_locate (config, "user", "super");
    if (config->located
    && streq (ipr_config_get (config, "password", ""), "super"))
        icl_console_print ("W: security warning - logins use default passwords");
    ipr_config_destroy (&config);

    //  Set system options from configuration
    smt_set_polling_threads (amq_server_config_polling_threads (amq_server_config));
    smt_set_working_threads (amq_server_config_working_threads (amq_server_config));
    smt_set_action_limit    (amq_server_config_kernel_limit    (amq_server_config));

    //  Move into the background if so desired
    if (s_opt_bkground) {
        char
           *background_args [] = { "-b", NULL };

        icl_console_print ("I: moving into the background...");
        if (ipr_process_server (NULL, NULL, argc, argv, background_args) == 0) {
            FILE
                *logfile;
            icl_shortstr_t
                logfile_name;

            icl_shortstr_fmt (logfile_name, "amq_server_%s.log",
                amq_server_config_port (amq_server_config));
            logfile = fopen (logfile_name, "a");
            icl_console_redirect (logfile);
            icl_console_print ("I: ************************   SERVER START   ************************");
            icl_console_print ("I: " SERVER_NAME "/" VERSION " - revision " SVN_REVISION);
        }
        else {
            icl_console_print ("E: backgrounding failed - server is halting");
            exit (EXIT_FAILURE);
        }
    }
    smt_initialise ();
    ipr_uid_set_real_user ();
}

//  Initialise server

static int
s_run_server (void)
{
    int
        rc  = 0;                        //  0 = done, 1 = restart
    amq_broker_t
        *broker_ref;

    //  Synchronous start-up
    if (s_opt_trace)
        amq_server_config_set_trace   (amq_server_config, atoi (s_opt_trace));
    if (s_opt_monitor)
        amq_server_config_set_monitor (amq_server_config, atoi (s_opt_monitor));
    if (s_opt_port)
        amq_server_config_set_port    (amq_server_config, s_opt_port);

    //  Create main objects and do the work
    amq_console = amq_console_new ();
    amq_broker = amq_broker_new ();
    smt_wait (0);                       //  Execute the server

    if (amq_broker->restart)
        rc = 1;                         //  Restart the broker

    broker_ref = amq_broker_link (amq_broker);
    amq_broker_destroy  (&broker_ref);
    amq_console_destroy (&amq_console);
    smt_wait (0);
    amq_broker_unlink   (&amq_broker);
    smt_wait (0);

    //  Synchronous shutdown
    amq_server_config_destroy (&amq_server_config);

    if (s_opt_showinfo)
        icl_stats_dump ();
    icl_console_print ("I: shutdown is complete");
    icl_system_terminate ();
    return (rc);
}
