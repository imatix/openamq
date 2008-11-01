//
//  im_receiver.c - Receives messages from a chatroom
//  By iMatix Corporation, April 2008.  Code released into the public domain.
//
//  Name:     im_receiver
//  Usage:    im_receiver <broker-addeess> <chatroom>
//  Example:  im_receiver 127.0.0.1:5672 "OpenAMQ discussion"
//  Receives messages from the chatroom and writed them to stdout
//  To send messages to the chat room, use im_sender application

#include "base.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

int main (int argc, char *argv [])
{
    amq_client_connection_t
        *connection;
    amq_client_session_t
        *session;
    icl_longstr_t
        *auth_data;
    amq_content_basic_t
        *content;
    char
        message_text [1024];
    size_t
        message_size;


    assert (argc == 3);

    //  Initialise system
    icl_system_initialise (argc, argv);

    //  Open a connection
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    connection = amq_client_connection_new (
            argv [1], "/", auth_data, "im_receiver", 0, 30000);
    assert (connection);
    icl_longstr_destroy (&auth_data);

    //  Open a channel
    session = amq_client_session_new (connection);
    assert (session);

    //  Create a private queue
    amq_client_session_queue_declare (
        session,                        //  session
        0,                              //  ticket
        NULL,                           //  queue name
        FALSE,                          //  passive
        FALSE,                          //  durable
        TRUE,                           //  exclusive
        TRUE,                           //  auto-delete
        NULL);                          //  arguments

    //  Bind the queue to the exchange
    amq_client_session_queue_bind (
        session,                        //  session
        0,                              //  ticket
        NULL,                           //  queue
        "amq.direct",                   //  exchange
        argv [2],                       //  routing-key
        NULL);                          //  arguments

    //  Consume from the queue
    amq_client_session_basic_consume (
        session,                        //  session
        0,                              //  ticket
        NULL,                           //  queue
        NULL,                           //  consumer-tag
        TRUE,                           //  no-local
        TRUE,                           //  no-ack
        TRUE,                           //  exclusive
        NULL);                          //  arguments

    while (1) {

        while (1) {

            //  Get next message
            content = amq_client_session_basic_arrived (session);
            if (!content)
                break;

            //  Get the message body and write it to stdout
            message_size = amq_content_basic_get_body (content,
                  (byte*) message_text, sizeof (message_text));
            if (message_size) {
                message_text [message_size] = 0;
                fputs (message_text, stdout);
            }

            //  Destroy the message
            amq_content_basic_unlink (&content);
        }
        
        //  Wait while next message arrives
        amq_client_session_wait (session, 0);

        //  Exit the loop if Ctrl+C is encountered
        if (!connection->alive)
            break;
    }

    //  Close the channel
    amq_client_session_destroy (&session);

    //  Close the connection
    amq_client_connection_destroy (&connection);

    //  Uninitialise system
    icl_system_terminate ();

    return 0;
}

