/*===========================================================================
    amq_kill.c - demo of remote broker management in C
    
    Connects to an OpenAMQ server as specified on the command line, reports
    current status of server, and shuts it down.

    By iMatix Corporation, May 2008.  Code released into the public domain.
 *===========================================================================*/

#include "icl.h"
#include "amq_mgt_console.h"
#include "amq_mgt_broker.h"

#define USAGE                                                                \
    "Syntax: amq_kill [options...]\n"                                        \
    "   Kill OpenAMQ server\n"                                               \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -s hostname      Server hostname and :port (localhost)\n"             \
    "  -u user          User name for console access (guest)\n"              \
    "  -p password      Password for console access (guest)\n"               \
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
        args_ok = TRUE;                 //  Were the arguments okay?
    char
        *s_opt_host = NULL,             //  -s specifies server name
        *s_opt_user = NULL,             //  -u specifies user name
        *s_opt_pass = NULL;             //  -p specifies password
    char
        **argparm;                      //  Argument parameter to pick-up
    amq_mgt_console_t
        *console;
    ipr_token_list_t
        *tokens;                        //  Command tokens

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
                case 'h':
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
        s_opt_user = "guest";
    if (!s_opt_pass)
        s_opt_pass = "guest";

    console = amq_mgt_console_new (s_opt_host, "/", s_opt_user, s_opt_pass);
    if (!console)
        exit (EXIT_FAILURE);

    icl_console_out ("Connected to %s/%s on %s\n",
        console->connection->server_product,
        console->connection->server_version,
        console->connection->server_host);

    amq_mgt_broker_print_properties (console->broker, NULL);

    tokens = ipr_token_split ("shutdown");
    amq_mgt_broker_cmdline (console->broker, console->connection->server_host, 0, tokens, NULL);
    ipr_token_list_destroy (&tokens);
    amq_mgt_console_destroy (&console);

    icl_system_terminate ();            //  Terminate all classes
    return (EXIT_SUCCESS);
}

