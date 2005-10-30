package org.openamq.client;

import org.apache.log4j.Logger;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.listener.SpecificMethodFrameListener;
import org.openamq.client.transport.TransportConnection;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.ChannelOpenBody;
import org.openamq.framing.ChannelOpenOkBody;
import org.openamq.jms.ChannelLimitReachedException;
import org.openamq.jms.Connection;
import org.openamq.jms.ConnectionListener;
import org.openamq.AMQException;

import javax.jms.*;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;


public class AMQConnection extends Closeable implements Connection, QueueConnection, TopicConnection
{
    private static final Logger _logger = Logger.getLogger(AMQConnection.class);

    private final IdFactory _idFactory = new IdFactory();

    private TransportConnection _transportConnection;

    /**
     * A channel is roughly analogous to a session. The server can negotiate the maximum number of channels
     * per session and we must prevent the client from opening too many. Zero means unlimited.
     */
    private long _maximumChannelCount;

    /**
     * The maximum size of frame supported by the server
     */
    private long _maximumFrameSize;

    private AMQProtocolHandler _protocolHandler;

    /**
     * Maps from session id (Integer) to AMQSession instance
     */
    private final HashMap _sessions = new LinkedHashMap();

    private String _clientName;

    /**
     * The host to which the connection is connected
     */
    private String _host;

    /**
     * The port on which the connection is made
     */
    private int _port;

    /**
     * The user name to use for authentication
     */
    private String _username;

    /**
     * The password to use for authentication
     */
    private String _password;

    /**
     * The virtual path to connect to on the AMQ server
      */
    private String _virtualPath;

    private ExceptionListener _exceptionListener;

    private ConnectionListener _connectionListener;

    public AMQConnection(String host, int port, String username, String password,
                         String clientName, String virtualPath) throws AMQException
    {
        _clientName = clientName;
        _host = host;
        _port = port;
        _username = username;
        _password = password;
        _virtualPath = virtualPath;

        try
        {
            _transportConnection = new TransportConnection(this);
            _protocolHandler = _transportConnection.connect();
            // this blocks until the connection has been set up
            _protocolHandler.attainState(AMQState.CONNECTION_OPEN);
        }
        catch (IOException e)
        {
            throw new AMQException("Error creating transport connection: " + e, e);
        }
    }

    public Session createSession(boolean transacted, int acknowledgeMode) throws JMSException
    {
        checkNotClosed();
        if (channelLimitReached())
        {
            throw new ChannelLimitReachedException(_maximumChannelCount);
        }
        else
        {
            // TODO: check thread safety
            short channelId = _idFactory.getChannelId();
            AMQFrame frame = ChannelOpenBody.createAMQFrame(channelId, 100,
                                                            null);

            if (_logger.isDebugEnabled())
            {
                _logger.debug("Write channel open frame for channel id " + channelId);
            }

            // we must create the session and register it before actually sending the frame to the server to
            // open it, so that there is no window where we could receive data on the channel and not be set up to
            // handle it appropriately.
            AMQSession session = new AMQSession(this, channelId, transacted, acknowledgeMode);
            _protocolHandler.addSessionByChannel(channelId, session);
            _sessions.put(new Integer(channelId), session);
            try
            {
                _protocolHandler.writeCommandFrameAndWaitForReply(frame,
                                                                  new SpecificMethodFrameListener(channelId,
                                                                                                  ChannelOpenOkBody.class));
            }
            catch (AMQException e)
            {
                _protocolHandler.removeSessionByChannel(channelId);
                _sessions.remove(new Integer(channelId));
                throw new JMSException("Error creating session: " + e);
            }
            return session;
        }
    }

    public QueueSession createQueueSession(boolean transacted, int acknowledgeMode) throws JMSException
    {
        return (QueueSession) createSession(transacted, acknowledgeMode);
    }

    public TopicSession createTopicSession(boolean transacted, int acknowledgeMode) throws JMSException
    {
        return (TopicSession) createSession(transacted, acknowledgeMode);
    }

    private boolean channelLimitReached()
    {
        return _maximumChannelCount != 0 && _sessions.size() == _maximumChannelCount;
    }

    public String getClientID() throws JMSException
    {
        checkNotClosed();
        return _clientName;
    }

    public void setClientID(String clientID) throws JMSException
    {
        checkNotClosed();
        _clientName = clientID;
    }

    public ConnectionMetaData getMetaData() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public ExceptionListener getExceptionListener() throws JMSException
    {
        checkNotClosed();
        return _exceptionListener;
    }

    public void setExceptionListener(ExceptionListener listener) throws JMSException
    {
        checkNotClosed();
        _exceptionListener = listener;
    }

    public void start() throws JMSException
    {
        checkNotClosed();
        final Iterator it = _sessions.entrySet().iterator();
        while (it.hasNext())
        {
            final AMQSession s = (AMQSession)((Map.Entry) it.next()).getValue();
            s.start();
        }

    }

    public void stop() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
    }

    public void close() throws JMSException
    {
        if (!_closed.getAndSet(true))
        {
            try
            {
                // TODO: close the sessions in an order fashion
                _protocolHandler.closeConnection();
            }
            catch (AMQException e)
            {
                throw new JMSException("Error closing connection: " + e);
            }
        }
    }

    public ConnectionConsumer createConnectionConsumer(Destination destination, String messageSelector,
                                                       ServerSessionPool sessionPool, int maxMessages) throws JMSException
    {
        checkNotClosed();
        return null;
    }

    public ConnectionConsumer createConnectionConsumer(Queue queue, String messageSelector,
                                                       ServerSessionPool sessionPool,
                                                       int maxMessages) throws JMSException
    {
        checkNotClosed();
        return null;
    }

    public ConnectionConsumer createConnectionConsumer(Topic topic, String messageSelector,
                                                       ServerSessionPool sessionPool,
                                                       int maxMessages) throws JMSException
    {
        checkNotClosed();
        return null;
    }

    public ConnectionConsumer createDurableConnectionConsumer(Topic topic, String subscriptionName,
                                                              String messageSelector, ServerSessionPool sessionPool, int maxMessages)
                                    throws JMSException
    {
        // TODO Auto-generated method stub
        checkNotClosed();
        return null;
    }

    IdFactory getIdFactory()
    {
        return _idFactory;
    }

    public long getMaximumChannelCount()
    {
        checkNotClosed();
        return _maximumChannelCount;
    }

    public void setConnectionListener(ConnectionListener listener)
    {
        _connectionListener = listener;
    }

    public ConnectionListener getConnectionListener()
    {
        return _connectionListener;
    }

    public void setMaximumChannelCount(long maximumChannelCount)
    {
        checkNotClosed();
        _maximumChannelCount = maximumChannelCount;
    }

    public void setMaximumFrameSize(long frameMax)
    {
        _maximumFrameSize = frameMax;
    }

    public long getMaximumFrameSize()
    {
        return _maximumFrameSize;
    }

    public Map getSessions()
    {
        return _sessions;
    }

    public String getClientName()
    {
        return _clientName;
    }

    public String getHost()
    {
        return _host;
    }

    public int getPort()
    {
        return _port;
    }

    public String getUsername()
    {
        return _username;
    }

    public String getPassword()
    {
        return _password;
    }

    public String getVirtualPath()
    {
        return _virtualPath;
    }

    public AMQProtocolHandler getProtocolHandler()
    {
        return _protocolHandler;
    }

    public void bytesSent(long writtenBytes)
    {
        if (_connectionListener != null)
        {
            _connectionListener.bytesSent(writtenBytes);
        }
    }

    public void bytesReceived(long receivedBytes)
    {
        if (_connectionListener != null)
        {
            _connectionListener.bytesReceived(receivedBytes);
        }
    }

    /**
     * Invoked by the AMQProtocolSession when a protocol session exception has occurred.
     * This method sends the exception to a JMS exception listener, if configured, and
     * propagates the exception to sessions, which in turn will propagate to consumers.
     * This allows synchronous consumers to have exceptions thrown to them.
     *
     * @param cause the exception
     */
    public void exceptionReceived(Throwable cause)
    {
        if (_exceptionListener != null)
        {
            JMSException je;
            if (cause instanceof JMSException)
            {
                je = (JMSException) cause;
            }
            else
            {
                je = new JMSException("Exception thrown against " + toString() + ": " + cause);
                if (cause instanceof Exception)
                {
                    je.setLinkedException((Exception)cause);
                }
            }
            _exceptionListener.onException(je);
        }
        // we also must propagate exceptions to any consumers
        final Iterator it = _sessions.values().iterator();
        while (it.hasNext())
        {
            final AMQSession session = (AMQSession) it.next();
            session.connectionExceptionReceived(Throwable cause);
        }
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer("AMQConnection:\n");
        buf.append("Host: ").append(String.valueOf(_host));
        buf.append("\nPort: ").append(_port);
        buf.append("\nVirtual Path: ").append(String.valueOf(_virtualPath));
        buf.append("\nClient ID: ").append(String.valueOf(_clientName));
        buf.append("\nActive session count: ").append(_sessions == null ? 0 : _sessions.size());
        return buf.toString();
    }
}
