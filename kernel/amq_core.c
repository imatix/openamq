/*===========================================================================
 *
 *  amq_core.c   OpenAMQ server kernel core
 *
 *  Copyright (c) 2004-2005 JPMorgan and iMatix Corporation
 *===========================================================================*/

#include "amq_classes.h"
#include "amq_server_agent.h"
#include "version.h"

#define SERVER_NAME "%s/" VERSION "\n%s\n\n"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: %s [options...]\n"                                       \
    "Options:\n"                                                            \
    "  -w directory     Working directory for server\n"                     \
    "  -q               Quiet mode: no messages\n"                          \
    "  -s               Server mode: run as background job\n"               \
    "  -c               Console mode: run as foreground job\n"              \
    "  -t level         Set trace level\n"                                  \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "  -m n             Monitor server queues every N seconds\n"            \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"     \
    "\n"

static char
    *s_server_name = "amqpsrv",
    *s_server_text = "*** Test server - for internal use only ***";

/*  Prototypes for local functions                                           */

static void s_prepare_logging  (void);
static void s_prepare_security (void);

/*  --------------------------------------------------------------------------
    amq_set_server_name

    Sets the displayed server name.
 */

void
amq_set_server_name (char *name)
{
    s_server_name = name;
}


/*  --------------------------------------------------------------------------
    amq_set_server_text

    Sets the displayed server description.
 */

void
amq_set_server_text (char *text)
{
    s_server_text = text;
}


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
        *opt_workdir,                   /*  Working directory                */
        *opt_server,                    /*  -s means run in background       */
        *opt_trace,                     /*  0-3                              */
        *opt_monitor,                   /*  >0 = monitor                     */
        **argparm;                      /*  Argument parameter to pick-up    */

    /*  First off, switch to user's id                                       */
    set_uid_user ();

    /*  These are the arguments we may get on the command line               */
    opt_workdir = NULL;
    opt_trace   = "0";
    opt_monitor = "0";
    opt_server  = NULL;
    console_set_mode (CONSOLE_DATETIME);

    argparm = NULL;                     /*  Argument parameter to pick-up    */
    for (argn = 1; argn < argc; argn++) {
        /*  If argparm is set, we have to collect an argument parameter      */
        if (argparm) {
            if (*argv [argn] != '-') {  /*  Parameter can't start with '-'   */
                *argparm = strdupl (argv [argn]);
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
                /*  These switches take a parameter                          */
                case 'w':
                    argparm = &opt_workdir;
                    break;
                case 't':
                    argparm = &opt_trace;
                    break;
                case 'm':
                    argparm = &opt_monitor;
                    break;

                /*  These switches have an immediate effect                  */
                case 'q':
                    quiet_mode = TRUE;
                    break;
                case 's':
                    opt_server = "1";
                    break;
                case 'S':
                    opt_server = "0";
                    break;
                case 'v':
                    printf (SERVER_NAME, s_server_name, s_server_text);
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
                    printf ("Built on: " BUILDDATE "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (SERVER_NAME, s_server_name, s_server_text);
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (USAGE, s_server_name);
                    exit (EXIT_SUCCESS);

                /*  Anything else is an error                                */
                default:
                    args_ok = FALSE;
            }
        }
        else {
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
    if (opt_workdir && set_curdir (opt_workdir)) {
        coprintf ("Can't work in '%s' - %s", opt_workdir, strerror (errno));
        goto failed;
    }

    /*  Load configuration data, if any, into the config_table               */
    amq_config = ipr_config_new (".", AMQ_SERVER_CONFIG, TRUE);
    ipr_config_load (amq_config, ".", AMQ_CUSTOM_CONFIG, FALSE);
    s_prepare_logging ();
    amq_global_set_monitor (atoi (opt_monitor));

    /*  Initialise arguments, taking defaults from the config_table          */
    if (!opt_server) {
        ipr_config_locate (amq_config, "/config/server", NULL);
        opt_server = ipr_config_attr (amq_config, "background", "0");
    }
    if (quiet_mode) {
        fclose (stdout);                /*  Kill standard output             */
        fclose (stderr);                /*   and standard error              */
    }
    else {
        printf (SERVER_NAME, s_server_name, s_server_text);
        printf (COPYRIGHT "\n");
        printf (NOWARRANTY);
    }
    if (*opt_server == '1') {
        const char
           *background_args [] = { "-s", NULL };

        coprintf ("Moving into the background...");
        if (process_server (NULL, NULL, argc, argv, background_args) != 0) {
            coprintf ("Backgrounding failed - server is halting");
            goto failed;
        }
    }

    /*  Configure server resource parameters                                 */
    ipr_config_locate (amq_config, "/config/resources", NULL);
    amq_max_memory = ipr_config_attrn (amq_config, "max-memory");
    amq_txn_limit  = ipr_config_attrn (amq_config, "txn-limit");

    /*  Pre-module initialisation                                            */
    amq_users  = amq_user_table_new ();
    amq_vhosts = amq_vhost_table_new (amq_config);
    s_prepare_security ();

    if (amq_server_agent_init (atoi (opt_trace))) {
        coprintf ("E: could not start server protocol agent");
        goto failed;
    }
    /*  Register user modules                                                */
    if (amq_user_modules ()) {
        coprintf ("E: module registration failed - server is halting");
        goto failed;
    }

    /*  Execute the server                                                   */
    smt_thread_execute (SMT_EXEC_FULL);

    /*  Release resources                                                    */
    amq_vhost_table_destroy (&amq_vhosts);
    amq_user_table_destroy  (&amq_users);
    ipr_config_destroy      (&amq_config);
    icl_system_destroy ();

    /*  Report memory usage                                                  */
    coprintf ("Allocs=%ld frees=%ld\n", icl_mem_allocs (), icl_mem_frees ());

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

    amq_global_set_verbose (TRUE);
    ipr_config_locate (amq_config, "/config/logging", NULL);

    log_dir = ipr_config_attr (amq_config, "directory", "./logs");
    if (!file_is_directory (log_dir))
        make_dir (log_dir);

    console_file = file_where ('a', log_dir,
        ipr_config_attr (amq_config, "console", "amq_console.log"), NULL);

    console_send    (NULL, TRUE);
    console_capture (console_file, 'a');
}


/*  -------------------------------------------------------------------------
    Prepares server security mechanisms
 */

static void
s_prepare_security (void)
{
    char
        *mechanism;

    ipr_config_locate (amq_config, "/config/security", NULL);
    mechanism = ipr_config_attr (amq_config, "mechanism", "PLAIN");
    if (streq (mechanism, "PLAIN"))
        amq_global_set_mechanism (AMQ_MECHANISM_PLAIN);
    else {
        coprintf ("E: invalid security mechanism, '%s'", mechanism);
        amq_global_set_mechanism (AMQ_MECHANISM_PLAIN);
    }
    /*  Load user table from configuration file                              */
    ipr_config_locate (amq_config, "/config/users/user", NULL);
    while (amq_config->located) {
        amq_user_new (
            amq_users,                  /*  Users hash table                 */
            ipr_config_attr (amq_config, "name", ""),
            amq_config);                /*  Configuration entry              */
        ipr_config_next (amq_config);
    }
}
