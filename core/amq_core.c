/*===========================================================================
 *
 *  amq_core.c   OpenAMQ server kernel core
 *
 *  Copyright (c) 2004 JPMorgan
 *===========================================================================*/

#include "amq_classes.h"
#include "version.h"

#define SERVER_NAME "OpenAMQ/" VERSION "\n"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
#define USAGE                                                               \
    "Syntax: openamqd [options...]\n"                                       \
    "Options:\n"                                                            \
    "  -w directory     Working directory for server\n"                     \
    "  -q               Quiet mode: no messages\n"                          \
    "  -s               Server mode: run as background job\n"               \
    "  -c               Console mode: run as foreground job\n"              \
    "  -t               Trace all socket i/o operations to log file\n"      \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"


/*  Prototypes for local functions                                           */

static void s_prepare_logging (void);


/*  --------------------------------------------------------------------------
    amq_server_core

    Initialises and runs the AMQP server core.  Returns zero if the server
    ran successfully, else returns 1.
 */

int
amq_server_core (
    int argc, char *argv [])            /*  Command-line arguments           */
{
    int
        argn;                           /*  Argument number                  */
    Bool
        args_ok = TRUE,                 /*  Were the arguments okay?         */
        quiet_mode = FALSE;             /*  -q means suppress messages       */
    char
        *workdir,                       /*  Working directory                */
        *background,                    /*  -s means run in background       */
        **argparm;                      /*  Argument parameter to pick-up    */

    /*  First off, switch to user's id                                       */
    set_uid_user ();

    /*  These are the arguments we may get on the command line               */
    workdir    = NULL;
    background = NULL;
    console_set_mode (CONSOLE_DATETIME);

    argparm = NULL;                     /*  Argument parameter to pick-up    */
    for (argn = 1; argn < argc; argn++)
      {
        /*  If argparm is set, we have to collect an argument parameter      */
        if (argparm)
          {
            if (*argv [argn] != '-')    /*  Parameter can't start with '-'   */
              {
                *argparm = strdupl (argv [argn]);
                argparm = NULL;
              }
            else
              {
                args_ok = FALSE;
                break;
              }
          }
        else
        if (*argv [argn] == '-')
          {
            switch (argv [argn][1])
              {
                /*  These switches take a parameter                          */
                case 'w':
                    argparm = &workdir;  break;

                /*  These switches have an immediate effect                  */
                case 'q':
                    quiet_mode = TRUE;
                    break;
                case 's':
                    background = "1";
                    break;
                case 'S':
                    background = "0";
                    break;
                case 't':
                    smt_socket_request_trace (TRUE);
                    break;
                case 'v':
                    puts (SERVER_NAME);
                    puts (COPYRIGHT);
                    puts (NOWARRANTY);
                    puts (BUILDMODEL);
                    puts ("Built on: " BUILDDATE);
                    exit (EXIT_SUCCESS);
                case 'h':
                    puts (SERVER_NAME);
                    puts (COPYRIGHT);
                    puts (NOWARRANTY);
                    puts (USAGE);
                    exit (EXIT_SUCCESS);

                /*  Anything else is an error                                */
                default:
                    args_ok = FALSE;
              }
          }
        else
          {
            args_ok = FALSE;
            break;
          }
      }

    /*  If there was a missing parameter or an argument error, quit          */
    if (argparm) {
        coprintf ("Argument missing - type 'openamqd -h' for help");
        goto failed;
    }
    else
    if (!args_ok) {
        coprintf ("Invalid arguments - type 'openamqd -h' for help");
        goto failed;
    }

    /*  Set server working directory if necessary                            */
    if (workdir && set_curdir (workdir)) {
        coprintf ("Can't work in '%s' - %s", workdir, strerror (errno));
        goto failed;
    }
    /*  Load configuration data, if any, into the config_table               */
    amq_config = ipr_config_table_new (".", AMQ_SERVER_CONFIG);
    if (!amq_config) {
        coprintf ("No configuration provided - server is halting");
        goto failed;
    }

    /*  Initialise arguments, taking defaults from the config_table          */
    if (!background)
        background = ipr_config_table_lookup (amq_config, "server/background", "0");

    if (quiet_mode) {
        fclose (stdout);                /*  Kill standard output             */
        fclose (stderr);                /*   and standard error              */
    }
    else {
        puts (SERVER_NAME);
        puts (COPYRIGHT);
        puts (NOWARRANTY);
    }
    if (*background == '1') {
        const char
           *background_args [] = { "-s", NULL };

        coprintf ("Moving into the background...");
        if (process_server (NULL, NULL, argc, argv, background_args) != 0) {
            coprintf ("Backgrounding failed - server is halting");
            goto failed;
        }
    }

    /*  Pre-module initialisation                                            */
    s_prepare_logging ();
    amq_vhosts = amq_vhost_table_new (amq_config);

    /*  Register resources                                                   */
    if (amq_user_modules ()) {
        coprintf ("E: module registration failed - server is halting");
        goto failed;
    }

    /*  Execute the server                                                   */
    smt_thread_execute (SMT_EXEC_FULL);

    /*  Release resources                                                    */
    ipr_config_table_destroy (&amq_config);
    amq_vhost_table_destroy  (&amq_vhosts);
    icl_system_destroy ();

    /*  Report memory usage                                                  */
    coprintf ("Allocs=%ld frees=%ld\n", mem_allocs (), mem_frees ());

    icl_mem_assert ();
    return (0);

    failed:
        exit (EXIT_FAILURE);
}


/*  -------------------------------------------------------------------------
    Prepares server logging

    - creates log directory if necessary
    - redirects console output to log file
 */
static void
s_prepare_logging (void)
{
    char
        *log_dir,
        *console_file;

    log_dir = ipr_config_table_lookup (amq_config, "logging/directory", "./logs");
    if (!file_is_directory (log_dir))
        make_dir (log_dir);

    console_file = file_where ('a', log_dir,
        ipr_config_table_lookup (amq_config, "logging/console", "amq_console.log"), NULL);

    console_send    (NULL, TRUE);
    console_capture (console_file, 'a');
}

