package org.openamq.fragmentation;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQTopic;
import org.openamq.client.AMQQueue;
import org.openamq.jms.Session;

import javax.jms.*;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestLargeSubscriber
{
    private static final Logger _logger = Logger.getLogger(TestLargeSubscriber.class);

    private static MessageProducer _destinationProducer;

    private static String _destinationName;

    public static void main(String[] args)
    {
        _logger.info("Starting...");

        if (args.length != 6)
        {
            System.out.println("Usage: host port username password virtual-path expectedMessageCount");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            AMQConnection con = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                                  address.getHostName(), args[4]);            
            final Session session = (Session) con.createSession(false, Session.AUTO_ACKNOWLEDGE);

            final int expectedMessageCount = Integer.parseInt(args[5]);

            MessageConsumer consumer = session.createConsumer(new AMQTopic("large", false),
                                                              100, false, true, true, null);

            consumer.setMessageListener(new MessageListener()
            {
                private int _messageCount;

                private long _startTime = 0;

                public void onMessage(Message message)
                {
                    validateMessage(message);
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

                private void validateMessage(Message message)
                {
                    if (!(message instanceof BytesMessage))
                    {
                        _logger.error("Message is not of correct type - should be BytesMessage and is " +
                                      message.getClass());
                    }
                    BytesMessage bm = (BytesMessage) message;
                    final int expectedSize = 1024 * 187; // 187k
                    try
                    {
                        if (bm.getBodyLength() != expectedSize)
                        {
                            _logger.error("Message is not correct length - should be  " + expectedSize + " and is " +
                                          bm.getBodyLength());
                        }
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Failed to validate message: " + e, e);
                    }
                    try
                    {
                        byte[] data = new byte[(int)bm.getBodyLength()];
                        bm.readBytes(data);
                        for (int i = 0; i < data.length; i++)
                        {
                            if (data[i] != (byte)(i%25))
                            {
                                _logger.error("byte " + i + " of message is wrong - should be " + i%25 + " but is " +
                                              data[i]);
                            }
                        }
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Failed to validate message: " + e, e);
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

