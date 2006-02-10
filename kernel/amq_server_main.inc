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
    s_opt_silent = FALSE,               //  -q means suppress messages
    s_opt_bkground = FALSE,             //  -s means run in background
    s_opt_showinfo = FALSE;             //  -i means show information
static char
    *s_opt_port = NULL,                 //  -p specifies server port
    *s_opt_workdir = NULL,              //  -w specifies working directory
    *s_opt_cluster = NULL,              //  -c specifies broker cluster name
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

#define USAGE1                                                               \
    "Syntax: %s [options...]\n"                                              \
    "   Starts the OpenAMQ server\n"                                         \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -w directory     Working directory for server (current)\n"            \
    "  -c name          Set server cluster name (none)\n"                    \
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
    "Deprecated options:\n"                                                  \
    "  -p port          Server port\n"                                       \
    "  -t level         Set trace level 0-3 (0)\n"                           \
    "  -m n             Monitor server status every N seconds (1)\n"         \
    "\n"                                                                     \
    "The order of arguments is not important. Switches and filenames\n"      \
    "are case sensitive. See documentation for detailed information.\n"      \
    "\n"

//  Parse arguments, return 0 if OK, 1 if error

static int
s_start (int argc, char *argv [], char *config_file, char *overlay)
{
    int
        rc = 0,
        argn;                           //  Argument number                  
    char
        *myname,                        //  Executable's name                
        **argparm;                      //  Argument parameter to pick-up

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
                case 'p':
                    icl_console_print ("W: deprecated argument, suggest you use '--port'");
                    argparm = &s_opt_port;
                    break;
                case 'w':
                    argparm = &s_opt_workdir;
                    break;
                case 'c':
                    argparm = &s_opt_cluster;
                    break;
                case 't':
                    icl_console_print ("W: deprecated argument, suggest you use '--trace'");
                    argparm = &s_opt_trace;
                    break;
                case 'm':
                    icl_console_print ("W: deprecated argument, suggest you use '--monitor'");
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
                    printf (BUILDMODEL "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
                    printf ("Compiled with: " CCOPTS "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (SERVER_NAME "/" VERSION "\n");
                    printf (BUILDMODEL "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (USAGE1, SERVER_NAME);
                    amq_server_config_cmdline_help ();
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
        printf (BUILDMODEL "\n\n");
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
    amq_server_config = amq_server_config_new (config_file, overlay, TRUE);
    amq_proxy_config  = amq_proxy_config_new  (config_file, overlay, FALSE);
    amq_server_config_cmdline_parse (amq_server_config, SERVER_NAME, s_argc, s_argv);

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
    return (rc);
}

//  Initialise server

static int
s_run_server (void)
{
    amq_broker_t
        *broker_handle;                 //  Hold onto broker
        
    //  Synchronous start-up
    if (s_opt_trace)
        amq_server_config_set_trace   (amq_server_config, atoi (s_opt_trace));
    if (s_opt_monitor)
        amq_server_config_set_monitor (amq_server_config, atoi (s_opt_monitor));
    if (s_opt_port)
        amq_server_config_set_port    (amq_server_config, s_opt_port);

    //  Create main objects and do the work
    amq_console = amq_console_new ();
    amq_broker  = amq_broker_new  ();
    amq_cluster = amq_cluster_new (amq_broker, s_opt_cluster);

    smt_wait (0);                       //  Execute the server

    broker_handle = amq_broker_link (amq_broker);
    amq_cluster_destroy (&amq_cluster);
    smt_wait (0);
    amq_broker_destroy  (&amq_broker);
    smt_wait (0);
    amq_console_destroy (&amq_console);
    smt_wait (0);

    //  Synchronous shutdown
    amq_server_config_destroy (&amq_server_config);
    amq_proxy_config_destroy  (&amq_proxy_config);
    amq_broker_unlink         (&broker_handle);

    if (s_opt_showinfo)
        icl_stats_dump ();
    icl_console_print ("I: shutdown is complete");
    icl_system_terminate ();
    return (0);
}
