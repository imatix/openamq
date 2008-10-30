package org.openamq.client.protocol;

import edu.emory.mathcs.backport.java.util.concurrent.ConcurrentHashMap;
import edu.emory.mathcs.backport.java.util.concurrent.ConcurrentMap;
import org.apache.log4j.Logger;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.WriteFuture;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.CloseFuture;
import org.openamq.AMQException;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.client.message.UnexpectedBodyReceivedException;
import org.openamq.framing.*;

import javax.jms.JMSException;
import javax.security.sasl.SaslClient;

/**
 * Wrapper for protocol session that provides type-safe access to session attributes.
 *
 * The underlying protocol session is still available but clients should not
 * use it to obtain session attributes.
 *
 */
public class AMQProtocolSession
{
    private static final Logger _logger = Logger.getLogger(AMQProtocolSession.class);

    public static final String PROTOCOL_INITIATION_RECEIVED = "ProtocolInitiatiionReceived";

    protected static final String CONNECTION_TUNE_PARAMETERS = "ConnectionTuneParameters";

    private static final String AMQ_CONNECTION = "AMQConnection";

    private static final String SASL_CLIENT = "SASLClient";

    private final IoSession _minaProtocolSession;

    /**
     * The handler from which this session was created and which is used to handle protocol events.
     * We send failover events to the handler.
     */
    private final AMQProtocolHandler _protocolHandler;

    /**
     * Maps from the channel id to the AMQSession that it represents.
     */
    private ConcurrentMap _channelId2SessionMap = new ConcurrentHashMap();

    private ConcurrentMap _closingChannels = new ConcurrentHashMap();

    /**
     * Maps from a channel id to an unprocessed message. This is used to tie together the
     * JmsDeliverBody (which arrives first) with the subsequent content header and content bodies.
     */
    private ConcurrentMap _channelId2UnprocessedMsgMap = new ConcurrentHashMap();

    /**
     * Counter to ensure unique queue names
     */
    private int _queueId = 1;
    private final Object _queueIdLock = new Object();

    public AMQProtocolSession(AMQProtocolHandler protocolHandler, IoSession protocolSession, AMQConnection connection)
    {
        _protocolHandler = protocolHandler;
        _minaProtocolSession = protocolSession;
        // properties of the connection are made available to the event handlers
        _minaProtocolSession.setAttribute(AMQ_CONNECTION, connection);
    }

    public void init()
    {
        // start the process of setting up the connection. This is the first place that
        // data is written to the server.
        _minaProtocolSession.write(new ProtocolInitiation());
    }

    public String getClientID()
    {
        try
        {
            return getAMQConnection().getClientID();
        }
        catch (JMSException e)
        {
            // we never throw a JMSException here
            return null;
        }
    }

    public void setClientID(String clientID) throws JMSException
    {
        getAMQConnection().setClientID(clientID);
    }

    public String getVirtualPath()
    {
        return getAMQConnection().getVirtualPath();
    }

    public String getUsername()
    {
        return getAMQConnection().getUsername();
    }

    public String getPassword()
    {
        return getAMQConnection().getPassword();
    }

    public IoSession getIoSession()
    {
        return _minaProtocolSession;
    }

    public SaslClient getSaslClient()
    {
        return (SaslClient) _minaProtocolSession.getAttribute(SASL_CLIENT);
    }

    /**
     * Store the SASL client currently being used for the authentication handshake
     * @param client if non-null, stores this in the session. if null clears any existing client
     * being stored
     */
    public void setSaslClient(SaslClient client)
    {
        if (client == null)
        {
            _minaProtocolSession.removeAttribute(SASL_CLIENT);
        }
        else
        {
            _minaProtocolSession.setAttribute(SASL_CLIENT, client);
        }
    }

    public ConnectionTuneParameters getConnectionTuneParameters()
    {
        return (ConnectionTuneParameters) _minaProtocolSession.getAttribute(CONNECTION_TUNE_PARAMETERS);
    }

    public void setConnectionTuneParameters(ConnectionTuneParameters params)
    {
        _minaProtocolSession.setAttribute(CONNECTION_TUNE_PARAMETERS, params);
        AMQConnection con = getAMQConnection();
        con.setMaximumChannelCount(params.getChannelMax());
        con.setMaximumFrameSize(params.getFrameMax());
        initHeartbeats((int) params.getHeartbeat());
    }

    /**
     * Callback invoked from the BasicDeliverMethodHandler when a message has been received.
     * This is invoked on the MINA dispatcher thread.
     * @param message
     * @throws AMQException if this was not expected
     */
    public void unprocessedMessageReceived(UnprocessedMessage message) throws AMQException
    {
        _channelId2UnprocessedMsgMap.put(new Integer(message.channelId), message);
    }

    public void messageContentHeaderReceived(int channelId, ContentHeaderBody contentHeader)
            throws AMQException
    {
        UnprocessedMessage msg = (UnprocessedMessage) _channelId2UnprocessedMsgMap.get(new Integer(channelId));
        if (msg == null)
        {
            throw new AMQException("Error: received content header without having received a JMSDeliver frame first");
        }
        if (msg.contentHeader != null)
        {
            throw new AMQException("Error: received duplicate content header or did not receive correct number of content body frames");
        }
        msg.contentHeader = contentHeader;
        if (contentHeader.bodySize == 0)
        {
            deliverMessageToAMQSession(channelId, msg);
        }
    }

    public void messageContentBodyReceived(int channelId, ContentBody contentBody) throws AMQException
    {
        UnprocessedMessage msg = (UnprocessedMessage) _channelId2UnprocessedMsgMap.get(new Integer(channelId));
        if (msg == null)
        {
            throw new AMQException("Error: received content body without having received a JMSDeliver frame first");
        }
        if (msg.contentHeader == null)
        {
            _channelId2UnprocessedMsgMap.remove(new Integer(channelId));
            throw new AMQException("Error: received content body without having received a ContentHeader frame first");
        }
        try
        {
            msg.receiveBody(contentBody);
        }
        catch (UnexpectedBodyReceivedException e)
        {
            _channelId2UnprocessedMsgMap.remove(new Integer(channelId));
            throw e;
        }
        if (msg.isAllBodyDataReceived())
        {
            deliverMessageToAMQSession(channelId,  msg);
        }
    }

    /**
     * Deliver a message to the appropriate session, removing the unprocessed message
     * from our map
     * @param channelId the channel id the message should be delivered to
     * @param msg the message
     */
    private void deliverMessageToAMQSession(int channelId, UnprocessedMessage msg)
    {
        AMQSession session = (AMQSession) _channelId2SessionMap.get(new Integer(channelId));
        session.messageReceived(msg);
        _channelId2UnprocessedMsgMap.remove(new Integer(channelId));
    }

    /**
     * Convenience method that writes a frame to the protocol session. Equivalent
     * to calling getProtocolSession().write().
     *
     * @param frame the frame to write
     */
    public void writeFrame(AMQDataBlock frame)
    {
        _minaProtocolSession.write(frame);
    }

    public void writeFrame(AMQDataBlock frame, boolean wait)
    {
        WriteFuture f =_minaProtocolSession.write(frame);
        if(wait)
        {
            f.join();
        }
    }

    public void addSessionByChannel(int channelId, AMQSession session)
    {
        if (channelId <=0)
        {
            throw new IllegalArgumentException("Attempt to register a session with a channel id <= zero");
        }
        if (session == null)
        {
            throw new IllegalArgumentException("Attempt to register a null session");
        }
        _logger.debug("Add session with channel id  " + channelId);
        _channelId2SessionMap.put(new Integer(channelId), session);
    }

    public void removeSessionByChannel(int channelId)
    {
        if (channelId <=0)
        {
            throw new IllegalArgumentException("Attempt to deregister a session with a channel id <= zero");
        }
        _logger.debug("Removing session with channelId " + channelId);
        _channelId2SessionMap.remove(new Integer(channelId));
    }

    /**
     * Starts the process of closing a session
     * @param session the AMQSession being closed
     */
    public void closeSession(AMQSession session)
    {
        _logger.debug("closeSession called on protocol session for session " + session.getChannelId());
        final int channelId = session.getChannelId();
        if (channelId <=0)
        {
            throw new IllegalArgumentException("Attempt to close a channel with id < 0");
        }
        // we need to know when a channel is closing so that we can respond
        // with a channel.close frame when we receive any other type of frame
        // on that channel
        _closingChannels.putIfAbsent(new Integer(channelId), session);

        final AMQFrame frame = ChannelCloseBody.createAMQFrame(channelId, AMQConstant.REPLY_SUCCESS.getCode(),
                                                               "JMS client closing channel", 0, 0);
        writeFrame(frame);
    }

    /**
     * Called from the ChannelClose handler when a channel close frame is received.
     * This method decides whether this is a response or an initiation. The latter
     * case causes the AMQSession to be closed and an exception to be thrown if
     * appropriate.
     * @param channelId the id of the channel (session)
     * @return true if the client must respond to the server, i.e. if the server
     * initiated the channel close, false if the channel close is just the server
     * responding to the client's earlier request to close the channel.
     */
    public boolean channelClosed(int channelId, int code, String text)
    {
        final Integer chId = new Integer(channelId);
        // if this is not a response to an earlier request to close the channel
        if (_closingChannels.remove(chId) == null)
        {
            final AMQSession session = (AMQSession) _channelId2SessionMap.get(chId);
            session.closed(new AMQException(_logger, code, text));
            return true;
        }
        else
        {
            return false;
        }
    }

    public AMQConnection getAMQConnection()
    {
        return (AMQConnection) _minaProtocolSession.getAttribute(AMQ_CONNECTION);
    }

    public void closeProtocolSession()
    {
        _logger.debug("Closing protocol session");
        final CloseFuture future = _minaProtocolSession.close();
        future.join();
    }

    public void failover(String host, int port)
    {
        _protocolHandler.failover(host, port);
    }

    String generateQueueName()
    {
        int id;
        synchronized(_queueIdLock)
        {
            id = _queueId++;
        }
        return "tmp_" + _minaProtocolSession.getLocalAddress() + "_" + id;
    }

    /**
     *
     * @param delay delay in seconds (not ms)
     */
    void initHeartbeats(int delay)
    {
        if(delay > 0)
        {
            _minaProtocolSession.setIdleTime(IdleStatus.WRITER_IDLE, delay);
            _minaProtocolSession.setIdleTime(IdleStatus.READER_IDLE, HeartbeatConfig.CONFIG.getTimeout(delay));
            HeartbeatDiagnostics.init(delay, HeartbeatConfig.CONFIG.getTimeout(delay));
        }
    }

}
