/*===========================================================================

    Main preparation for demo server

    This file is defined as an include file so that it can be invoked
    directly from the main, setting up static variables that are used
    at different points in the initialisation process.  The separation
    of this code from the main code is intended to ease the creation
    of customised main programs without duplication of this code.

    Copyright (c) 2004-2005 iMatix Corporation
 *===========================================================================*/

#include "asl.h"
#include "version.h"
#include "amq_server_classes.h"

static Bool
    s_opt_silent,                       //  -q means suppress messages
    s_opt_bkground,                     //  -s means run in background
    s_opt_showinfo;                     //  -i means show information
static char
    *s_opt_port,                        //  -p specifies server port
    *s_opt_workdir,                     //  -w specifies working directory
    *s_opt_trace,                       //  -t defines trace level
    *s_opt_monitor;                     //  -m means monitor server

#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                                \
    "Syntax: %s [options...]\n"                                              \
    "Options:\n"                                                             \
    "  -p port          Server port\n"                                       \
    "  -w directory     Working directory for server (current)\n"            \
    "  -q               Quiet mode: no messages (no)\n"                      \
    "  -b               Run as background server process (no)\n"             \
    "  -f               Run as foreground console process (yes)\n"           \
    "  -t level         Set trace level 0-3 (0)\n"                           \
    "  -v               Show version information\n"                          \
    "  -h               Show summary of command-line options\n"              \
    "  -m n             Monitor server status every N seconds (1)\n"         \
    "\nThe order of arguments is not important. Switches and filenames\n"    \
    "are case sensitive. See documentation for detailed information.\n"      \
    "\n"


//  Parse arguments, return 0 if OK, 1 if error

static int
s_parse_args (int argc, char *argv [])
{
    int
        rc = 0,
        argn;                           //  Argument number                  
    char
        *myname,                        //  Executable's name                
        **argparm;                      //  Argument parameter to pick-up

    //  These are the arguments we may get on the command line
    s_opt_silent   = FALSE;
    s_opt_bkground = FALSE;
    s_opt_showinfo = FALSE;
    s_opt_port     = NULL;
    s_opt_workdir  = NULL;
    s_opt_trace    = NULL;
    s_opt_monitor  = NULL;
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
        if (*argv [argn] == '-') {
            switch (argv [argn][1]) {
                //  These switches take a parameter
                case 'p':
                    argparm = &s_opt_port;
                    break;
                case 'w':
                    argparm = &s_opt_workdir;
                    break;
                case 't':
                    argparm = &s_opt_trace;
                    break;
                case 'm':
                    argparm = &s_opt_monitor;
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
                    printf (USAGE, SERVER_NAME);
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
    if (rc)
        icl_console_print ("E: invalid arguments - type '%s -h' for help", myname);
    else
    if (argparm) {
        icl_console_print ("E: argument missing - type '%s -h' for help", myname);
        rc = 1;
    }
    else {
        if (s_opt_silent)
            icl_console_mode (ICL_CONSOLE_QUIET, TRUE);
        else {
            printf (SERVER_NAME "/" VERSION "\n");
            printf (SERVER_TEXT "\n\n");
            printf (COPYRIGHT "\n");
            printf (NOWARRANTY);
        }
        icl_system_initialise (argc, argv);

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

                if (s_opt_port)
                    icl_shortstr_fmt (logfile_name, "amq_server_%s.log", s_opt_port);
                else
                    icl_shortstr_cpy (logfile_name, "amq_server.log");

                logfile = fopen (logfile_name, "a");
                icl_console_redirect (logfile);
                icl_console_print ("I: ************************   SERVER START   ************************");
                icl_console_print ("I: " SERVER_NAME "/" VERSION " - revision " SVN_REVISION);
            }
            else {
                icl_console_print ("E: backgrounding failed - server is halting");
                rc = 1;
            }
        }
        smt_initialise ();
        ipr_uid_set_real_user ();
    }
    return (rc);
}

//  Load server configuration files

static void
s_load_config (char *config_file, char *overlay)
{
    //  Set server working directory if necessary
    if (s_opt_workdir && apr_filepath_set (s_opt_workdir, icl_global_pool) != APR_SUCCESS) {
        icl_console_print ("E: can't work in '%s' - %s", s_opt_workdir, strerror (errno));
        exit (EXIT_FAILURE);
    }
    //  Load configuration data, if any, into the config_table
    amq_server_config = amq_server_config_new (
        config_file, overlay, TRUE, s_opt_trace? atoi (s_opt_trace): 0);
    amq_proxy_config = amq_proxy_config_new (
        config_file, overlay, FALSE, FALSE);
}


//  Initialise server

static int
s_run_server (void)
{
    //  Synchronous start-up
    if (s_opt_trace)
        amq_server_config_set_trace   (amq_server_config, atoi (s_opt_trace));
    if (s_opt_monitor)
        amq_server_config_set_monitor (amq_server_config, (Bool) atoi (s_opt_monitor));
    if (s_opt_port)
        amq_server_config_set_port    (amq_server_config, s_opt_port);

    //  Create main objects and do the work
    amq_console = amq_console_new ();
    amq_broker  = amq_broker_new  ();
    amq_cluster = amq_cluster_new ();
    smt_wait (0);                       //  Execute the server

    amq_cluster_destroy (&amq_cluster);
    smt_wait (0);
    amq_broker_destroy  (&amq_broker);
    smt_wait (0);
    amq_console_destroy (&amq_console);
    smt_wait (0);

    //  Synchronous shutdown
    amq_server_config_destroy (&amq_server_config);
    amq_proxy_config_destroy  (&amq_proxy_config);
    if (s_opt_showinfo)
        icl_stats_dump ();
    icl_console_print ("I: shutdown is complete");
    icl_system_terminate ();
    return (0);
}
