package org.openamq.client;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.AMQUndeliveredException;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.protocol.FailoverSupport;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.listener.SpecificMethodFrameListener;
import org.openamq.client.transport.TransportConnection;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.BasicQosBody;
import org.openamq.framing.ChannelOpenBody;
import org.openamq.framing.ChannelOpenOkBody;
import org.openamq.framing.TxSelectBody;
import org.openamq.framing.TxSelectOkBody;
import org.openamq.jms.ChannelLimitReachedException;
import org.openamq.jms.Connection;
import org.openamq.jms.ConnectionListener;

import javax.jms.ConnectionConsumer;
import javax.jms.ConnectionMetaData;
import javax.jms.Destination;
import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueSession;
import javax.jms.ServerSessionPool;
import javax.jms.Session;
import javax.jms.Topic;
import javax.jms.TopicConnection;
import javax.jms.TopicSession;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.StringTokenizer;


public class AMQConnection extends Closeable implements Connection, QueueConnection, TopicConnection
{
    private static final Logger _logger = Logger.getLogger(AMQConnection.class);

    private final IdFactory _idFactory = new IdFactory();

    /**
     * This is the "root" mutex that must be held when doing anything that could be impacted by failover.
     * This must be held by any child objects of this connection such as the session, producers and consumers.
     */
    private final Object _failoverMutex = new Object();

    /**
     * Details of a broker that we can connect to. For failover we can know about a number of brokers each of which
     * we can attempt to use. These brokers must be a member of the cluster.
     */
    public static class BrokerDetail
    {
        public String host;
        public int port;
        public boolean useSSL;

        public BrokerDetail(String host, int port, boolean useSSL)
        {
            this.host = host;
            this.port = port;
            this.useSSL = useSSL;
        }

        public BrokerDetail(String host, int port)
        {
            this(host, port, false);
        }

        public String toString()
        {
            return (useSSL ? "SSL:" : "") + host + ":" + port;
        }

        public boolean equals(Object o)
        {
            if (!(o instanceof BrokerDetail))
            {
                return false;
            }
            BrokerDetail bd = (BrokerDetail) o;
            return host.equals(bd.host) && (port == bd.port) && useSSL == bd.useSSL;
        }
    }

    /**
     * A channel is roughly analogous to a session. The server can negotiate the maximum number of channels
     * per session and we must prevent the client from opening too many. Zero means unlimited.
     */
    private long _maximumChannelCount;

    /**
     * The maximum size of frame supported by the server
     */
    private long _maximumFrameSize;

    /**
     * The protocol handler dispatches protocol events for this connection. For example, when the connection is dropped
     * the handler deals with this. It also deals with the initial dispatch of any protocol frames to their appropriate
     * handler.
     */
    private AMQProtocolHandler _protocolHandler;

    /**
     * Maps from session id (Integer) to AMQSession instance
     */
    private final Map _sessions = new LinkedHashMap();

    private String _clientName;

    private BrokerDetail[] _brokerDetails;

    /**
     * The index into the hostDetails array of the broker to which we are connected
     */
    private int _activeBrokerIndex = -1;

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

    /**
     * Whether this connection is started, i.e. whether messages are flowing to consumers. It has no meaning for
     * message publication.
     */
    private boolean _started;

    public AMQConnection(String host, int port, String username, String password,
                         String clientName, String virtualPath) throws AMQException
    {
        this(new BrokerDetail(host, port), username, password, clientName, virtualPath);
    }

    public AMQConnection(String host, int port, boolean useSSL, String username, String password,
                         String clientName, String virtualPath) throws AMQException
    {
        this(new BrokerDetail(host, port, useSSL), username, password, clientName, virtualPath);
    }

    public AMQConnection(String brokerDetails, String username, String password,
                         String clientName, String virtualPath) throws AMQException
    {
        this(parseBrokerDetails(brokerDetails), username, password, clientName, virtualPath);
    }

    public AMQConnection(BrokerDetail brokerDetail, String username, String password,
                         String clientName, String virtualPath) throws AMQException
    {
        this(new BrokerDetail[]{brokerDetail}, username, password, clientName, virtualPath);
    }

    public AMQConnection(BrokerDetail[] brokerDetails, String username, String password,
                         String clientName, String virtualPath) throws AMQException
    {
        if (brokerDetails == null || brokerDetails.length == 0)
        {
            throw new IllegalArgumentException("Broker details must specify at least one broker");
        }
        _brokerDetails = brokerDetails;
        _clientName = clientName;
        _username = username;
        _password = password;
        _virtualPath = virtualPath;

        _protocolHandler = new AMQProtocolHandler(this);
        for (int i = 0; i < brokerDetails.length; i++)
        {
            try
            {
                makeBrokerConnection(brokerDetails[i]);
                _activeBrokerIndex = i;
                // has succeeded so break out the loop now
                break;
            }
            catch (Exception e)
            {
                _logger.info("Unable to connect to broker at " + brokerDetails[i], e);
            }
        }
        if (_activeBrokerIndex == -1)
        {
            StringBuffer buf = new StringBuffer();
            for (int i = 0; i < brokerDetails.length; i++)
            {
                buf.append(brokerDetails[i].toString()).append(' ');
            }
            throw new AMQException("Unable to connect to any specified broker in list " + buf.toString());
        }
    }

    protected AMQConnection(String username, String password, String clientName, String virtualPath)
    {
        _clientName = clientName;
        _username = username;
        _password = password;
        _virtualPath = virtualPath;
    }

    private static BrokerDetail[] parseBrokerDetails(String brokerDetails)
    {
        if (brokerDetails == null)
        {
            throw new IllegalArgumentException("Broker string cannot be null");
        }
        LinkedList ll = new LinkedList();
        StringTokenizer tokenizer = new StringTokenizer(brokerDetails, ";");
        while (tokenizer.hasMoreTokens())
        {
            String token = tokenizer.nextToken();
            int index = token.indexOf(":");
            if (index == -1)
            {
                throw new IllegalArgumentException("Invalid broker string: " + token + ". Must be in format host:port");
            }
            else
            {
                int port = Integer.parseInt(token.substring(index + 1));
                boolean useSSL = false;
                int hostStart = 0;
                if (token.charAt(0) == '$')
                {
                    hostStart = 1;
                    useSSL = true;
                }
                BrokerDetail bd = new BrokerDetail(token.substring(hostStart, index), port, useSSL);
                ll.add(bd);
            }
        }
        BrokerDetail[] bd = new BrokerDetail[ll.size()];
        return (BrokerDetail[]) ll.toArray(bd);
    }

    private void makeBrokerConnection(BrokerDetail brokerDetail) throws IOException, AMQException
    {
        TransportConnection.getInstance().connect(_protocolHandler, brokerDetail);
        // this blocks until the connection has been set up or when an error has prevented the connection being
        // set up
        _protocolHandler.attainState(AMQState.CONNECTION_OPEN);
    }

    public boolean attemptReconnection(String host, int port, boolean useSSL)
    {
        // first we find the host port combo in the broker details array
        int index = -1;
        BrokerDetail bd = new BrokerDetail(host, port, useSSL);
        for (int i = 0; i < _brokerDetails.length; i++)
        {
            if (_brokerDetails[i].equals(bd))
            {
                index = i;
                break;
            }
        }

        if (index == -1)
        {
            int len = _brokerDetails.length + 1;
            BrokerDetail[] newDetails = new BrokerDetail[len];
            System.arraycopy(_brokerDetails, 0, newDetails, 0, _brokerDetails.length);
            index = len - 1;
            newDetails[index] = bd;
        }
        try
        {
            makeBrokerConnection(bd);
            _activeBrokerIndex = index;
            return true;
        }
        catch (Exception e)
        {
            _logger.info("Unable to connect to broker at " + bd);
        }
        return false;
    }

    public boolean attemptReconnection()
    {
        if (_activeBrokerIndex < 0)
        {
            //failed to connect to first broker
            _activeBrokerIndex = 0;
        }
        else
        {
            //retry the current broker first:
            try
            {
                _logger.info("Retrying " + _brokerDetails[_activeBrokerIndex]);
                makeBrokerConnection(_brokerDetails[_activeBrokerIndex]);
                return true;
            }
            catch (Exception e)
            {
                _logger.info("Unable to reconnect to broker at " + _brokerDetails[_activeBrokerIndex]);
            }
        }

        //then try the others:
        for (int i = 0; i < _brokerDetails.length; i++)
        {
            if (i == _activeBrokerIndex)
            {
                continue;
            }
            try
            {
                makeBrokerConnection(_brokerDetails[i]);
                _activeBrokerIndex = i;
                return true;
            }
            catch (Exception e)
            {
                _logger.info("Unable to connect to broker at " + _brokerDetails[i]);
            }
        }

        //connection unsuccessful
        return false;
    }

    /**
     * Get the details of the currently active broker
     *
     * @return null if no broker is active (i.e. no successful connection has been made, or
     *         the BrokerDetail instance otherwise
     */
    public BrokerDetail getActiveBrokerDetails()
    {
        if (_activeBrokerIndex < 0)
        {
            return null;
        }
        return _brokerDetails[_activeBrokerIndex];
    }

    public Session createSession(final boolean transacted, final int acknowledgeMode) throws JMSException
    {
        return createSession(transacted, acknowledgeMode, AMQSession.DEFAULT_PREFETCH);
    }

    public org.openamq.jms.Session createSession(final boolean transacted, final int acknowledgeMode,
                                                 final int prefetch) throws JMSException
    {
        checkNotClosed();
        if (channelLimitReached())
        {
            throw new ChannelLimitReachedException(_maximumChannelCount);
        }
        else
        {
            return (org.openamq.jms.Session) new FailoverSupport()
            {
                public Object operation() throws JMSException
                {

                    int channelId = _idFactory.getChannelId();
                    AMQFrame frame = ChannelOpenBody.createAMQFrame(channelId, null);

                    if (_logger.isDebugEnabled())
                    {
                        _logger.debug("Write channel open frame for channel id " + channelId);
                    }

                    // we must create the session and register it before actually sending the frame to the server to
                    // open it, so that there is no window where we could receive data on the channel and not be set up to
                    // handle it appropriately.
                    AMQSession session = new AMQSession(AMQConnection.this, channelId, transacted, acknowledgeMode, prefetch);
                    _protocolHandler.addSessionByChannel(channelId, session);
                    registerSession(channelId, session);

                    try
                    {
                        _protocolHandler.writeCommandFrameAndWaitForReply(frame,
                                                                          new SpecificMethodFrameListener(channelId,
                                                                                                          ChannelOpenOkBody.class));
                        _protocolHandler.writeFrame(BasicQosBody.createAMQFrame(channelId, 0, prefetch, false));
                        if(transacted)
                        {
                            if (_logger.isDebugEnabled())
                            {
                                _logger.debug("Issuing TxSelect for " + channelId);
                            }
                            _protocolHandler.writeCommandFrameAndWaitForReply(
                                    TxSelectBody.createAMQFrame(channelId),
                                    new SpecificMethodFrameListener(channelId, TxSelectOkBody.class)
                            );
                        }
                    }
                    catch (AMQException e)
                    {
                        _protocolHandler.removeSessionByChannel(channelId);
                        deregisterSession(channelId);
                        throw new JMSException("Error creating session: " + e);
                    }
                    if (_started)
                    {
                        session.start();
                    }
                    return session;
                }
            }.execute(this);
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

    /**
     * Start the connection, i.e. start flowing messages. Note that this method must be called only from a single thread
     * and is not thread safe (which is legal according to the JMS specification).
     *
     * @throws JMSException
     */
    public void start() throws JMSException
    {
        checkNotClosed();
        if (!_started)
        {
            final Iterator it = _sessions.entrySet().iterator();
            while (it.hasNext())
            {
                final AMQSession s = (AMQSession) ((Map.Entry) it.next()).getValue();
                s.start();
            }
            _started = true;
        }
    }

    public void stop() throws JMSException
    {
        checkNotClosed();

        if (_started)
        {
            for (Iterator i = _sessions.values().iterator(); i.hasNext();)
            {
                ((AMQSession) i.next()).stop();
            }
            _started = false;
        }
    }

    public void close() throws JMSException
    {
        synchronized (getFailoverMutex())
        {
            if (!_closed.getAndSet(true))
            {
                try
                {
                    closeAllSessions(null);
                    _protocolHandler.closeConnection();
                }
                catch (AMQException e)
                {
                    throw new JMSException("Error closing connection: " + e);
                }
            }
        }
    }

    /**
     * Marks all sessions and their children as closed without sending any protocol messages. Useful when
     * you need to mark objects "visible" in userland as closed after failover or other significant event that
     * impacts the connection.
     * <p/>
     * The caller must hold the failover mutex before calling this method.
     */
    private void markAllSessionsClosed()
    {
        final LinkedList sessionCopy = new LinkedList(_sessions.values());
        final Iterator it = sessionCopy.iterator();
        while (it.hasNext())
        {
            final AMQSession session = (AMQSession) it.next();

            session.markClosed();
        }
        _sessions.clear();
    }

    /**
     * Close all the sessions, either due to normal connection closure or due to an error occurring.
     *
     * @param cause if not null, the error that is causing this shutdown
     *              <p/>
     *              The caller must hold the failover mutex before calling this method.
     */
    private void closeAllSessions(Throwable cause) throws JMSException
    {
        final LinkedList sessionCopy = new LinkedList(_sessions.values());
        final Iterator it = sessionCopy.iterator();
        JMSException sessionException = null;
        while (it.hasNext())
        {
            final AMQSession session = (AMQSession) it.next();
            if (cause != null)
            {
                session.closed(cause);
            }
            else
            {
                try
                {
                    session.close();
                }
                catch (JMSException e)
                {
                    _logger.error("Error closing session: " + e);
                    sessionException = e;
                }
            }
        }
        _sessions.clear();
        if (sessionException != null)
        {
            throw sessionException;
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
     * Fire the preFailover event to the registered connection listener (if any)
     *
     * @param redirect true if this is the result of a redirect request rather than a connection error
     * @return true if no listener or listener does not veto change
     */
    public boolean firePreFailover(boolean redirect)
    {
        boolean proceed = true;
        if (_connectionListener != null)
        {
            proceed = _connectionListener.preFailover(redirect);
        }
        return proceed;
    }

    /**
     * Fire the preResubscribe event to the registered connection listener (if any). If the listener
     * vetoes resubscription then all the sessions are closed.
     *
     * @return true if no listener or listener does not veto resubscription.
     * @throws JMSException
     */
    public boolean firePreResubscribe() throws JMSException
    {
        if (_connectionListener != null)
        {
            boolean resubscribe = _connectionListener.preResubscribe();
            if (!resubscribe)
            {
                markAllSessionsClosed();
            }
            return resubscribe;
        }
        else
        {
            return true;
        }
    }

    /**
     * Fires a failover complete event to the registered connection listener (if any).
     */
    public void fireFailoverComplete()
    {
        if (_connectionListener != null)
        {
            _connectionListener.failoverComplete();
        }
    }

    /**
     * In order to protect the consistency of the connection and its child sessions, consumers and producers,
     * the "failover mutex" must be held when doing any operations that could be corrupted during failover.
     *
     * @return a mutex. Guaranteed never to change for the lifetime of this connection even if failover occurs.
     */
    public final Object getFailoverMutex()
    {
        return _failoverMutex;
    }

    /**
     * If failover is taking place this will block until it has completed. If failover
     * is not taking place it will return immediately.
     *
     * @throws InterruptedException
     */
    public void blockUntilNotFailingOver() throws InterruptedException
    {
        _protocolHandler.blockUntilNotFailingOver();
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
        JMSException je = null;

        if (_exceptionListener != null)
        {
            if (cause instanceof JMSException)
            {
                je = (JMSException) cause;
            }
            else
            {
                je = new JMSException("Exception thrown against " + toString() + ": " + cause);
                if (cause instanceof Exception)
                {
                    je.setLinkedException((Exception) cause);
                }
            }
            // in the case of an IOException, MINA has closed the protocol session so we set _closed to true
            // so that any generic client code that tries to close the connection will not mess up this error
            // handling sequence
            if (cause instanceof IOException)
            {
                _closed.set(true);
            }
            _exceptionListener.onException(je);
        }
        // TODO: this is nasty. Review this (GB change)
        if (je == null || !(je.getLinkedException() instanceof AMQUndeliveredException))
        {
            try
            {
                _closed.set(true);
                closeAllSessions(cause);
            }
            catch (JMSException e)
            {
                _logger.error("Error closing all sessions: " + e, e);
            }
        }

    }

    void registerSession(int channelId, AMQSession session)
    {
        _sessions.put(new Integer(channelId), session);
    }

    void deregisterSession(int channelId)
    {
        _sessions.remove(new Integer(channelId));
    }

    /**
     * For all sessions, and for all consumers in those sessions, resubscribe. This is called during failover handling.
     * The caller must hold the failover mutex before calling this method.
     */
    public void resubscribeSessions() throws AMQException
    {
        ArrayList sessions = new ArrayList(_sessions.values());
        for (Iterator it = sessions.iterator(); it.hasNext();)
        {
            AMQSession s = (AMQSession) it.next();
            _protocolHandler.addSessionByChannel(s.getChannelId(), s);
            reopenChannel(s.getChannelId(), s.getDefaultPrefetch());
            s.resubscribe();
        }
    }

    private void reopenChannel(int channelId, int prefetch) throws AMQException
    {
        AMQFrame frame = ChannelOpenBody.createAMQFrame(channelId, null);

        try
        {
            _protocolHandler.writeCommandFrameAndWaitForReply(frame,
                                                              new SpecificMethodFrameListener(channelId,
                                                                                              ChannelOpenOkBody.class));
            _protocolHandler.writeFrame(BasicQosBody.createAMQFrame(channelId, 0, prefetch, false));

        }
        catch (AMQException e)
        {
            _protocolHandler.removeSessionByChannel(channelId);
            deregisterSession(channelId);
            throw new AMQException("Error reopening channel " + channelId + " after failover: " + e);
        }

    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer("AMQConnection:\n");
        if (_activeBrokerIndex == -1)
        {
            buf.append("No active broker connection");
        }
        else
        {
            buf.append("Host: ").append(String.valueOf(_brokerDetails[_activeBrokerIndex].host));
            buf.append("\nPort: ").append(String.valueOf(_brokerDetails[_activeBrokerIndex].port));
        }
        buf.append("\nVirtual Path: ").append(String.valueOf(_virtualPath));
        buf.append("\nClient ID: ").append(String.valueOf(_clientName));
        buf.append("\nActive session count: ").append(_sessions == null ? 0 : _sessions.size());
        return buf.toString();
    }
}
