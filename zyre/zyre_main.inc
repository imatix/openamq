/*===========================================================================

    Main body for Zyre server

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
#include "http.h"
#include "zyre_classes.h"
#include "version.h"

static Bool
    s_opt_silent = FALSE,               //  -q means suppress messages
    s_opt_bkground = FALSE,             //  -s means run in background
    s_opt_showinfo = FALSE;             //  -i means show information
static char
    *s_opt_workdir = NULL,              //  -w specifies working directory
    *s_opt_config = "zyre.cfg",         //  -s specifies configuration file
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
    "Zyre is an implementation of the 7-RestMS@wiki.amqp.org protocol for\n" \
    "RESTful business messaging over plain HTTP.  Zyre speaks to HTTP apps\n" \
    "at one edge, and to an AMQP broker like OpenAMQ at the other.\n"\
    "\n"                                                                     \
    "Syntax: %s [options...]\n"                                              \
    "   Starts the " SERVER_NAME "/" VERSION " server\n"                     \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -w directory     Working directory for server (current)\n"            \
    "  -s filename      Load custom settings from file (zyre.cfg)\n"         \
    "  -S filename      Snapshot server settings to file\n"                  \
    "  -X comment       Comment, has no effect\n"                            \
    "  -q               Quiet mode: no messages (no)\n"                      \
    "  -b               Run as background server process (no)\n"             \
    "  -f               Run as foreground console process (yes)\n"           \
    "  -v               Show version information\n"                          \
    "  -h               Show summary of command-line options\n"              \
    "  --help           Show detailed configuration help\n"                  \
    "\n"

//  Parse arguments, return 0 if OK, 1 if error

static void
s_zyre_initialise (int argc, char *argv [])
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
                    zyre_config_cmdline_help ();
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
        printf (SERVER_NAME "/" VERSION " -- for bugs and updates, go to www.zyre.com\n");
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
    //  Load configuration data into the config tables
    //  We need to prepare two config objects, zyre_config and http_config,
    //  used by the Zyre and Xitami cores respectively.  They will share an
    //  identical config tree, loaded from:
    //
    //      - http_base.cfg (in-built)
    //      - zyre_base.cfg (in-built)
    //      - xitami.cfg    (if found on path)
    //      - s_opt_config  (zyre.cfg, if found on path)
    //      - command line  (final override)
    //
    //  The command line is parsed by zyre_config.
    //
    zyre_config = zyre_config_new ();
    bucket = http_static_get ("http_base.cfg");
    zyre_config_load_bucket  (zyre_config, bucket);
    ipr_bucket_destroy (&bucket);
    bucket = zyre_static_get ("zyre_base.cfg");
    zyre_config_load_bucket  (zyre_config, bucket);
    ipr_bucket_destroy (&bucket);
    zyre_config_load_xmlfile  (zyre_config, "xitami.cfg", FALSE);
    zyre_config_load_xmlfile  (zyre_config, s_opt_config, FALSE);
    zyre_config_cmdline_parse (zyre_config, SERVER_NAME, s_argc, s_argv);

    //  Now shadow http_config from zyre_config so that we can run the
    //  Xitami core properly
    http_config = http_config_new ();
    http_config_shadow (http_config, zyre_config_table (zyre_config));

    //  Set system options from configuration
#   if defined (BASE_THREADSAFE)
    icl_console_print ("I: allocating %d poll threads, %d worker threads",
        zyre_config_polling_threads (zyre_config),
        zyre_config_working_threads (zyre_config));
    smt_set_polling_threads (zyre_config_polling_threads (zyre_config));
    smt_set_working_threads (zyre_config_working_threads (zyre_config));
#   endif

    //  Snapshot configuration into debug file, if wanted
    if (s_opt_snapshot) {
        if (streq (s_opt_snapshot, s_opt_config))
            icl_console_print ("E: unwise snapshot to '%s' prevented", s_opt_snapshot);
        else {
            zyre_config_commit (zyre_config, s_opt_snapshot);
            icl_console_print ("I: saved configuration snapshot to '%s'", s_opt_snapshot);
        }
    }
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

            icl_shortstr_fmt (logfile_name, "zyre_%s.log", zyre_config_port (zyre_config));
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
s_zyre_execute (void)
{
    smt_log_print (http_server->alert_log, "I: loading web server plugin modules...");
    //  Register driver_modules, in lookup order
    s_register_driver_module (zyre_restms__http_driver_module_new (NULL, "/restms"));
    s_register_driver_module (http_driver_null__http_driver_module_new (NULL, "/null"));
    s_register_driver_module (http_driver_file__http_driver_module_new (NULL, "/"));

    //  Register access modules, in execution order
    s_register_access_module (http_access_dos__http_access_module_new (NULL));
    s_register_access_module (http_access_policy__http_access_module_new (NULL));
    s_register_access_module (http_access_logger__http_access_module_new (NULL));
    //  This replaces the http_access_refresh module
    s_register_access_module (zyre_digest_amqp__http_access_module_new (NULL));

    //  Run SMT until finished
    smt_wait (0);
}

static void
s_zyre_terminate (void)
{
    http_server_destroy (&http_server);
    http_config_destroy (&http_config);
    zyre_config_destroy (&zyre_config);

    if (s_opt_showinfo)
        icl_stats_dump ();
    icl_console_print ("I: shutdown is complete");
    icl_system_terminate ();
}
