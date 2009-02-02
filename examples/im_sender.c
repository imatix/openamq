//
//  im_sender.c - Sends messages to the chatroom
//  By iMatix Corporation, April 2008.  Code released into the public domain.
//
//  Name:     im_sender
//  Usage:    im_sender <broker-addeess> <chatroom> <your-name>
//  Example:  im_sender 127.0.0.1:5672 "OpenAMQ discussion" "Baron Bartholomaeus von Saburg-Fridetzki"
//  Sends messages from stdio to the chat room 
//  To receive messages from the chat room, use im_receiver application

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
    char
        *message_body;
        
    assert (argc == 4);

    //  Initialise system
    icl_system_initialise (argc, argv);

    //  Open a connection
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    connection = amq_client_connection_new (
            argv [1], "/", auth_data, "im_sender", 0, 30000);
    assert (connection);
    icl_longstr_destroy (&auth_data);

    //  Open a channel
    session = amq_client_session_new (connection);
    assert (session);

    while (1) {

        //  Read one line from stdin
        fgets (message_text, sizeof (message_text), stdin);

        //  Exit the loop if Ctrl+C is encountered
        if (!connection->alive)
            break;

        //  Create the message body
        message_body =
            malloc (strlen (argv [3]) + 2 + strlen (message_text) + 1);
        assert (message_body);
        sprintf (message_body, "%s: %s", argv [3], message_text);

        //  Create the message itself
        content = amq_content_basic_new ();
//        amq_content_basic_set_body (content, message_body,
  //          strlen (message_body), free);

        //  Send the message
        amq_client_session_basic_publish (
            session,                        //  session
            content,                        //  content to send
            0,                              //  ticket
            "amq.direct",                   //  exchange to send message to
            argv [2],                       //  routing-key
            FALSE,                          //  mandatory
            FALSE);                         //  immediate

        //  Release the message
        amq_content_basic_unlink (&content);
    }

    //  Close the channel
    amq_client_session_destroy (&session);

    //  Close the connection
    amq_client_connection_destroy (&connection);

    //  Uninitialise system
    icl_system_terminate ();

    return 0;
}

