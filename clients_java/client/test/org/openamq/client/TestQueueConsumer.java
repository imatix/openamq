package org.openamq.client;

import org.apache.log4j.Logger;
import org.openamq.jms.Session;

import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestQueueConsumer
{
	private static final Logger _logger = Logger.getLogger(TestQueueConsumer.class);

    public static void main(String[] args)
    {
    	_logger.info("Starting...");

        if (args.length != 6)
        {
            System.out.println("Usage: host port username password virtual-path queue");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            final AMQConnection con = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                                  address.getHostName(), args[4]);
            org.openamq.jms.Session session = (org.openamq.jms.Session)con.createSession(false,
                                                                                         Session.CLIENT_ACKNOWLEDGE);
            AMQQueue destination = new AMQQueue(args[5], false);

            MessageConsumer consumer = session.createConsumer(destination,
            		100, true, false, null);

            /*consumer.setMessageListener(new MessageListener()
            {
                public void onMessage(javax.jms.Message message)
                {
                    _logger.info("Got message " + message);
                    try
                    {
                        con.close();
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Error closing connection");
                    }
                }
            });*/

            con.setExceptionListener(new ExceptionListener()
            {
                public void onException(JMSException jmsException)
                {
                    _logger.error("Error occurred: " + jmsException, jmsException);
                    try
                    {
                        con.close();
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Error closing connection: " + e, e);
                    }
                }
            });

            con.start();
            Message m = consumer.receive();
            _logger.info("Message received: " + m);
            m = consumer.receive();
            m.acknowledge();
            _logger.info("Closing consumer");
            consumer.close();
            _logger.info("Closing session");
            session.close();
            _logger.info("Closing connection");
            con.close();
            //_logger.info("Waiting...");
        }
        catch (Throwable t)
        {
            _logger.error("Fatal error: " + t, t);
        }

    }
}
