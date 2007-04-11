package org.openamq.basic;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQQueue;
import org.openamq.client.message.JMSTextMessage;

import javax.jms.MessageListener;
import javax.jms.Message;
import javax.jms.JMSException;
import java.util.List;
import java.util.ArrayList;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class SessionStartTest implements MessageListener
{
    private final AMQConnection _connection;
    private final AMQDestination _destination;
    private final AMQSession _session;
    private final List<JMSTextMessage> received = new ArrayList<JMSTextMessage>();
    private final List<String> messages = new ArrayList<String>();
    private int count;

    SessionStartTest(String broker) throws Exception
    {
        this(new AMQConnection(broker, "guest", "guest", randomize("Client"), "/test_path"));
    }

    SessionStartTest(AMQConnection connection) throws Exception
    {
        this(connection, new AMQQueue(randomize("SessionStartTest"), true));
    }

    SessionStartTest(AMQConnection connection, AMQDestination destination) throws Exception
    {
        _connection = connection;
        _destination = destination;
        connection.start();

        _session = (AMQSession) connection.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        _session.createConsumer(destination).setMessageListener(this);
    }

    synchronized void test() throws JMSException, InterruptedException
    {
        _session.createProducer(_destination).send(_session.createTextMessage("Message"));
        System.out.println("Message sent, waiting for response...");
        wait(1000);
        if(count > 0)
        {
            System.out.println("Got message");
        }
        else
        {
            throw new RuntimeException("Did not get message!");
        }
    }

    public synchronized void onMessage(Message message)
    {
        count++;
        notify();
    }

    private static String randomize(String in)
    {
        return in + System.currentTimeMillis();
    }

    public static void main(String[] argv) throws Exception
    {
        SessionStartTest test = new SessionStartTest(argv.length == 0 ? "localhost:5672" : argv[0]);
        test.test();
    }

}
