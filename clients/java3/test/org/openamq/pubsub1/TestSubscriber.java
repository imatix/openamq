package org.openamq.pubsub1;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQTopic;
import org.openamq.jms.Session;

import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestSubscriber
{
    private static final Logger _logger = Logger.getLogger(TestSubscriber.class);

    private static MessageProducer _destinationProducer;

    private static String _destinationName;

    public static void main(String[] args)
    {
        _logger.info("Starting...");

        if (args.length != 7)
        {
            System.out.println("Usage: host port username password virtual-path selector expectedMessageCount");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            AMQConnection con = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                                  address.getHostName(), args[4]);            
            final org.openamq.jms.Session session = (org.openamq.jms.Session) con.createSession(false, Session.AUTO_ACKNOWLEDGE);

            String selector = args[5];

            final int expectedMessageCount = Integer.parseInt(args[6]);
            _logger.info("Message selector is <" + selector + ">...");

            MessageConsumer consumer = session.createConsumer(new AMQTopic("cbr"),
                                                              100, false, true, true, selector);

            consumer.setMessageListener(new MessageListener()
            {
                private int _messageCount;

                private long _startTime = 0;

                public void onMessage(javax.jms.Message message)
                {
                    if (_messageCount++ == 0)
                    {
                        _startTime = System.currentTimeMillis();
                    }
                    if (_logger.isInfoEnabled())
                    {
                        _logger.info("Got message '" + message + "'");
                    }
                    if (_messageCount == expectedMessageCount)
                    {
                        long totalTime = System.currentTimeMillis() - _startTime;
                        _logger.error("Total time to receive " + _messageCount + " messages was " +
                                      totalTime + "ms. Rate is " + (_messageCount/(totalTime/1000.0)));
                    }
                }
            });
            con.start();
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }

        System.out.println("Waiting...");
    }
}

