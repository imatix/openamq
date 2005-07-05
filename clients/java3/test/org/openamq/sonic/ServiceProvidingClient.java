/**
 * Date Created: 13-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.sonic;

import org.apache.log4j.Logger;

import javax.jms.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ServiceProvidingClient
{

    private static final Logger _logger = Logger.getLogger(ServiceProvidingClient.class);

    private static MessageProducer _destinationProducer;

    private static Queue _destinationQ;

    public static void main(String[] args)
    {
        _logger.info("Starting...");

        if (args.length != 2)
        {
            System.out.println("Usage: <WLS URI> <service queue>");
            System.exit(1);
        }
        try
        {
            String url = args[0];
            String receiveQueue = args[1];

            //QueueConnectionFactory qconFactory = (QueueConnectionFactory) ctx.lookup(JMS_FACTORY);
            progress.message.jclient.ConnectionFactory qconFactory = new progress.message.jclient.ConnectionFactory(url);
            qconFactory.setLoadBalancing(false);
            Connection qcon = qconFactory.createConnection();
            Connection qcon2 = qconFactory.createConnection();
            final Session qsession = qcon.createSession(false, Session.CLIENT_ACKNOWLEDGE);
            ((progress.message.jclient.Session)qsession).setFlowControlDisabled(true);
            final Session qsession2 = qcon2.createSession(false, Session.CLIENT_ACKNOWLEDGE);
            ((progress.message.jclient.Session)qsession2).setFlowControlDisabled(true);
            Queue receiveQ = qsession.createQueue(receiveQueue);

            _logger.info("Service (queue) name is '" + receiveQ + "'...");

            String selector = (args.length > 2 && args[2] != null && args[2].length() > 1) ? args[2] : null;

            _logger.info("Message selector is <" + selector + ">...");

            MessageConsumer consumer = qsession.createConsumer(receiveQ, selector);
            ((progress.message.jclient.MessageConsumer)consumer).setPrefetchCount(200);
            ((progress.message.jclient.MessageConsumer)consumer).setPrefetchThreshold(30);


            consumer.setMessageListener(new MessageListener()
            {
                private int _messageCount;

                public void onMessage(Message message)
                {
                    //_logger.info("Got message '" + message + "'");

                    TextMessage tm = (TextMessage) message;

                    try
                    {
                        Queue responseQueue = (Queue)tm.getJMSReplyTo();
                        if (_destinationQ == null ||
                            !responseQueue.getQueueName().equals(_destinationQ.getQueueName()))
                        {
                            _destinationQ = responseQueue;
                            _logger.info("Creating destination for " + responseQueue);

                            try
                            {
                                _destinationProducer = qsession2.createProducer(_destinationQ);
                                _destinationProducer.setDisableMessageTimestamp(true);
                                _destinationProducer.setDeliveryMode(progress.message.jclient.DeliveryMode.NON_PERSISTENT_ASYNC);
                            }
                            catch (JMSException e)
                            {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                            }
                        }
                        _messageCount++;
                        if (_messageCount % 1000 == 0)
                        {
                            _logger.info("Received message total: " + _messageCount);
                            _logger.info("Sending response to '" + responseQueue + "'");

                        }

                        String payload = "This is a response: sing together: 'Mahnah mahnah...'" + tm.getText();
                        TextMessage msg = qsession2.createTextMessage(payload);

                        if (tm.propertyExists("timeSent"))
                        {
                            _logger.info("timeSent property set on message");
                            final long timeSent = tm.getLongProperty("timeSent");
                            msg.setLongProperty("timeSent", timeSent);
                            _logger.info("time taken to go from service request to provider is: " + (System.currentTimeMillis() - timeSent));
                        }
                        _destinationProducer.send(msg);
                        if (_messageCount % 1000 == 0)
                        {
                            tm.acknowledge();
                        }
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Error sending message: " + e, e);
                    }
                }
            });
            qcon.start();
            qcon2.start();
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }


        System.out.println("Waiting...");
    }
}
