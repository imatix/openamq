package org.openamq.requestreply1;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.openamq.jms.MessageConsumer;
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
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ServiceRequestingClient
{
    private static final String MESSAGE_DATA_BYTES = "jfd ghljgl hjvhlj cvhvjf ldhfsj lhfdsjf hldsjfk hdslkfj hsdflk  ";

    private static final String MESSAGE_DATA;

    private static final Logger _log = Logger.getLogger(ServiceRequestingClient.class);

    private AMQConnection _connection;

    private Session _session;

    private long _averageLatency;

    static
    {
        StringBuffer buf = new StringBuffer(4096);
        for (int i = 0; i < 63; i++)
        {
            buf.append(MESSAGE_DATA_BYTES);
        }
        MESSAGE_DATA = buf.toString();
    }

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
            try
            {
                if (m.propertyExists("timeSent"))
                {
                    long timeSent = Long.parseLong(m.getStringProperty("timeSent"));
                    long now = System.currentTimeMillis();
                    if (_averageLatency == 0)
                    {
                        _averageLatency = now - timeSent;
                        _log.info("Latency " + _averageLatency);
                    }
                    else
                    {
                        _log.info("Individual latency: " + (now-timeSent));
                        _averageLatency = (_averageLatency + (now - timeSent))/2;
                        _log.info("Average latency now: " + _averageLatency);
                    }
                }
            }
            catch (JMSException e)
            {
                _log.error("Error getting latency data: " + e, e);
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
                try
                {
                    _connection.close();
                    _log.info("Connection closed");
                }
                catch (JMSException e)
                {
                    _log.error("Error closing connection");
                }
            }
        }
    }

    public ServiceRequestingClient(String brokerHosts, String clientID, String username, String password,
                                   String vpath, String commandQueueName,
                                   final int messageCount) throws AMQException
    {
        try
        {
            createConnection(brokerHosts, clientID, username, password, vpath);
            _session = (Session) _connection.createSession(false, Session.NO_ACKNOWLEDGE);
            AMQQueue destination = new AMQQueue(commandQueueName);
            MessageProducer producer = (MessageProducer) _session.createProducer(destination);
            producer.setDisableMessageTimestamp(true);
            producer.setDeliveryMode(DeliveryMode.NON_PERSISTENT);

            AMQQueue tempDestination = new AMQQueue("TempResponse" +
                                                    Long.toString(System.currentTimeMillis()), true);
            MessageConsumer messageConsumer = (MessageConsumer) _session.createConsumer(tempDestination, 100, true,
                                                                                        true, null);

            final long startTime = System.currentTimeMillis();

            messageConsumer.setMessageListener(new CallbackHandler(messageCount, startTime));
            _connection.start();
            for (int i = 0; i < messageCount; i++)
            {
                //TextMessage msg = _session.createTextMessage("Presented to in conjunction with Mahnah Mahnah and the Snowths:" + i);
                TextMessage msg = _session.createTextMessage(MESSAGE_DATA + i);
                msg.setJMSReplyTo(tempDestination);
                if (i%1000 == 0)
                {
                    long timeNow = System.currentTimeMillis();
                    msg.setStringProperty("timeSent", String.valueOf(timeNow));
                }
                producer.send(msg);
            }
            _log.info("Finished sending " + messageCount + " messages");
        }
        catch (JMSException e)
        {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
    }

    private void createConnection(String brokerHosts, String clientID, String username, String password,
                                  String vpath) throws AMQException
    {
        _connection = new AMQConnection(brokerHosts, username, password,
                                        clientID, vpath);
    }

    /**
     *
     * @param args argument 1 if present specifies the name of the temporary queue to create. Leaving it blank
     * means the server will allocate a name.
     */
    public static void main(String[] args)
    {
        if (args.length < 6)
        {
            System.err.println("Usage: ServiceRequestingClient <brokerDetails - semicolon separated host:port list> <username> <password> <vpath> <command queue name> <number of messages>");
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            String clientID = address.getHostName() + System.currentTimeMillis();
            ServiceRequestingClient client = new ServiceRequestingClient(args[0],clientID, args[1], args[2], args[3],
                                                                         args[4], Integer.parseInt(args[5]));
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
    }
}
