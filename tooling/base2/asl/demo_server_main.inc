/*===========================================================================

    Main preparation for demo server

    This file is defined as an include file so that it can be invoked
    directly from the main, setting up static variables that are used
    at different points in the initialisation process.  The separation
    of this code from the main code is intended to ease the creation
    of customised main programs without duplication of this code.

    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 *===========================================================================*/

#include "asl.h"
#include "version.h"
#include "demo_server_agent.h"
#include "demo_srv_resource.h"

static Bool
    s_opt_silent = FALSE,               //  -q means suppress messages
    s_opt_bkground = FALSE,             //  -s means run in background
    s_opt_showinfo = FALSE;             //  -i means show information
static char
    *s_opt_workdir = NULL,              //  -w specifies working directory
    *s_opt_config = "demo_srv.cfg";     //  -s specifies configuration file
static int
    s_argc;
static char
    **s_argv;

#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"

#define USAGE1                                                               \
    "Syntax: %s [options...]\n"                                              \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -w directory     Working directory for server (current)\n"            \
    "  -s filename      Load custom settings from file (demo_srv.cfg)\n"     \
    "  -q               Quiet mode: no messages (no)\n"                      \
    "  -b               Run as background server process (no)\n"             \
    "  -f               Run as foreground console process (yes)\n"           \
    "  -i               Show program statistics when ending (no)\n"          \
    "  -v               Show version information\n"                          \
    "  -h               Show summary of command-line options\n"              \
    "  --help           Show detailed configuration help\n"                  \
    "\n"

#define USAGE2                                                               \
    "\n"                                                                     \
    "The order of arguments is not important. Switches and filenames\n"      \
    "are case sensitive. See documentation for detailed information.\n"      \
    "\n"

//  Parse arguments, return 0 if OK, 1 if error

static int
s_start (int argc, char *argv [])
{
    int
        rc = 0,
        argn;                           //  Argument number
    char
        *myname,                        //  Executable's name
        **argparm;                      //  Argument parameter to pick-up
    ipr_bucket_t
        *bucket;

    //  Save argument line for later use
    s_argc = argc;
    s_argv = argv;

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
                rc = 1;
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
                case 'w':
                    argparm = &s_opt_workdir;
                    break;
                case 's':
                    argparm = &s_opt_config;
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
                    printf (SERVER_TEXT "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
                    printf ("Compiled with: " CCOPTS "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (SERVER_NAME "/" VERSION "\n");
                    printf (SERVER_TEXT "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (USAGE1, SERVER_NAME);
                    demo_server_config_cmdline_help ();
                    printf (USAGE2);
                    exit (EXIT_SUCCESS);

                //  Anything else is an error
                default:
                    rc = 1;
            }
        }
        else {
            rc = 1;
            break;
        }
    }
    //  Check that the arguments were valid
    if (rc) {
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
        printf (SERVER_NAME "/" VERSION "\n");
        printf (SERVER_TEXT "\n\n");
        printf (COPYRIGHT "\n");
        printf (NOWARRANTY);
    }
    icl_system_initialise (argc, argv);

    //  Set server working directory if necessary
    if (s_opt_workdir && apr_filepath_set (s_opt_workdir, icl_global_pool) != APR_SUCCESS) {
        icl_console_print ("E: can't work in '%s' - %s", s_opt_workdir, strerror (errno));
        exit (EXIT_FAILURE);
    }
    //  Load configuration data, if any, into the config_table
    demo_server_config = demo_server_config_new ();
    bucket = demo_srv_resource_get ("demo_srv_base.cfg");
    demo_server_config_load_bucket   (demo_server_config, bucket);
    ipr_bucket_destroy (&bucket);
    demo_server_config_load_xmlfile  (demo_server_config, s_opt_config, FALSE);
    demo_server_config_cmdline_parse (demo_server_config, SERVER_NAME, s_argc, s_argv);

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

            icl_shortstr_fmt (logfile_name, "demo_server_%s.log",
                demo_server_config_port (demo_server_config));

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
    return (rc);
}

//  Initialise server

static int
s_run_server (void)
{
    //  Do synchronous class initialisations here
    //  -------------------------------------------------------------------

    //  Main processing
    demo_broker = demo_broker_new ();
    smt_wait (0);
    demo_broker_destroy (&demo_broker);
    smt_wait (0);                       //  Shutdown SMT kernel

    //  Synchronous shutdown
    demo_server_config_destroy (&demo_server_config);
    if (s_opt_showinfo)
        icl_stats_dump ();
    icl_system_terminate ();
    return (0);
}


