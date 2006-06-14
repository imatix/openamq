/* test multiple declare binds */
#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

int main (int argc, char** argv)
{
    amq_client_connection_t
        *connection = NULL;             //  Current connection
    amq_client_session_t
        *session = NULL;                //  Current session
    byte
        *payload = NULL;                //  The message payload

    //  Initialize system in order to use console
    icl_system_initialise (argc, argv);

    //  Set up a connection
    connection = amq_client_connection_new (
        "localhost:9876",
        "/",
        amq_client_connection_auth_plain ("guest","guest"),
        3,                              //  Trace level
        30000);                         //  Timeout

    if (connection)
        icl_console_print("I: connected to: %s/%s",
            connection->server_product, connection->server_version);
    else {
        icl_console_print("E: could not connect to server");
        goto finished;
    }

    //  Set up a session
    session = amq_client_session_new (connection);
    if (!session) {
        icl_console_print("E: could not open session to server");
        goto finished;
    }

    //  Declare a queue for replies from back-end
    amq_client_session_queue_declare (
        session,
        0,                              //  Access ticked granted by server
        "global",                       //  Queue scope
        NULL,                           //  Queue name
        FALSE,                          //  Passive declare?
        FALSE,                          //  Durable queue?
        FALSE,                          //  Private queue?
        FALSE);                         //  Auto delete?

    //  Clean up
    finished:
    amq_client_session_destroy (&session);
    amq_client_connection_destroy (&connection);

    icl_mem_free (payload);
    icl_system_terminate();
    return 0;
}
