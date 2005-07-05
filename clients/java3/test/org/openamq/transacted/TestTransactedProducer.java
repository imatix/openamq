package org.openamq.transacted;

import org.apache.log4j.Logger;

import javax.jms.MessageProducer;
import org.openamq.jms.Session;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;

import javax.jms.TextMessage;
import javax.jms.JMSException;
import javax.jms.DeliveryMode;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestTransactedProducer
{
	private static final Logger _logger = Logger.getLogger(TestTransactedProducer.class);

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
            Session session = (Session)con.createSession(true, Session.SESSION_TRANSACTED);
            AMQQueue destination = new AMQQueue(args[5]);
            AMQQueue replyTo = new AMQQueue("kermit");
            MessageProducer producer = session.createProducer(destination);
            producer.setDisableMessageTimestamp(false);
            TextMessage msg = session.createTextMessage("mahnah mahnah");
            msg.setJMSReplyTo(replyTo);
            producer.send(msg, DeliveryMode.NON_PERSISTENT, 4, 5000);
            producer.send(msg, DeliveryMode.NON_PERSISTENT, 5, 3000);
            producer.send(msg, DeliveryMode.NON_PERSISTENT, 1, 0);
            producer.send(msg, DeliveryMode.NON_PERSISTENT, 9, 1);
            _logger.info("Committing session...");
            session.commit();
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
