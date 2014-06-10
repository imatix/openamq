/*===========================================================================

    Main body for stand-alone HTTP server application

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

#include "icl.h"
#include "ipr.h"
#include "smt.h"
#include "version.h"
#include "http.h"

static Bool
    s_opt_silent = FALSE,               //  -q means suppress messages
    s_opt_bkground = FALSE,             //  -s means run in background
    s_opt_showinfo = FALSE;             //  -i means show information
static char
    *s_opt_workdir = NULL,              //  -w specifies working directory
    *s_opt_config = "xitami.cfg",       //  -s specifies configuration file
    *s_opt_snapshot = NULL;             //  -S specifies snapshot filename
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
    "   Starts the " SERVER_NAME "/" VERSION " web server\n"                 \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -w directory     Working directory for server (current)\n"            \
    "  -s filename      Load custom settings from file (xitami.cfg)\n"       \
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
s_http_initialise (int argc, char *argv [])
{
    Bool
        arg_error = FALSE;
    int
        argn;                           //  Argument number
    char
        *myname,                        //  Executable's name
        *comment,                       //  We ignore this argument
        **argparm;                      //  Argument parameter to pick-up
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
                case 'w':
                    argparm = &s_opt_workdir;
                    break;
                case 's':
                    argparm = &s_opt_config;
                    break;
                case 'S':
                    argparm = &s_opt_snapshot;
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
#if (defined (CCOPTS))
                    printf ("\nCompiler: " CCOPTS "\n");
#endif
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (SERVER_NAME "/" VERSION "\n");
                    printf (BUILDMODEL "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (USAGE, argv [0]);
                    http_config_cmdline_help ();
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
        printf (SERVER_NAME "/" VERSION " -- for bugs and updates, go to www.xitami.com\n");
        printf (BUILDMODEL "\n\n");
        printf (COPYRIGHT "\n");
        printf (NOWARRANTY);
    }
#   if defined (BASE_THREADSAFE)
    icl_console_print ("I: loading X5 web server engine (multi-core scalable)");
#   else
    icl_console_print ("I: loading X5 web server engine (single-core build)");
#   endif
    //  Set server working directory if necessary
    if (s_opt_workdir) {
        icl_console_print ("I: switching current directory to %s", s_opt_workdir);
        if (apr_filepath_set (s_opt_workdir, icl_global_pool) != APR_SUCCESS) {
            icl_console_print ("E: can't work in '%s' - %s", s_opt_workdir, strerror (errno));
            exit (EXIT_FAILURE);
        }
    }
    //  Load configuration data, if any, into the config_table
    http_config = http_config_new ();
    bucket = http_static_get ("http_base.cfg");
    http_config_load_bucket  (http_config, bucket);
    ipr_bucket_destroy (&bucket);
    http_config_load_xmlfile  (http_config, s_opt_config, FALSE);
    http_config_cmdline_parse (http_config, SERVER_NAME, s_argc, s_argv);

    //  Snapshot configuration into debug file, if wanted
    if (s_opt_snapshot) {
        if (streq (s_opt_snapshot, s_opt_config))
            icl_console_print ("E: unwise snapshot to '%s' prevented", s_opt_snapshot);
        else {
            http_config_commit (http_config, s_opt_snapshot);
            icl_console_print ("I: saved configuration snapshot to '%s'", s_opt_snapshot);
        }
    }
    //  Set system options from configuration
#   if defined (BASE_THREADSAFE)
    icl_console_print ("I: allocating %d poll threads, %d worker threads",
        http_config_polling_threads (http_config),
        http_config_working_threads (http_config));
    smt_set_polling_threads (http_config_polling_threads (http_config));
    smt_set_working_threads (http_config_working_threads (http_config));
#   endif
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

            icl_shortstr_fmt (logfile_name, "http_%s.log", http_config_port (http_config));
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

    //  Start a single server instance on the configured HTTP port
    http_server = http_server_new ();
}

//  Registers a driver module and sends it an announce request
static void
s_register_driver_module (http_driver_module_t *module)
{
    http_server__http_driver_module_bind (http_server, module);
    http_driver_module_request_announce (module, http_server->alert_log);
    http_driver_module_unlink (&module);
}

//  Registers an access module and sends it an announce request
static void
s_register_access_module (http_access_module_t *module)
{
    http_server__http_access_module_bind (http_server, module);
    http_access_module_request_announce (module, http_server->alert_log);
    http_access_module_unlink (&module);
}

static void
s_http_execute (void)
{
    smt_log_print (http_server->alert_log, "I: loading web server plugin modules...");
    //  Register driver_modules, in lookup order
    s_register_driver_module (http_driver_cgi__http_driver_module_new  (NULL, "/cgi-bin"));
    s_register_driver_module (http_driver_null__http_driver_module_new (NULL, "/null"));
    s_register_driver_module (http_driver_file__http_driver_module_new (NULL, "/"));

    //  Register access modules, in execution order
    s_register_access_module (http_access_dos__http_access_module_new (NULL));
    s_register_access_module (http_access_policy__http_access_module_new (NULL));
    s_register_access_module (http_access_logger__http_access_module_new (NULL));
    s_register_access_module (http_access_refresh__http_access_module_new (NULL));

    //  Run SMT until finished
    smt_wait (0);
}

static void
s_http_terminate (void)
{
    http_server_destroy (&http_server);
    http_config_destroy (&http_config);

    if (s_opt_showinfo)
        icl_stats_dump ();
    icl_console_print ("I: shutdown is complete");
    icl_system_terminate ();
}
