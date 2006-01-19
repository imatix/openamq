package org.openamq.requestreply1;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.openamq.jms.Session;

import javax.jms.*;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ServiceProvidingClient
{
    private static final Logger _logger = Logger.getLogger(ServiceProvidingClient.class);

    private static MessageProducer _destinationProducer;

    private static Destination _responseDest;

    public static void main(String[] args)
    {
        _logger.info("Starting...");

        if (args.length < 6)
        {
            System.out.println("Usage: host port username password virtual-path serviceQueue [selector]");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            AMQConnection con = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                                  address.getHostName(), args[4]);
            final org.openamq.jms.Session session = (org.openamq.jms.Session) con.createSession(false,
                                                                                                Session.NO_ACKNOWLEDGE);

            String serviceName = args[5];

            _logger.info("Service (queue) name is '" + serviceName + "'...");

            AMQQueue destination = new AMQQueue(serviceName);

            String selector = (args.length >= 7 && args[6].length() > 1) ? args[6] : null;

            _logger.info("Message selector is <" + selector + ">...");

            MessageConsumer consumer = session.createConsumer(destination,
                                                              100, true, false, selector);

            consumer.setMessageListener(new MessageListener()
            {
                private int _messageCount;

                public void onMessage(javax.jms.Message message)
                {
                    //_logger.info("Got message '" + message + "'");

                    TextMessage tm = (TextMessage) message;

                    try
                    {
                        Destination responseDest = tm.getJMSReplyTo();
                        if (!responseDest.equals(_responseDest))
                        {
System.out.println("before producer");
                            _responseDest = responseDest;

                            try
                            {
                                _logger.debug("About to create a producer");
                                _destinationProducer = session.createProducer(responseDest);
                                _destinationProducer.setDisableMessageTimestamp(true);
                                _destinationProducer.setDeliveryMode(DeliveryMode.NON_PERSISTENT);
                                _logger.debug("After create a producer");
                            }
                            catch (JMSException e)
                            {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                            }
System.out.println("after producer");
                        }
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Error creating destination");
                    }
                    _messageCount++;
                    if (_messageCount % 1000 == 0)
                    {
                        _logger.info("Received message total: " + _messageCount);
                        _logger.info("Sending response to '" + _responseDest + "'");
                    }

                    try
                    {
                        String payload = "This is a response: sing together: 'Mahnah mahnah...'" + tm.getText();
                        TextMessage msg = session.createTextMessage(payload);
                        if (tm.propertyExists("timeSent"))
                        {
                            _logger.info("timeSent property set on message");
                            _logger.info("timeSent value is: " + tm.getLongProperty("timeSent"));
                            msg.setStringProperty("timeSent", tm.getStringProperty("timeSent"));
                        }
                        _destinationProducer.send(msg);
                        if (_messageCount % 1000 == 0)
                        {
                            _logger.info("Sent response to '" + _responseDest + "'");
                        }
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Error sending message: " + e, e);
                    }
                }
            });
            con.start();
            System.out.println("Waiting...");
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }
    }

}

