package org.openamq.transacted;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.openamq.jms.Session;

import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestTransactedConsumer
{
	private static final Logger _logger = Logger.getLogger(TestTransactedConsumer.class);

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
            Session session = (Session)con.createSession(true, Session.SESSION_TRANSACTED);
            AMQQueue destination = new AMQQueue(args[5]);

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
            _logger.info("Reply to is: " + m.getJMSReplyTo());
            logLatency(m.getJMSTimestamp());
            m = consumer.receive();
            _logger.info("Message received: " + m);
            _logger.info("Reply to is: " + m.getJMSReplyTo());
            logLatency(m.getJMSTimestamp());
            m = consumer.receive();
            _logger.info("Message received: " + m);
            _logger.info("Reply to is: " + m.getJMSReplyTo());
            logLatency(m.getJMSTimestamp());
            m = consumer.receive();
            _logger.info("Message received: " + m);
            _logger.info("Reply to is: " + m.getJMSReplyTo());
            logLatency(m.getJMSTimestamp());
            session.commit();
            con.close();
            //_logger.info("Waiting...");
        }
        catch (Throwable t)
        {
            _logger.error("Fatal error: " + t, t);
        }

    }

    private static void logLatency(long timestamp)
    {
        if (timestamp > 0)
        {
            _logger.info("Latency is " + (System.currentTimeMillis() - timestamp));
        }
    }
}
