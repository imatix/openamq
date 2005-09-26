#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

/*
    code review comments - ph, 2005/09/17
        - do not align function arguments to right
            - instead, indent them 4 spaces from first line
            - and comment arguments in column 40
            -> see amq_measure_front.c
        - spaces around ()
        - 'else' on new line, don't use "} else {"
  */

int main (int argc, char** argv)
{
    amq_client_connection_t
        *connection = NULL;             //  Current connection
    amq_client_session_t
        *session = NULL;                //  Current session
    amq_content_jms_t
        *message = NULL;                //  Current message
    icl_longstr_t
        *arguments = NULL;              //  Arguments to bind
    byte
        *payload = NULL;                //  The message payload
    int
        rc = 0;                         //  Return code from calls

    //  Initialize system in order to use console
    icl_system_initialise (argc, argv);

    //  Set up a connection
    connection = amq_client_connection_new (
        "localhost:9876",
        amq_client_connection_auth_plain("guest","guest"),
        0,      //  Trace level
        30000); //  Timeout

    if (connection) 
        icl_console_print("I: connected to: %s/%s",
                          connection->server_product,
                          connection->server_version); 
    else {
        icl_console_print("E: could not connect to server"); 
        goto finished;
    }


    //  Set up a session
    session = amq_client_session_new (connection,"/");

    if (!session) {
        icl_console_print("E: could not open session to server");
        goto finished;
    }

    //  Declare a queue
    amq_client_session_queue_declare (
        session,      //  The session
        0,            //  Access ticked granted by server
        "global",     //  Queue scope
        "service",    //  Queue name
        FALSE,        //  Passive declaration
        FALSE,        //  Durable declaration
        FALSE,        //  Private queue
        FALSE);       //  Auto delete

   
    //  Bind the queue to the exchange (named queue)
    arguments = asl_field_list_build ("destination", "service", NULL);
    amq_client_session_queue_bind (
        session,
        0,               //  Ticket
        "global",        //  Queue scope
        "service",       //  Queue
        "queue",         //  Exchange
        arguments);      //  Arguments
    icl_longstr_destroy (&arguments);

    amq_client_session_jms_consume (
        session,
        0,                          //  Access ticket granted by server
        "global",                   //  Queue scope
        "service",                  //  Queue name
        0,                          //  Prefetch size
        0,                          //  Prefetch count
        FALSE,                      //  No local messages
        TRUE,                       //  Auto-acknowledge
        FALSE);                     //  Exclusive access to queue

    for (;;) {
        if (amq_client_session_wait (session, 0))
            break;
        
        while ((message = amq_client_session_jms_arrived (session)) != NULL) {
            icl_console_print ("I: [%s] message {%s} arrived",
                "service", message->message_id);
            rc = amq_client_session_jms_publish (
                session,
                message,
                0,                 //  Ticket
                "queue",           //  Exchange
                message->reply_to, //  Destination
                FALSE,             //  Mandatory
                FALSE);            //  Immediate
            if (rc)
                break;
        }
    }
    //  Clean up
    finished:
    amq_client_session_destroy (&session);
    amq_client_connection_destroy (&connection);

    icl_mem_free (payload);
    icl_system_terminate();
    return 0;
}
