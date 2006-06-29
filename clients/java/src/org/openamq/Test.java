package org.openamq;

import java.net.*;

import org.openamq.framing.*;
import org.openamq.transport.*;
import org.openamq.protocol.*;
import org.openamq.*;

public class Test {
    public static void main(String args[]) throws Exception {
        AuthData
            auth = AMQClientConnection.authPlain("guest", "guest");
        AMQClientConnection
            connection = new AMQClientConnection("localhost", "/", auth, "test", 0, 30000);
        AMQClientSession
            session = connection.createSession();
        AMQMessage
            message = session.createMessage();

        // Consume messages
        session.queueDeclare("aqueue");
        session.basicConsume("aqueue");

        // Send message
        message.getHeaders().setMessageId("id1");
        message.setBody("0123456789");
        session.basicPublish(message, "", "aqueue");

        // Receive message
        message = session.getMessage();
        System.out.println("Received message:");
        System.out.println(message.getHeaders().getMessageId());
        System.out.println(message.getBody().getHexDump());
    }
}
