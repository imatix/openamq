
#include "amq_client_classes.hpp"

int main (int argc, char *argv [])
{
    IclInitialise (argc, argv);

    //  create a connection to AMQ server
    AmqClientConnection connection ("127.0.0.1", "/",
        AmqClientConnection::AuthPlain ("guest", "guest"), 3, 30000);
    AmqClientSession session (connection);

    //  create and bind a queue
    session.QueueDeclare (0, "queue", false, false, false, true, IclLongstr (NULL, 0));
    session.QueueBind (0, "queue", "amq.direct", "key", IclLongstr (NULL, 0));

    //  start consuming messages from the queue
    session.BasicConsume (0, "queue", NULL, 0, 0, false, true, false);

    //  build a message
    AmqContentBasic content;
    void *content_body = icl_mem_alloc (1);
    *(char*) content_body = '0';
    content.SetBody (content_body, 1, icl_mem_free);
    AslFieldList field_list (IclLongstr (NULL, 0));
    AslField::NewInteger (field_list, "integer", 20);
    AslField::NewString (field_list, "routing_key", "key");
    IclLongstr headers (field_list.Flatten ());
    content.SetHeaders (headers);

    //  send the message to the queue
    session.BasicPublish (content, 0, "amq.direct", "key", false, false);

    //  process the message
    session.Wait (0);
    content = session.BasicArrived ();

    return 0;
}
