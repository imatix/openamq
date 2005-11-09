
#include "amq_client_classes.hpp"

void test ()
{
    //  create a connection to AMQ server
    IclLongstr auth = AmqClientConnection::AuthPlain ("guest", "guest");
    AmqClientConnection connection ("127.0.0.1", "/", auth, 3, 30000);
    AmqClientSession session (connection);

    //  create and bind a queue
    session.QueueDeclare (0, "scope", "queue", false, false, false, true);
    IclLongstr emptystr (NULL, 0);
    session.QueueBind (0, "scope", "queue", "amq.direct", "key", emptystr);

    //  start consuming messages from the queue
    session.JmsConsume (0, "scope", "queue", 0, 0, false, true, false);

    //  build a message
    AmqContentJms content;
    void *content_body = icl_mem_alloc (1);
    *(char*)content_body = '0';
    content.SetBody (content_body, 1, icl_mem_free);
    AslFieldList field_list (emptystr);
    AslField::NewInteger (field_list, "integer", 20);
    AslField::NewString (field_list, "routing_key", "key");
    IclLongstr headers (field_list.Flatten ());
    content.SetHeaders (headers);

    //  send the message to the queue
    session.JmsPublish (content, 0, "amq.direct", "key", false, false);

    //  process the message
    session.Wait (0);
    content = session.JmsArrived ();
}

int main (int argc, char *argv [])
{
    icl_system_initialise (argc, argv);
    test ();
    icl_system_terminate ();
    return 0;
}
