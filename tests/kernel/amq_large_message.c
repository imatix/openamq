//  Test case - large messages
//
#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

int
main (int argc, char *argv [])
{
    amq_client_connection_t
        *connection;
    amq_client_session_t
        *channel;
    amq_content_jms_t
        *content;
    char
        *data;
    int
        i;

    /*  Initialise system in order to use console.  */
    icl_system_initialise (argc, argv);

    /*  Connect to AMQ server */
    connection = amq_client_connection_new ("localhost:9876",
                                            amq_client_connection_auth_plain ("guest", "guest"), 
                                            0, 
                                            300000);
    channel = amq_client_session_new (connection, "/");
    amq_client_session_exchange_declare (channel, 0, "service", "dest", 0, 1, 0, 0);

    /*  Send a message */
    content = amq_content_jms_new ();
    data = icl_mem_alloc (26*60000);
    for (i = 0; i != 26*60000; i+=4)
        *((qbyte*)(data + i)) = i;

    amq_content_jms_set_body (content, data, 26*60000, icl_mem_free);
    amq_client_session_jms_publish (channel, content, 0, "service", "blabla", TRUE, TRUE);

    amq_client_session_wait (channel, 0);

    /*  Do cleanup */
    amq_client_session_destroy (&channel);
    amq_client_connection_destroy (&connection);

    icl_system_terminate ();
    return (0);
}
