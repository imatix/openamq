package org.openamq.client;

import org.openamq.AMQException;

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

    public AMQConnectionFactory()
    {        
    }

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


    /**
     * @param password The _defaultPassword to set.
     */
    public final void setDefaultPassword(String password) {
        _defaultPassword = password;
    }

    /**
     * @param username The _defaultUsername to set.
     */
    public final void setDefaultUsername(String username) {
        _defaultUsername = username;
    }

     /**
     * @param _host The _host to set.
     */
    public final void setHost(String _host) {
        this._host = _host;
    }

    /**
     * @param _port The _port to set.
     */
    public final void setPort(int _port) {
        this._port = _port;
    }

    /**
     * @param path The _virtualPath to set.
     */
    public final void setVirtualPath(String path) {
        _virtualPath = path;
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
