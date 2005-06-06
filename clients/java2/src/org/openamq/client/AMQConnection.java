package org.openamq.client;

import org.openamq.client.transport.TransportConnection;
import org.openamq.jms.ChannelLimitReachedException;
import org.openamq.jms.Connection;

import javax.jms.*;
import java.io.IOException;
import java.util.LinkedHashMap;


public class AMQConnection implements Connection
{
    private IdFactory _idFactory = new IdFactory();

    private TransportConnection _transportConnection;

    /**
     * A channel is roughly analogous to a session. The server can negotiate the maximum number of channels
     * per session and we must prevent the client from opening too many. Zero means unlimited.
     */
    private short _maximumChannelCount;

    /**
     * A handle is roughly analogous to a message producer or consumer. The server can negotiate the maximum
     * number of handles per session and we must prevent the client from opening too many. Zero means unlimited.
     */
    private int _maximumHandleCount;

    /**
     * Maps from session id (Integer) to AMQSession instance
     */
    private LinkedHashMap _sessions;

    public AMQConnection(String host, int port, String username, String password,
                         String clientName, String virtualPath) throws AMQException
    {
        try
        {
            _transportConnection = new TransportConnection(host, port, username, password,
                                                           clientName, virtualPath);
        }
        catch (IOException e)
        {
            throw new AMQException("Error creating transport connection: " + e, e);
        }
    }

    public Session createSession(boolean transacted, int acknowledgeMode) throws JMSException
    {
        if (channelLimitReached())
        {
            throw new ChannelLimitReachedException(_maximumChannelCount);
        }
        else
        {
            // TODO: thread safety
            int channelId = _idFactory.getChannelId();
            AMQSession session = new AMQSession(this, channelId, transacted, acknowledgeMode);
            _sessions.put(new Integer(channelId), session);
            return session;
        }
    }

    private boolean channelLimitReached()
    {
        return _sessions.size() == _maximumChannelCount;
    }

    public String getClientID() throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public void setClientID(String clientID) throws JMSException
    {
        // TODO Auto-generated method stub

    }

    public ConnectionMetaData getMetaData() throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public ExceptionListener getExceptionListener() throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public void setExceptionListener(ExceptionListener listener) throws JMSException
    {
        // TODO Auto-generated method stub

    }

    public void start() throws JMSException
    {
        // TODO Auto-generated method stub

    }

    public void stop() throws JMSException
    {
        // TODO Auto-generated method stub

    }

    public void close() throws JMSException
    {
        // TODO Auto-generated method stub

    }

    public ConnectionConsumer createConnectionConsumer(Destination destination, String messageSelector,
                                                       ServerSessionPool sessionPool, int maxMessages) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public ConnectionConsumer createDurableConnectionConsumer(Topic topic, String subscriptionName,
                                                              String messageSelector, ServerSessionPool sessionPool, int maxMessages)
                                    throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    IdFactory getIdFactory()
    {
        return _idFactory;
    }

    public short getMaximumChannelCount()
    {
        return _maximumChannelCount;
    }
}
