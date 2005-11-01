package org.openamq.basic;

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
public class TestBasicQueueConsumer
{
	private static final Logger _logger = Logger.getLogger(TestBasicQueueConsumer.class);

    private static AMQConnection _con;

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
            _con = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                    address.getHostName(), args[4]);
            Session session = (Session)_con.createSession(false, Session.CLIENT_ACKNOWLEDGE);
            AMQQueue destination = new AMQQueue(args[5], false);

            MessageConsumer consumer = session.createBasicConsumer(destination, true, false);
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

            _con.setExceptionListener(new ExceptionListener()
            {
                public void onException(JMSException jmsException)
                {
                    _logger.error("Error occurred: " + jmsException, jmsException);
                    try
                    {
                        _con.close();
                    }
                    catch (JMSException e)
                    {
                        _logger.error("Error closing connection: " + e, e);
                    }
                }
            });

            _con.start();
            Message m = consumer.receive();
            _logger.info("Message received: " + m);
            m = consumer.receive();
            m.acknowledge();
            _logger.info("Closing consumer");
            consumer.close();
            _logger.info("Closing session");
            session.close();

            //_logger.info("Waiting...");
        }
        catch (Throwable t)
        {
            _logger.error("Fatal error: " + t, t);
        }
        finally
        {
            if (_con != null)
            {
                _logger.info("Closing connection");
                try
                {
                    _con.close();
                }
                catch (JMSException e)
                {
                    _logger.error("Error closing connection: " + e);
                }
            }
        }

    }
}
