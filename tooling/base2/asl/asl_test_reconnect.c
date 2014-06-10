/*===========================================================================

    ASL test connection / reconnection facility

    Copyright (c) 1991-2009 iMatix Corporation
 *===========================================================================*/

#include "asl.h"
#include "demo_client_connection.h"
#include "demo_client_session.h"
#include "version.h"

#define CLIENT_NAME "demo_cli/1.0\n\n"
#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"
#define USAGE                                                               \
    "Syntax: program [options...]\n"                                        \
    "Options:\n"                                                            \
    "  -s server        Name or address of server (localhost)\n"            \
    "  -t level         Set trace level (default = 0)\n"                    \
    "                   0=none, 1=low, 2=medium, 3=high\n"                  \
    "  -q               Quiet mode: no messages\n"                          \
    "  -i               Show program statistics when ending (no)\n"         \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive. See documentation for detailed information.\n"     \
    "\n"

int
main (int argc, char *argv [])
{
    int
        argn;                           //  Argument number                    
    Bool
        args_ok = TRUE,                 //  Were the arguments okay?           
        quiet_mode = FALSE,             //  -q means suppress messages         
        show_info  = FALSE;             //  -I means show information          
    char
        *opt_server,                    //  Host to connect to                 
        *opt_trace,                     //  0-3                                
        **argparm;                      //  Argument parameter to pick-up      
    demo_client_connection_t
        *connection;                    //  Current connection                 
    demo_client_session_t
        *session;                       //  Current session                    
    dbyte
        ticket = 0;                     //  Access ticket                      
    int
        count;
    ipr_shortstr_t
        exchange;                       //  Exchange name                      

    //  Initialise basic iCL                                                   
    icl_system_initialise (argc, argv);
    
    //  These are the arguments we may get on the command line                 
    opt_server   = "localhost";
    opt_trace    = "0";

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
                //  These switches take a parameter                            
                case 's':
                    argparm = &opt_server;
                    break;
                case 't':
                    argparm = &opt_trace;
                    break;

                //  These switches have an immediate effect                    
                case 'i':
                    show_info = TRUE;
                    break;
                case 'q':
                    quiet_mode = TRUE;
                    break;
                case 'v':
                    printf (CLIENT_NAME);
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
                    printf ("Built on: " BUILDDATE "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (CLIENT_NAME);
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

    connection = demo_client_connection_new (
        opt_server, "guest", "guest", atoi (opt_trace));
    if (!connection) {
        icl_console_print ("E: could not connect to server");
        goto finished;
    }
    session = demo_client_session_new (connection, "/", 0, NULL);
    if (!session) {
        icl_console_print ("E: could not open session to server");
        goto finished;
    }
    for (count = 0; count < 1000; count++) {
        ipr_shortstr_fmt (exchange, "exchange-%d", count);
        if (demo_client_session_exchange_declare (session,
            ticket, exchange, "fanout", FALSE, FALSE, FALSE))
            goto finished;
    }
    demo_client_connection_destroy (&connection);

    if (show_info)
        icl_stats_dump ();
    icl_system_destroy ();
    return (0);

    finished:
        return (1);
}

