/*===========================================================================
    failover_test.c - demo of high-availability failover in C
    
    To run, start two OpenAMQ servers, one as primary on port 5672 and one
    as backup on port 6666.  You can then stop and restart servers as you
    like and this program will report the status of the HA failover pair.

    By iMatix Corporation, April 2008.  Code released into the public domain.
 *===========================================================================*/

#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

//  Establish or reestablish connection and session
static int s_establish_session (char *server_name);

//  Static source-global variables
static amq_client_connection_t
    *s_connection = NULL;               //  Current connection
static amq_client_session_t
    *s_session = NULL;                  //  Current session

int
main (int argc, char *argv [])
{
    int
        status = -1;                    //  Show status changes only
        
    //  Initialise iCL system
    icl_system_initialise (argc, argv);

    FOREVER {
        if (s_establish_session ("localhost:5672 localhost:6666")) {
            if (status != 0) {
                status = 0;
                puts ("Neither server is reachable at present");
            }
            apr_sleep (1000 * 1000);    //  Wait one second and retry
        }
        else
        if (streq (s_connection->server_port, "5672")) {
            if (status != 1) {
                puts ("Connected to primary server");
                status = 1;
            }
        }
        else {
            if (status != 2) {
                puts ("Now connected to backup server");
                status = 2;
            }
        }
        //  Wait for something to happen on the session, if it's active
        if (s_session)
            amq_client_session_wait (s_session, 0);
        //  Now check if the server disappeared (without an error)
        if (s_connection && !s_connection->alive) {
            if (s_connection->reply_code == 100) {
                puts ("W: connection to server was lost, failing over");
                amq_client_session_destroy (&s_session);
                amq_client_connection_destroy (&s_connection);
            }
            else {
                if (s_session)
                    printf ("E: %d - %s\n", s_session->reply_code, s_session->reply_text);
                else
                    printf ("E: %d - %s\n", s_connection->reply_code, s_connection->reply_text);
                break;                      //  Exit if we got a real error
            }
        }
    }
    //  Clean up and exit
    amq_client_session_destroy (&s_session);
    amq_client_connection_destroy (&s_connection);
    icl_system_terminate ();
    return (0);
}


//  Establish connection and session
//

static int s_establish_session (char *server_name)
{
    icl_longstr_t
        *auth_data;                     //  Login authorisation
    ipr_token_list_t
        *host_list;                     //  List of known hosts
    ipr_token_t
        *token;                         //  Next host to try
    int
        rc = 0;                         //  Return code

    //  Both connection and session must be null when we start
    assert (!s_connection && !s_session);
    
    //  Login using default guest credentials
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    
    //  Split host name into tokens, and check we have one or two names
    host_list = ipr_token_split (server_name);
    assert (ipr_token_list_count (host_list) == 1 
         || ipr_token_list_count (host_list) == 2);

    token = ipr_token_list_first (host_list);
    while (token) {
        s_connection = amq_client_connection_new (
            token->value, "/", auth_data, "failover test", 0, 30000);
        if (s_connection) {
            ipr_token_unlink (&token);
            break;
        }
        token = ipr_token_list_next (&token);
    }
    ipr_token_list_destroy (&host_list);
    icl_longstr_destroy (&auth_data);

    if (s_connection) {
        s_session = amq_client_session_new (s_connection);
        if (!s_session) {
            puts ("E: could not open session to server");
            rc = -1;
        }
    }
    else
        rc = -1;

    return (rc);
}

