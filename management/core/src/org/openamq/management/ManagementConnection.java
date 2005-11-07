/*****************************************************************************
 * Filename    : ManagementConnection.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management;

import org.openamq.AMQException;
import org.openamq.management.messaging.ManagementDestination;
import org.openamq.jms.Session;
import org.openamq.jms.MessageProducer;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.apache.log4j.Logger;

import javax.jms.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ManagementConnection
{
    private static final Logger _log = Logger.getLogger(ManagementConnection.class);

    private String _brokerHost;

    private int _brokerPort;

    private String _username;

    private String _password;

    private String _virtualPath;

    private AMQConnection _connection;

    private Session _session;

    private MessageConsumer _consumer;

    private MessageProducer _producer;

    private AMQQueue _replyQueue;

    public ManagementConnection(String brokerHost, int brokerPort, String username,
                                String password, String virtualPath)
    {
        _brokerHost = brokerHost;
        _brokerPort = brokerPort;
        _username = username;
        _password = password;
        _virtualPath = virtualPath;
    }

    public void connect() throws AMQException, JMSException
    {
        _connection = new AMQConnection(_brokerHost, _brokerPort, _username, _password,
                                        "clientName" + System.currentTimeMillis(), _virtualPath);
        _connection.setExceptionListener(new ExceptionListener()
        {
            public void onException(JMSException jmsException)
            {
                _log.error("Error occurred: " + jmsException, jmsException);
                try
                {
                    _connection.close();
                }
                catch (JMSException e)
                {
                    _log.error("Error closing connection: " + e, e);
                }
            }
        });
        _session = (Session)_connection.createSession(false, Session.CLIENT_ACKNOWLEDGE);
        _replyQueue = new AMQQueue("response", true)
        {
            public String getEncodedName()
            {
                return getQueueName();
            }
        };
        _consumer = _session.createConsumer(_replyQueue, 100, true, true, null);

        _producer = (MessageProducer) _session.createProducer(new ManagementDestination());
        _connection.start();
    }

    /**
     * Send a request and wait for a response.
     * @param xmlRequest the request to send
     * @return the response received from the broker
     * @throws AMQException when an AMQ error occurs
     * @throws JMSException when a JMS error occurs
     */
    public TextMessage sendRequest(String xmlRequest) throws AMQException, JMSException
    {
        TextMessage requestMsg = _session.createTextMessage(xmlRequest);
        requestMsg.setJMSReplyTo(_replyQueue);
        _producer.send(requestMsg);
        return (TextMessage) _consumer.receive();
    }

    public void close() throws AMQException, JMSException
    {
        if (_connection != null)
        {
            _connection.close();
        }
    }
}
