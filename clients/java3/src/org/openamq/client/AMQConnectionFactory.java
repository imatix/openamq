package org.openamq.client;

import javax.jms.*;
import java.net.InetAddress;
import java.net.UnknownHostException;


public class AMQConnectionFactory implements ConnectionFactory, QueueConnectionFactory, TopicConnectionFactory
{
    private String _host;
    private int _port;
    private String _defaultUsername;
    private String _defaultPassword;
    private String _virtualPath;

    public AMQConnectionFactory(String host, int port, String virtualPath)
    {
        this(host, port, "guest", "guest", virtualPath);
    }

    public AMQConnectionFactory(String host, int port, String defaultUsername, String defaultPassword,
                                String virtualPath)
    {
        _host = host;
        _port = port;
        _defaultUsername = defaultUsername;
        _defaultPassword = defaultPassword;
        _virtualPath = virtualPath;
    }

    private static String getUniqueClientID()
    {
        try
        {
            InetAddress addr = InetAddress.getLocalHost();
            return addr.getHostName() + System.currentTimeMillis();
        }
        catch (UnknownHostException e)
        {
            return null;
        }
    }

    public Connection createConnection() throws JMSException
    {
        try
        {
            return new AMQConnection(_host, _port, _defaultUsername, _defaultPassword, getUniqueClientID(), _virtualPath);
        }
        catch (AMQException e)
        {
            throw new JMSException("Error creating connection: " + e);
        }
    }

    public Connection createConnection(String userName, String password) throws JMSException
    {
        try
        {
            return new AMQConnection(_host, _port, userName, password, getUniqueClientID(), _virtualPath);
        }
        catch (AMQException e)
        {
            throw new JMSException("Error creating connection: " + e);
        }
    }

    public QueueConnection createQueueConnection() throws JMSException
    {
        return (QueueConnection) createConnection();
    }

    public QueueConnection createQueueConnection(String username, String password) throws JMSException
    {
        return (QueueConnection) createConnection(username, password);
    }

    public TopicConnection createTopicConnection() throws JMSException
    {
        return (TopicConnection) createConnection();
    }

    public TopicConnection createTopicConnection(String username, String password) throws JMSException
    {
        return (TopicConnection) createConnection(username, password);
    }
}
