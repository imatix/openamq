/*****************************************************************************
 * Filename    : TestParseSchema.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.schema;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.openamq.jms.Session;
import org.openamq.jms.MessageProducer;
import org.openamq.management.messaging.ManagementDestination;

import javax.jms.*;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestParseSchema
{
    private static final Logger _logger = Logger.getLogger(TestParseSchema.class);

    private static AMQConnection _con;

    public static void main(String[] args)
    {
        _logger.info("Starting...");

        if (args.length != 5)
        {
            System.out.println("Usage: host port username password vhost");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            _con = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                     address.getHostName(), args[4]);
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
            Session session = (Session)_con.createSession(false, Session.CLIENT_ACKNOWLEDGE);
            AMQQueue replyQueue = new AMQQueue("response", true)
            {
                public String getEncodedName()
                {
                    return getQueueName();
                }
            };

            MessageConsumer consumer = session.createConsumer(replyQueue, 100, true, true, null);

            MessageProducer producer = (MessageProducer) session.createProducer(new ManagementDestination());
            TextMessage schemaRequestMsg = session.createTextMessage("<?xml?><cml version = \"1.0\"><schema /></cml>");
            _con.start();
            // the server-assigned queue name is populated after creation of the consumer
            schemaRequestMsg.setJMSReplyTo(replyQueue);
            producer.send(schemaRequestMsg);
            Message m = consumer.receive();
            _logger.info("Message received: " + m);

            _logger.info("Reeived: " + m);
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
