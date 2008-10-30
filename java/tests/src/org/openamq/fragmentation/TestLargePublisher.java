package org.openamq.fragmentation;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.AMQException;
import org.openamq.client.AMQTopic;
import org.openamq.jms.MessageProducer;
import org.openamq.jms.Session;

import javax.jms.*;
import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * A client that behaves as follows:
 * <ul><li>Connects to a queue, whose name is specified as a cmd-line argument</li>
 * <li>Creates a temporary queue</li>
 * <li>Creates messages containing a property that is the name of the temporary queue</li>
 * <li>Fires off a message on the original queue and waits for a response on the temporary queue</li>
 * </ul>
 *
 */
public class TestLargePublisher
{
    private static final Logger _log = Logger.getLogger(TestLargePublisher.class);

    private AMQConnection _connection;

    private Session _session;

    private class CallbackHandler implements MessageListener
    {
        private int _expectedMessageCount;

        private int _actualMessageCount;

        private long _startTime;

        public CallbackHandler(int expectedMessageCount, long startTime)
        {
            _expectedMessageCount = expectedMessageCount;
            _startTime = startTime;
        }

        public void onMessage(Message m)
        {
            if (_log.isDebugEnabled())
            {
                _log.debug("Message received: " + m);
            }
            _actualMessageCount++;
            if (_actualMessageCount%1000 == 0)
            {
                _log.info("Received message count: " + _actualMessageCount);
            }
            /*if (!"henson".equals(m.toString()))
           {
               _log.error("AbstractJMSMessage response not correct: expected 'henson' but got " + m.toString());
           }
           else
           {
               if (_log.isDebugEnabled())
               {
                   _log.debug("AbstractJMSMessage " + m + " received");
               }
               else
               {
                   _log.info("AbstractJMSMessage received");
               }
           } */

            if (_actualMessageCount == _expectedMessageCount)
            {
                long timeTaken = System.currentTimeMillis() - _startTime;
                System.out.println("Total time taken to receive " + _expectedMessageCount+ " messages was " +
                                   timeTaken + "ms, equivalent to " +
                                   (_expectedMessageCount/(timeTaken/1000.0)) + " messages per second");
            }
        }
    }

    public TestLargePublisher(String host, int port, String clientID,
                              final int messageCount) throws AMQException
    {
        try
        {
            createConnection(host, port, clientID);
            
            _session = (Session) _connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            AMQTopic destination = new AMQTopic("large");
            MessageProducer producer = (MessageProducer) _session.createProducer(destination);

            _connection.start();
            //TextMessage msg = _session.createTextMessage(tempDestination.getQueueName() + "/Presented to in conjunction with Mahnah Mahnah and the Snowths");
            final long startTime = System.currentTimeMillis();

            for (int i = 0; i < messageCount; i++)
            {
                BytesMessage msg = _session.createBytesMessage();
                populateMessage(msg);
                producer.send(msg);
            }
            _log.info("Finished sending " + messageCount + " messages");
        }
        catch (JMSException e)
        {
            e.printStackTrace();
        }
    }

    private void createConnection(String host, int port, String clientID) throws AMQException
    {
        _connection = new AMQConnection(host, port, "guest", "guest",
                                        clientID, "/test");
    }

    private void populateMessage(BytesMessage msg) throws JMSException
    {
        int size = 1024 * 187; // 187k
        byte[] data = new byte[size];
        for (int i = 0; i < data.length; i++)
        {
            data[i] = (byte)(i%25);
        }
        msg.writeBytes(data);
    }

    /**
     *
     * @param args argument 1 if present specifies the name of the temporary queue to create. Leaving it blank
     * means the server will allocate a name.
     */
    public static void main(String[] args)
    {
        if (args.length == 0)
        {
            System.err.println("Usage: TestLargePublisher <host> <port> <number of messages>");
        }
        try
        {
            int port = Integer.parseInt(args[1]);
            InetAddress address = InetAddress.getLocalHost();
            String clientID = address.getHostName() + System.currentTimeMillis();
            TestLargePublisher client = new TestLargePublisher(args[0], port, clientID, Integer.parseInt(args[2]));
        }
        catch (UnknownHostException e)
        {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
        catch (AMQException e)
        {
            System.err.println("Error in client: " + e);
            e.printStackTrace();
        }

        //System.exit(0);
    }
}
