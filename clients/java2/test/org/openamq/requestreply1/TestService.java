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
            
            String selector = (args[3].length() > 1) ? args[3] : null;
            
            _logger.info("Message selector is <" + selector + ">...");
            
            MessageConsumer consumer = session.createQueueConsumer(destination,
                    100,false,true,true,false,selector);
            
            consumer.setMessageListener(new MessageListener()
            {
                private int _messageCount;

                public void onMessage(javax.jms.Message message)
                {
                    //_logger.info("Got message '" + message + "'");
                    
                	TextMessage tm = (TextMessage)message;
                	
                	String body = null;
                	
                	try
                	{
                		body = tm.getText();
                	}
                	catch(JMSException e)
                	{
                		_logger.error(e);
                	}
                    
                    int slash = body.indexOf("/");
                    
                    String responseQueue = (slash < 0) ? body : body.substring(0,slash);
                    String payload = (slash < 0) ? "This is a response to queue <" + responseQueue + ">, sing together: 'Mahnah mahnah...'" : body.substring(slash + 1);
                    
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
                    if (_messageCount % 1000 == 0)
                    {
                        _logger.info("Received message total: " + _messageCount);
                        _logger.info("Sending response to '" + responseQueue + "'");
                    }
                    
                    try
                    {
                        TextMessage msg = session.createTextMessage(payload);
                        _destinationProducer.send(msg);
                        if (_messageCount % 1000 == 0)
                        {
                            _logger.info("Sent response to '" + responseQueue + "'");
                        }
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Error sending message: " + e, e);
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

