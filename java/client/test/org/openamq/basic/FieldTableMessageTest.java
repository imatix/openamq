package org.openamq.basic;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQQueue;
import org.openamq.client.message.JMSBytesMessage;
import org.openamq.framing.FieldTable;
import org.openamq.framing.AMQFrameDecodingException;
import org.openamq.framing.FieldTableTest;
import org.apache.mina.common.ByteBuffer;

import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.BytesMessage;
import java.util.ArrayList;
import java.io.IOException;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class FieldTableMessageTest implements MessageListener
{
    private final AMQConnection _connection;
    private final AMQDestination _destination;
    private final AMQSession _session;
    private final ArrayList received = new ArrayList();
    private final FieldTable _expected;

    FieldTableMessageTest  (String broker) throws Exception
    {
        this(new AMQConnection(broker, "guest", "guest", randomize("Client"), "/test_path"));
    }

    FieldTableMessageTest  (AMQConnection connection) throws Exception
    {
        this(connection, new AMQQueue(randomize("FieldTableMessageTest"), true));
    }

    FieldTableMessageTest  (AMQConnection connection, AMQDestination destination) throws Exception
    {
        _connection = connection;
        _destination = destination;
        _session = (AMQSession) connection.createSession(false, AMQSession.NO_ACKNOWLEDGE);

        //set up a slow consumer
        _session.createConsumer(destination).setMessageListener(this);
        connection.start();

        _expected = new FieldTableTest().load("FieldTableTest2.properties");
    }

    void test(int count) throws Exception
    {
        send(count);
        waitFor(count);
        check();
        System.out.println("Completed without failure");
        _connection.close();
    }

    void send(int count) throws JMSException, IOException
    {
        //create a publisher
        MessageProducer producer = _session.createProducer(_destination);
        for (int i = 0; i < count; i++)
        {
            BytesMessage msg = _session.createBytesMessage();
            msg.writeBytes(_expected.getDataAsBytes());
            producer.send(msg);
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

    void check() throws JMSException, AMQFrameDecodingException
    {
        for (Object m : received)
        {
            ByteBuffer buffer = ((JMSBytesMessage) m).getData();
            FieldTable actual = new FieldTable(buffer, buffer.remaining());
            new FieldTableTest().assertEquivalent(_expected, actual);
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
        FieldTableMessageTest   test = new FieldTableMessageTest  (argv.length == 0 ? "localhost:5672" : argv[0]);
        test.test(argv.length > 1 ? Integer.parseInt(argv[1]) : 5);
    }


}
