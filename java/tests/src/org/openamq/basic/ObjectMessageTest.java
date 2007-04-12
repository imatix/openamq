package org.openamq.basic;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQQueue;
import org.openamq.client.message.JMSTextMessage;
import org.openamq.client.message.JMSObjectMessage;

import javax.jms.MessageListener;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.Message;
import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;
import java.io.Serializable;

public class ObjectMessageTest implements MessageListener
{
    private final AMQConnection _connection;
    private final AMQDestination _destination;
    private final AMQSession _session;
    private final List<JMSObjectMessage> received = new ArrayList<JMSObjectMessage>();
    private final List<Payload> messages = new ArrayList<Payload>();

    ObjectMessageTest(String broker) throws Exception
    {
        this(new AMQConnection(broker, "guest", "guest", randomize("Client"), "/test_path"));
    }

    ObjectMessageTest(AMQConnection connection) throws Exception
    {
        this(connection, new AMQQueue(randomize("ObjectMessageTest"), true));
    }

    ObjectMessageTest(AMQConnection connection, AMQDestination destination) throws Exception
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
            Payload payload = new Payload("Message " + i);
            messages.add(payload);
            producer.send(_session.createObjectMessage(payload));
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
        List<Object> actual = new ArrayList<Object>();
        for (JMSObjectMessage m : received)
        {
            actual.add(m.getObject());
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
            received.add((JMSObjectMessage) message);
            received.notify();
        }
    }

    private static String randomize(String in)
    {
        return in + System.currentTimeMillis();
    }

    public static void main(String[] argv) throws Exception
    {
        ObjectMessageTest test = new ObjectMessageTest(argv.length == 0 ? "localhost:5672" : argv[0]);
        test.test(argv.length > 1 ? Integer.parseInt(argv[1]) : 1000);
    }


    private static class Payload implements Serializable
    {
        private final String data;

        Payload(String data)
        {
            this.data = data;
        }

        public int hashCode()
        {
            return data.hashCode();
        }

        public boolean equals(Object o)
        {
            return o instanceof Payload && ((Payload) o).data.equals(data);
        }

        public String toString()
        {
            return "Payload[" + data +"]";
        }
    }
}
