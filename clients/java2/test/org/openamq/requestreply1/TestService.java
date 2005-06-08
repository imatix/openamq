package org.openamq.requestreply1;

import org.openamq.jms.Session;
import javax.jms.*;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import java.net.InetAddress;
import org.apache.log4j.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestService
{
    private static final Logger _logger = Logger.getLogger(TestService.class);
    
    private static MessageProducer _destinationProducer;

    private static String _destinationName;
    
    public static void main(String[] args)
    {
        _logger.info("Starting...");
        
        if (args.length != 5)
        {
            System.out.println("Usage: host port username password virtual-path");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            AMQConnection con = new AMQConnection(args[0], Integer.parseInt(args[1]), "guest", "guest",
                    address.getHostName(), "/test");
            Thread.sleep(5000);
            final org.openamq.jms.Session session = (org.openamq.jms.Session)con.createSession(false, Session.AUTO_ACKNOWLEDGE);
            
            String serviceName = args[2];
            
            _logger.info("Service (queue) name is '" + serviceName + "'...");
            
            AMQDestination destination = new AMQDestination(serviceName);
            
            MessageConsumer consumer = session.createQueueConsumer(destination,
                    100,false,true,true,false,null);
            
            consumer.setMessageListener(new MessageListener()
            {
                private int _messageCount;

                public void onMessage(javax.jms.Message message)
                {
                    _logger.info("Got message '" + message + "'");
                    
                    String responseQueue = message.toString();
                    
                    if (!responseQueue.equals(_destinationName))                    
                    {
                        _destinationName = responseQueue;
                        _logger.info("Creating destination for " + responseQueue);
                        AMQDestination destination = new AMQDestination(responseQueue);
                                              
                        try
                        {
                            _destinationProducer = session.createProducer(destination);
                        }
                        catch (JMSException e)
                        {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }
                    }
                    _messageCount++;
                    if (_messageCount%100 == 0)
                    {
                        _logger.info("Received message total: " + _messageCount);
                        _logger.info("Sending response to '" + responseQueue + "'");
                    }
                    
                    try
                    {
                        TextMessage msg = session.createTextMessage("This is a response to queue <" + responseQueue + ">, sing together: 'Mahnah mahnah...'");
                        _destinationProducer.send(msg);
                        if (_messageCount%100 == 0)
                        {
                            _logger.info("Sent response to '" + responseQueue + "'");
                        }
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Erorr sending message: " + e, e);
                    }
                }
            });                        
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }
        
        
        
        System.out.println("Waiting...");
    }
    
}

