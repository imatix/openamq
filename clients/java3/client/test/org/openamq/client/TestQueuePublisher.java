package org.openamq.client;

import org.apache.log4j.Logger;

import javax.jms.MessageProducer;
import org.openamq.jms.Session;
import javax.jms.TextMessage;
import javax.jms.JMSException;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestQueuePublisher
{
	private static final Logger _logger = Logger.getLogger(TestQueuePublisher.class);

    public static void main(String[] args)
    {
    	_logger.info("Starting...");

        if (args.length != 6)
        {
            System.out.println("Usage: host port username password virtual-path queue");
            System.exit(1);
        }
        AMQConnection con = null;
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            con = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                    address.getHostName() + System.currentTimeMillis(), args[4]);
            Session session = (Session)con.createSession(false, Session.AUTO_ACKNOWLEDGE);
            _logger.info("Session created successfully");
            AMQQueue destination = new AMQQueue(args[5]);

            MessageProducer producer = session.createProducer(destination);
            TextMessage msg = session.createTextMessage("mahnah mahnah");
            producer.send(msg);
            _logger.info("Closing session...");
            session.close();
            _logger.info("Closing connection...");
            con.close();
            _logger.info("Test complete.");
        }
        catch (Throwable t)
        {
            _logger.fatal("Fatal error: " + t, t);
        }
        finally
        {
            if (con != null)
            {
                try
                {
                    con.close();
                }
                catch (JMSException e)
                {
                    _logger.error("Error closing connection: " + e);
                }
            }
        }
    }
}
