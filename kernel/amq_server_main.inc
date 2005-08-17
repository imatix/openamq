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
#include "amq_server_agent.h"

static Bool
    s_opt_silent,                       //  -q means suppress messages       
    s_opt_bkground,                     //  -s means run in background       
    s_opt_showinfo;                     //  -i means show information        
static char
    *s_opt_port,                        //  -p specifies server port         
    *s_opt_workdir,                     //  -w specifies working directory   
    *s_opt_trace,                       //  -t defines trace level           
    *s_opt_monitor;                     //  -m means monitor server          

//  Prototypes for local functions
static void
    s_exchange_declare (char *name, int class);

#define CURVERSION "%s/" VERSION "\n%s\n\n"
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
    "  -m n             Monitor server status every N seconds (off)\n"       \
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

    icl_system_initialise (argc, argv);
    ipr_uid_set_real_user ();

    //  These are the arguments we may get on the command line               
    s_opt_silent   = FALSE;
    s_opt_bkground = FALSE;
    s_opt_showinfo = FALSE;
    s_opt_port     = NULL;              
    s_opt_workdir  = NULL;
    s_opt_trace    = "0";
    s_opt_monitor  = "0";

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
                case 's':
                    s_opt_bkground = TRUE;
                    break;
                case 'c':
                    s_opt_bkground = FALSE;
                    break;
                case 'v':
                    printf (CURVERSION, SERVER_NAME, SERVER_TEXT);
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (BUILDMODEL "\n");
                    printf ("Built on: " BUILDDATE "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (CURVERSION, SERVER_NAME, SERVER_TEXT);
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
        if (s_opt_silent) {
            fclose (stdout);            //  Kill standard output             
            fclose (stderr);            //   and standard error              
        }
        else {
            printf (CURVERSION, SERVER_NAME, SERVER_TEXT);
            printf (COPYRIGHT "\n");
            printf (NOWARRANTY);
        }
        //  Move into the background if so desired                           
        if (s_opt_bkground) {
            char
               *background_args [] = { "-s", NULL };

            icl_console_print ("I: moving into the background...");
            if (ipr_process_server (NULL, NULL, argc, argv, background_args) != 0) {
                icl_console_print ("E: backgrounding failed - server is halting");
                rc = 1;
            }
        }
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
    amq_server_config = amq_server_config_new (config_file, atoi (s_opt_trace));
}


//  Initialise server                                                        

static int
s_run_server (void)
{
    //  Do synchronous class initialisations here                            
    //  -------------------------------------------------------------------  

    //  Set tracing options                                                  
    amq_server_config_set_trace   (amq_server_config, atoi (s_opt_trace));
    amq_server_config_set_monitor (amq_server_config, (Bool) atoi (s_opt_monitor));

    if (s_opt_port)
        amq_server_config_set_port (amq_server_config, s_opt_port);

    //  We use a single global vhost for now                                 
    amq_vhost = amq_vhost_new ();

    if (amq_server_agent_init ()) {
        icl_console_print ("E: could not start server protocol agent");
        return (1);
    }
    //  Do asynchronous class initialisations here                           
    //  -------------------------------------------------------------------  

    //  Automatic wiring schemes
    s_exchange_declare ("$queue",  AMQ_EXCHANGE_DEST_NAME);
    s_exchange_declare ("$topic",  AMQ_EXCHANGE_DEST_WILD);
    s_exchange_declare ("$system", AMQ_EXCHANGE_SYSTEM);
    s_exchange_declare ("$reply",  AMQ_EXCHANGE_DEST_NAME);
    s_exchange_declare ("$alert",  AMQ_EXCHANGE_DEST_WILD);

    //  Execute the server                                                   
    smt_os_thread_wait (0);

    //  Shut down cleanly                                                    
    amq_server_agent_term ();
    smt_os_thread_wait (0);
    
    //  Release resources                                                    
    amq_vhost_destroy (&amq_vhost);
    smt_os_thread_wait (0);

    amq_server_config_destroy (&amq_server_config);
    if (s_opt_showinfo)
        icl_stats_dump ();
    icl_system_destroy ();
    return (0);
}


//  Declare a default/standard exchange
//  Uses the single amq_vhost for now

static void
s_exchange_declare (char *name, int class)
{
    amq_exchange_t
        *exchange;                      //  Predeclared exchanges            
        
    exchange = amq_exchange_new (
        amq_vhost->exchange_table,
        class,                          //  Exchange class
        name,                           //  Exchange name
        TRUE,                           //  Durable, allows durable queues
        FALSE,                          //  Do not auto-delete
        FALSE);                         //  Not internal
    assert (exchange);
    amq_exchange_unlink (&exchange);
}
