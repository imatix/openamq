package org.openamq.basic;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import org.openamq.client.AMQQueue;
import org.openamq.client.AMQSession;
import org.openamq.client.message.JMSTextMessage;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import java.util.ArrayList;
import java.util.List;
import java.util.Iterator;

public class TextMessageTest implements MessageListener
{
    private final AMQConnection _connection;
    private final AMQDestination _destination;
    private final AMQSession _session;
    private final List<JMSTextMessage> received = new ArrayList<JMSTextMessage>();
    private final List<String> messages = new ArrayList<String>();

    TextMessageTest(String broker) throws Exception
    {
        this(new AMQConnection(broker, "guest", "guest", randomize("Client"), "/test_path"));
    }

    TextMessageTest(AMQConnection connection) throws Exception
    {
        this(connection, new AMQQueue(randomize("TextMessageTest"), true));
    }

    TextMessageTest(AMQConnection connection, AMQDestination destination) throws Exception
    {
        _connection = connection;
        _destination = destination;
        _session = (AMQSession) connection.createSession(false, AMQSession.NO_ACKNOWLEDGE);

        //set up a slow consumer
        _session.createConsumer(destination).setMessageListener(this);
        connection.start();
    }

    void test(int count) throws Exception
    {
        send(count);
        waitFor(count);
        check();
        System.out.println("Completed without failure");
        _connection.close();
    }

    void send(int count) throws JMSException
    {
        //create a publisher
        MessageProducer producer = _session.createProducer(_destination);
        for (int i = 0; i < count; i++)
        {
            String text = "Message " + i;
            messages.add(text);
            producer.send(_session.createTextMessage(text));
        }
    }

    void waitFor(int count) throws InterruptedException
    {
        synchronized(received)
        {
            while(received.size() < count)
            {
                received.wait();
            }
        }
    }

    void check() throws JMSException
    {
        List<String> actual = new ArrayList<String>();
        for (JMSTextMessage m : received)
        {
            actual.add(m.getText());
        }

        assertEqual(messages.iterator(), actual.iterator());
    }

    private static void assertEqual(Iterator expected, Iterator actual)
    {
        List<String> errors = new ArrayList<String>();
        while(expected.hasNext() && actual.hasNext())
        {
            try{
                assertEqual(expected.next(), actual.next());
            }
            catch(Exception e)
            {
                errors.add(e.getMessage());
            }
        }
        while(expected.hasNext())
        {
            errors.add("Expected " + expected.next() + " but no more actual values.");
        }
        while(actual.hasNext())
        {
            errors.add("Found " + actual.next() + " but no more expected values.");
        }
        if(!errors.isEmpty())
        {
            throw new RuntimeException(errors.toString());
        }
    }

    private static void assertEqual(Object expected, Object actual)
    {
        if(!expected.equals(actual))
        {
            throw new RuntimeException("Expected '" + expected + "' found '" + actual + "'");
        }
    }

    public void onMessage(Message message)
    {
        synchronized(received)
        {
            received.add((JMSTextMessage) message);
            received.notify();
        }
    }

    private static String randomize(String in)
    {
        return in + System.currentTimeMillis();
    }

    public static void main(String[] argv) throws Exception
    {
        TextMessageTest test = new TextMessageTest(argv.length == 0 ? "localhost:5672" : argv[0]);
        test.test(argv.length > 1 ? Integer.parseInt(argv[1]) : 100);
    }

}
