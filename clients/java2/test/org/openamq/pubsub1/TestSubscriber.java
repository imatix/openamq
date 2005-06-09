package org.openamq.pubsub1;

import org.openamq.jms.Session;
import javax.jms.*;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import java.net.InetAddress;
import org.apache.log4j.*;

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
            
            String selector = args[2];
            
            _logger.info("Message selector is <" + selector + ">...");
            
            MessageConsumer consumer = session.createTopicConsumer(null,
                    100,false,true,true,false,selector);
            
            consumer.setMessageListener(new MessageListener()
            {
                private int _messageCount;

                public void onMessage(javax.jms.Message message)
                {
                    _logger.info("Got message '" + message + "'");
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

