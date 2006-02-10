/*---------------------------------------------------------------------------
    amq_mgt_sh.c - OpenAMQ Console Management Shell

    Copyright (c) 2002-2006 iMatix Corporation
    For documentation and updates see http://www.openamq.org.
 *---------------------------------------------------------------------------*/

#include "icl.h"                
#include "amq_mgt_console.h"            //  Definitions for our class
#include "version.h"

#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"

#define USAGE                                                                \
    "Syntax: amq_shell [options...]\n"                                       \
    "   Management Console for OpenAMQ Servers\n"                            \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -s hostname      Server hostname and :port (localhost)\n"             \
    "  -u user          User name for console access (console)\n"            \
    "  -p password      Password for console access (console)\n"             \
    "  -e commands      Run shell commands, delimited by ;\n"                \
    "  -d               Show date and time in shell output\n"                \
    "  -t               Show time in shell output\n"                         \
    "  -x               Show broker status and then exit immediately\n"      \
    "  -h               Show summary of command-line options\n"              \
    "\n"                                                                     \
    "The order of arguments is not important. Switches and filenames\n"      \
    "are case sensitive. See documentation for detailed information.\n"      \
    "\n"                                                                     

int main (int argc, char *argv[])
{
    int
        argn;                           //  Argument number
    Bool
        args_ok = TRUE,                 //  Were the arguments okay?
        s_opt_exit = FALSE,             //  -x means show status and exit
        s_opt_date = FALSE,             //  -d means show date, time
        s_opt_time = FALSE;             //  -t means show time
    char
        *s_opt_host = NULL,             //  -s specifies server name
        *s_opt_user = NULL,             //  -u specifies user name
        *s_opt_pass = NULL;             //  -p specifies password
    char
        **argparm;                      //  Argument parameter to pick-up
    amq_mgt_console_t
        *console;

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
                case 's':
                    argparm = &s_opt_host;
                    break;
                case 'u':
                    argparm = &s_opt_user;
                    break;
                case 'p':
                    argparm = &s_opt_pass;
                    break;
                case 'd':
                    s_opt_date = TRUE;
                    break;
                case 't':
                    s_opt_time = TRUE;
                    break;
                case 'x':
                    s_opt_exit = TRUE;
                    break;
                case 'v':
                    printf (PRODUCT "\n");
                    printf (COPYRIGHT "\n");
                    printf (BUILDMODEL "\n");
                    printf ("Built on: " BUILDDATE "\n");
                    printf ("Compiler: " CCOPTS "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (PRODUCT "\n");
                    printf (BUILDMODEL "\n\n");
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
    //  Initialise global class so we can use the console
    icl_system_initialise (argc, argv);

    printf ("amq_shell/%s - Management Console for OpenAMQ Servers\n", VERSION);
    printf (COPYRIGHT "\n\n");

    icl_console_mode (ICL_CONSOLE_DATE, s_opt_date);
    icl_console_mode (ICL_CONSOLE_TIME, s_opt_date || s_opt_time);
     
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
    //  Default connection values
    if (!s_opt_host)
        s_opt_host = "localhost";
    if (!s_opt_user)
        s_opt_user = "console";
    if (!s_opt_pass)
        s_opt_pass = "console";
        
    console = amq_mgt_console_new (s_opt_host, s_opt_user, s_opt_pass);
    if (!console)
        exit (EXIT_FAILURE);
        
    icl_console_print ("Connected to %s/%s on %s\n",
        console->connection->server_product,
        console->connection->server_version,
        console->connection->server_host);

    //  Either dump broker state and exit, or do full command line
    if (s_opt_exit)
        amq_mgt_broker_print_full (console->broker);
    else
        amq_mgt_broker_cmdline (console->broker, console->connection->server_host, 0);
        
    amq_mgt_console_destroy (&console);

    icl_system_terminate ();            //  Terminate all classes
    return (EXIT_SUCCESS);
}

