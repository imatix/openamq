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
            			public void onMessage(javax.jms.Message message)
            			{
              				_logger.info("Got message '" + message + "'");
              				
            				String responseQueue = message.toString();
            				
            				AMQDestination destination = new AMQDestination(responseQueue);

            	            MessageProducer producer;
							try {
								producer = session.createProducer(destination);
							
								_logger.info("Sending response to '" + responseQueue + "'");
								
								TextMessage msg = session.createTextMessage("This is a response to queue <" + responseQueue + ">, sing together: 'Mahnah mahnah...'");
            	            
								producer.send(msg);

								_logger.info("Sent response to '" + responseQueue + "'");

							} catch (JMSException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
            	            
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
