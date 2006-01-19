package org.openamq.client.protocol;

import edu.emory.mathcs.backport.java.util.concurrent.CopyOnWriteArraySet;
import org.apache.log4j.Logger;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.transport.socket.nio.SocketSession;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.openamq.client.AMQConnection;
import org.openamq.AMQException;
import org.openamq.AMQDisconnectedException;
import org.openamq.client.AMQSession;
import org.openamq.client.transport.AMQProtocolProvider;
import org.openamq.framing.*;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.listener.ConnectionCloseOkListener;
import org.openamq.client.state.listener.SpecificMethodFrameListener;

import java.util.Iterator;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQProtocolHandler extends IoHandlerAdapter
{
    private static final Logger _logger = Logger.getLogger(AMQProtocolHandler.class);

    /**
     * The connection that this protocol handler is associated with. There is a 1-1
     * mapping between connection instances and protocol handler instances.
     */
    private AMQConnection _connection;

    /**
     * Our wrapper for a protocol session that provides access to session values
     * in a typesafe manner.
     */
    private AMQProtocolSession _protocolSession;

    private final AMQStateManager _stateManager = new AMQStateManager();

    private final CopyOnWriteArraySet _frameListeners = new CopyOnWriteArraySet();

    public AMQProtocolHandler(AMQConnection con)
    {
        _connection = con;
        _frameListeners.add(_stateManager);
    }

    public void sessionCreated(IoSession session) throws Exception
    {
        if (session instanceof SocketSession)
        {
            SocketSession socketSession = (SocketSession) session;
            _logger.info("Setting socket receive buffer size to 8192 bytes");
            socketSession.setReceiveBufferSize(8192);
        }

        AMQProtocolProvider provider = new AMQProtocolProvider();
        session.getFilterChain().addLast("protocolFilter",
                                         new ProtocolCodecFilter(provider.getCodecFactory()));
    }

    public void sessionOpened(IoSession session) throws Exception
    {
        _protocolSession = new AMQProtocolSession(session, _connection);
    }

    public void sessionClosed(IoSession session) throws Exception
    {
        // we only raise an exception if the close was not initiated by the client
        if (!_connection.isClosed())
        {
            _connection.exceptionReceived(new AMQDisconnectedException("Server closed connection"));
        }

        _logger.info("Protocol Session closed");
    }

    public void sessionIdle(IoSession session, IdleStatus status) throws Exception
    {
        _logger.info("Protocol Session idle");
    }

    public void exceptionCaught(IoSession session, Throwable cause) throws Exception
    {
        _logger.error("Exception caught by protocol handler: " + cause, cause);
        _connection.exceptionReceived(cause);
        _stateManager.error(new AMQException("Protocol handler error: " + cause, cause));
    }

    private static int _messageReceivedCount;

    public void messageReceived(IoSession session, Object message) throws Exception
    {
        if (_messageReceivedCount++ % 1000 == 0)
        {
            _logger.warn("Received " + _messageReceivedCount + " protocol messages");
        }
        Iterator it = _frameListeners.iterator();
        AMQFrame frame = (AMQFrame) message;

        if (frame.bodyFrame instanceof AMQMethodBody)
        {
            if (_logger.isDebugEnabled())
            {
                _logger.debug("Method frame received: " + frame);
            }
            final AMQMethodEvent evt = new AMQMethodEvent(frame.channel, (AMQMethodBody)frame.bodyFrame, _protocolSession);
            try
            {
                boolean wasAnyoneInterested = false;
                while (it.hasNext())
                {
                    final AMQMethodListener listener = (AMQMethodListener) it.next();
                    wasAnyoneInterested = listener.methodReceived(evt) || wasAnyoneInterested;
                }
                if (!wasAnyoneInterested)
                {
                    throw new AMQException("AMQMethodEvent " + evt + " was not processed by any listener.");
                }
            }
            catch (AMQException e)
            {
                it = _frameListeners.iterator();
                while (it.hasNext())
                {
                    final AMQMethodListener listener = (AMQMethodListener) it.next();
                    listener.error(e);
                }
                exceptionCaught(session, e);
            }
        }
        else if (frame.bodyFrame instanceof ContentHeaderBody)
        {
            _protocolSession.messageContentHeaderReceived(frame.channel,
                                                          (ContentHeaderBody) frame.bodyFrame);
        }
        else if (frame.bodyFrame instanceof ContentBody)
        {
            _protocolSession.messageContentBodyReceived(frame.channel,
                                                        (ContentBody) frame.bodyFrame);
        }
        _connection.bytesReceived(_protocolSession.getIoSession().getReadBytes());
    }

    private static int _messagesOut;

    public void messageSent(IoSession session, Object message) throws Exception
    {
        if (_messagesOut++ % 1000 == 0)
        {
            _logger.warn("Sent " + _messagesOut + " protocol messages");
        }
        _connection.bytesSent(_protocolSession.getIoSession().getWrittenBytes());
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Sent frame " + message);
        }
    }

    public void addFrameListener(AMQMethodListener listener)
    {
        _frameListeners.add(listener);
    }

    public void removeFrameListener(AMQMethodListener listener)
    {
        _frameListeners.remove(listener);
    }

    public void attainState(AMQState s) throws AMQException
    {
        _stateManager.attainState(s);
    }

    /**
     * Convenience method that writes a frame to the protocol session. Equivalent
     * to calling getProtocolSession().write().
     *
     * @param frame the frame to write
     */
    public void writeFrame(AMQDataBlock frame)
    {
        _protocolSession.writeFrame(frame);
    }

    /**
     * Convenience method that writes a frame to the protocol session and waits for
     * a particular response. Equivalent to calling getProtocolSession().write() then
     * waiting for the response.
     * @param frame
     * @param listener the blocking listener. Note the calling thread will block.
     */
    public AMQMethodEvent writeCommandFrameAndWaitForReply(AMQFrame frame,
                                                           BlockingMethodFrameListener listener)
        throws AMQException
    {
        _frameListeners.add(listener);
        _protocolSession.writeFrame(frame);
        return listener.blockForFrame();
        // When control resumes before this line, a reply will have been received
        // that matches the criteria defined in the blocking listener
    }

    /**
     * Convenience method to register an AMQSession with the protocol handler. Registering
     * a session with the protocol handler will ensure that messages are delivered to the
     * consumer(s) on that session.
     *
     * @param channelId the channel id of the session
     * @param session the session instance.
     */
    public void addSessionByChannel(int channelId, AMQSession session)
    {
        _protocolSession.addSessionByChannel(channelId, session);
    }

    /**
     * Convenience method to deregister an AMQSession with the protocol handler.
     * @param channelId then channel id of the session
     */
    public void removeSessionByChannel(int channelId)
    {
        _protocolSession.removeSessionByChannel(channelId);
    }

    public void closeSession(AMQSession session) throws AMQException
    {
        BlockingMethodFrameListener listener = new SpecificMethodFrameListener(session.getChannelId(),
                                                                               ChannelCloseOkBody.class);
        _frameListeners.add(listener);
        _protocolSession.closeSession(session);
        _logger.debug("Blocking for channel close frame for channel " + session.getChannelId());
        listener.blockForFrame();
        _logger.debug("Received channel close frame");
        // When control resumes at this point, a reply will have been received that
        // indicates the broker has closed the channel successfully
    }

    public void closeConnection() throws AMQException
    {
        BlockingMethodFrameListener listener = new ConnectionCloseOkListener();
        _frameListeners.add(listener);
        _stateManager.changeState(AMQState.CONNECTION_CLOSING);

        // TODO: Polish
        final AMQFrame frame = ConnectionCloseBody.createAMQFrame(0, AMQConstant.REPLY_SUCCESS.getCode(),
                                                                  "JMS client is closing the connection.", 0, 0);
        writeFrame(frame);
        _logger.debug("Blocking for connection close ok frame");
        listener.blockForFrame();
        _protocolSession.closeProtocolSession();
    }

    /**
     * @return the number of bytes read from this protocol session
     */
    public long getReadBytes()
    {
        return _protocolSession.getIoSession().getReadBytes();
    }

    /**
     * @return the number of bytes written to this protocol session
     */
    public long getWrittenBytes()
    {
        return _protocolSession.getIoSession().getWrittenBytes();
    }
}
