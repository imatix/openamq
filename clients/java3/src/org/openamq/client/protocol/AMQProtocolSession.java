package org.openamq.client.protocol;

import edu.emory.mathcs.backport.java.util.concurrent.ConcurrentHashMap;
import edu.emory.mathcs.backport.java.util.concurrent.ConcurrentMap;
import org.apache.log4j.Logger;
import org.apache.mina.protocol.ProtocolSession;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.framing.AMQDataBlock;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.Channel;
import org.openamq.client.framing.ProtocolInitiation;
import org.openamq.client.message.UnprocessedMessage;

import javax.jms.JMSException;
import java.util.Map;
import java.util.Set;

/**
 * Wrapper for protocol session that provides type-safe access to session attributes.
 *
 * The underlying protocol session is still available but clients should not
 * use it to obtain session attributes.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQProtocolSession
{
    private static final Logger _logger = Logger.getLogger(AMQProtocolSession.class);

    private static final String SECURITY_MECHANISM = "SecurityMechansim";

    private static final String CONNECTION_TUNE_PARAMETERS = "ConnectionTuneParameters";

    private static final String AMQ_CONNECTION = "AMQConnection";

    private final ProtocolSession _protocolSession;

    /**
     * Maps from handleIds to the AMQSession that created the consumer. This needs to
     * be concurrent since events are received on the MINA dispatcher thread but
     * the JMS session thread could be creating and registering a new consumer at the
     * same time.
     *
     * HandleNotify handler calls back to this object which looks up the appropriate
     * session in this map, and then calls a session method which hands off the
     * unprocessed message to a JMS dispatcher thread. At that point, the MINA dispatcher
     * thread (to be clear - different thread from the JMS dispatcher thread which we
     * create) is finished dealing with the message and can resume reading from the
     * socket.
     */
    private ConcurrentMap _handleId2SessionMap = new ConcurrentHashMap();

    /**
     * Maps from the channel id to the AMQSession that it represents.
     */
    private ConcurrentMap _channelId2SessionMap = new ConcurrentHashMap();

    private ConcurrentMap _closingChannels = new ConcurrentHashMap();

    public AMQProtocolSession(ProtocolSession protocolSession, AMQConnection connection)
    {
        _protocolSession = protocolSession;
        // properties of the connection are made available to the event handlers
        _protocolSession.setAttribute(AMQ_CONNECTION, connection);

        // start the process of setting up the connection. This is the first place that
        // data is written to the server.
                
        _protocolSession.write(new ProtocolInitiation());
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

    public ProtocolSession getProtocolSession()
    {
        return _protocolSession;
    }

    public String getSecurityMechanism()
    {
        return (String) _protocolSession.getAttribute(SECURITY_MECHANISM);
    }

    public void setSecurityMechanism(String mechanism)
    {
        _protocolSession.setAttribute(SECURITY_MECHANISM, mechanism);
    }

    public ConnectionTuneParameters getConnectionTuneParameters()
    {
        return (ConnectionTuneParameters) _protocolSession.getAttribute(CONNECTION_TUNE_PARAMETERS);
    }

    public void setConnectionTuneParameters(ConnectionTuneParameters params)
    {
        _protocolSession.setAttribute(CONNECTION_TUNE_PARAMETERS, params);
        AMQConnection con = getAMQConnection();
        con.setMaximumChannelCount(params.getChannelMax());
        con.setMaximumHandleCount(params.getHandleMax());
    }

    /**
     * Callback invoked from the HandleNotifyHandler when a message has been received.
     * This is invoked on the MINA dispatcher thread.
     * @param message
     */
    public void messageReceived(UnprocessedMessage message)
    {
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Message received for handle id " + message.handleId);
        }
        AMQSession session = (AMQSession) _handleId2SessionMap.get(new Integer(message.handleId));
        if (session == null)
        {
            // TODO: presumably need to sent something back to the server but for now just log
            _logger.warn("Message received for which no session is listening");
        }
        else if (_closingChannels.containsKey(new Integer(session.getChannelId())))
        {
            // while a channel is closing, respond to all messages on that channel
            // with a channel close frame
            Channel.Close frame = new Channel.Close();
            frame.channelId = session.getChannelId();
            frame.replyCode = 200;
            writeFrame(frame);
        }
        else
        {
            session.messageReceived(message);
        }
    }

    /**
     * Convenience method that writes a frame to the protocol session. Equivalent
     * to calling getProtocolSession().write().
     *
     * @param frame the frame to write
     */
    public void writeFrame(AMQDataBlock frame)
    {
        _protocolSession.write(frame);
    }

    public void addSessionByHandle(int handleId, AMQSession session)
    {
        if (handleId <=0)
        {
            throw new IllegalArgumentException("Attempt to register a session with a handle id of zero");
        }
        if (session == null)
        {
            throw new IllegalArgumentException("Attempt to register a null session");
        }
        _logger.debug("Add session with handle id  " + handleId);
        _handleId2SessionMap.put(new Integer(handleId), session);
    }

    public void removeSessionByHandle(int handleId)
    {
        if (handleId <=0)
        {
            throw new IllegalArgumentException("Attempt to remove a handle to session mapping where handle id is <= 0");
        }
        _logger.debug("Removing session with handleId " + handleId);
        _handleId2SessionMap.remove(new Integer(handleId));
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

        Channel.Close frame = new Channel.Close();
        frame.channelId = channelId;
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
            session.closed(code, text);
            return true;
        }
        else
        {
            return false;
        }
    }

    private AMQConnection getAMQConnection()
    {
        return (AMQConnection) _protocolSession.getAttribute(AMQ_CONNECTION);
    }

    public void closeProtocolSession()
    {
        _logger.debug("Closing protocol session");
        _protocolSession.close();
    }
}
