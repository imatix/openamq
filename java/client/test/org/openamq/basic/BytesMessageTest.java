package org.openamq.basic;

import org.apache.mina.common.ByteBuffer;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import org.openamq.client.AMQQueue;
import org.openamq.client.AMQSession;
import org.openamq.client.message.JMSBytesMessage;

import javax.jms.BytesMessage;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class BytesMessageTest implements MessageListener
{
    private final AMQConnection _connection;
    private final AMQDestination _destination;
    private final AMQSession _session;
    private final List<JMSBytesMessage> received = new ArrayList<JMSBytesMessage>();
    private final List<byte[]> messages = new ArrayList<byte[]>();

    BytesMessageTest(String broker) throws Exception
    {
        this(new AMQConnection(broker, "guest", "guest", randomize("Client"), "/test_path"));
    }

    BytesMessageTest(AMQConnection connection) throws Exception
    {
        this(connection, new AMQQueue(randomize("BytesMessageTest"), true));
    }

    BytesMessageTest(AMQConnection connection, AMQDestination destination) throws Exception
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
        try
        {
            send(count);
            waitFor(count);
            check();
            System.out.println("Completed without failure");
        }
        finally
        {
            _connection.close();
        }
    }

    void send(int count) throws JMSException
    {
        //create a publisher
        MessageProducer producer = _session.createProducer(_destination);
        for (int i = 0; i < count; i++)
        {
            BytesMessage msg = _session.createBytesMessage();
            byte[] data = new String("Message " + i).getBytes();
            msg.writeBytes(data);
            messages.add(data);
            producer.send(msg);
        }
    }

    void waitFor(int count) throws InterruptedException
    {
        synchronized (received)
        {
            while (received.size() < count)
            {
                received.wait();
            }
        }
    }

    void check() throws JMSException
    {
        List<byte[]> actual = new ArrayList<byte[]>();
        for (JMSBytesMessage m : received)
        {
            ByteBuffer buffer = m.getData();
            byte[] data = new byte[buffer.remaining()];
            buffer.get(data);
            actual.add(data);
        }

        assertEqual(messages.iterator(), actual.iterator());
    }

    private static void assertEqual(Iterator expected, Iterator actual)
    {
        List<String> errors = new ArrayList<String>();
        while (expected.hasNext() && actual.hasNext())
        {
            try
            {
                assertEquivalent((byte[]) expected.next(), (byte[]) actual.next());
            }
            catch (Exception e)
            {
                errors.add(e.getMessage());
            }
        }
        while (expected.hasNext())
        {
            errors.add("Expected " + expected.next() + " but no more actual values.");
        }
        while (actual.hasNext())
        {
            errors.add("Found " + actual.next() + " but no more expected values.");
        }
        if (!errors.isEmpty())
        {
            throw new RuntimeException(errors.toString());
        }
    }

    private static void assertEquivalent(byte[] expected, byte[] actual)
    {
        if (expected.length != actual.length)
        {
            throw new RuntimeException("Expected length " + expected.length + " got " + actual.length);
        }
        for (int i = 0; i < expected.length; i++)
        {
            if (expected[i] != actual[i])
            {
                throw new RuntimeException("Failed on byte " + i + " of " + expected.length);
            }
        }
    }

    private static void assertEqual(Object expected, Object actual)
    {
        if (!expected.equals(actual))
        {
            throw new RuntimeException("Expected '" + expected + "' found '" + actual + "'");
        }
    }

    public void onMessage(Message message)
    {
        synchronized (received)
        {
            received.add((JMSBytesMessage) message);
            received.notify();
        }
    }

    private static String randomize(String in)
    {
        return in + System.currentTimeMillis();
    }

    public static void main(String[] argv) throws Exception
    {
        BytesMessageTest test = new BytesMessageTest(argv.length == 0 ? "localhost:5672" : argv[0]);
        test.test(argv.length > 1 ? Integer.parseInt(argv[1]) : 100);
    }

}
