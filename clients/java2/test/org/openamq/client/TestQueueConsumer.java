package org.openamq.client;

import org.openamq.jms.Session;
import javax.jms.*;

import java.net.InetAddress;
import org.apache.log4j.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestQueueConsumer
{
	private static final Logger _logger = Logger.getLogger(TestQueueConsumer.class);

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
            org.openamq.jms.Session session = (org.openamq.jms.Session)con.createSession(false, Session.AUTO_ACKNOWLEDGE);
            AMQDestination destination = new AMQDestination("test");

            MessageConsumer consumer = session.createQueueConsumer(destination,
            		100,false,true,true,false,null);

            consumer.setMessageListener(new MessageListener()
            		{
            			public void onMessage(javax.jms.Message message)
            			{
            				_logger.info("Got message " + message); 
            			}
            		});
            
            
            System.out.println("Waiting...");
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }

    }
}
