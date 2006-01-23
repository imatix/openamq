package org.openamq.client.protocol;

import edu.emory.mathcs.backport.java.util.concurrent.CopyOnWriteArraySet;
import org.apache.log4j.Logger;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.transport.socket.nio.SocketSession;
import org.openamq.AMQException;
import org.openamq.AMQDisconnectedException;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.listener.ConnectionCloseOkListener;
import org.openamq.client.state.listener.SpecificMethodFrameListener;
import org.openamq.client.transport.AMQProtocolProvider;
import org.openamq.framing.*;

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

    private AMQStateManager _stateManager = new AMQStateManager();

    private final CopyOnWriteArraySet _frameListeners = new CopyOnWriteArraySet();

    /**
     * When failover is required, we need a separate thread to handle the establishment of the new connection and
     * the transfer of subscriptions.
     *
     * The reason this needs to be a separate thread is because you cannot do this work inside the MINA IO processor
     * thread. One significant task is the connection setup which involves a protocol exchange until a particular state
     * is achieved. However if you do this in the MINA thread, you have to block until the state is achieved which means
     * the IO processor is not able to do anything at all.
     */
    private class FailoverHandler implements Runnable
    {
        private final IoSession _session;

        public FailoverHandler(IoSession session)
        {
            _session = session;
        }

        public void run()
        {
            // we switch in a new state manager temporarily so that the interaction to get to the "connection open"
            // state works, without us having to terminate any existing "state waiters". We could theoretically
            // have a state waiter waiting until the connection is closed for some reason. Or in future we may have
            // a slightly more complex state model therefore I felt it was worthwhile doing this
            AMQStateManager existingStateManager = _stateManager;
            _stateManager = new AMQStateManager();
            if (_connection.firePreFailover() && !_connection.attemptReconnection())
            {
                _stateManager = existingStateManager;
                _connection.exceptionReceived(new AMQDisconnectedException("Server closed connection and no failover " +
                                                                           "was successful"));
            }
            else
            {
                _stateManager = existingStateManager;
                try
                {
                    if (_connection.firePreResubscribe())
                    {
                        _connection.resubscribeSessions();
                    }
                    _connection.fireFailoverComplete();
                    _logger.info("Connection failover completed successfully");
                }
                catch (Exception e)
                {
                    try
                    {
                        exceptionCaught(_session, e);
                    }
                    catch (Exception ex)
                    {
                        _logger.error("Error notifying protocol session of error: " + ex, ex);
                    }
                }
            }
        }
    }

    public AMQProtocolHandler(AMQConnection con)
    {
        _connection = con;
        // we add a proxy for the state manager so that we can substitute the state manager easily in this class.
        // We substitute the state manager when performing failover
        _frameListeners.add(new AMQMethodListener()
        {
            public boolean methodReceived(AMQMethodEvent evt) throws AMQException
            {
                return _stateManager.methodReceived(evt);
            }

            public void error(AMQException e)
            {
                _stateManager.error(e);
            }
        });
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
            // see javadoc for FailoverHandler to see rationale for separate thread
            new Thread(new FailoverHandler(session)).start();
        }

        _logger.info("Protocol Session [" + this + "] closed");
    }

    public void sessionIdle(IoSession session, IdleStatus status) throws Exception
    {
        _logger.info("Protocol Session [" + this + "] idle");
    }

    public void exceptionCaught(IoSession session, Throwable cause) throws Exception
    {
        _logger.error("Exception caught by protocol handler: " + cause, cause);
        _connection.exceptionReceived(cause);
        // we notify the state manager of the error in case we have any clients waiting on a state
        // change. Those "waiters" will be interrupted and can handle the exception
        AMQException amqe = new AMQException("Protocol handler error: " + cause, cause);
        propagateExceptionToWaiters(amqe);
    }

    /**
     * There are two cases where we have other threads potentially blocking for events to be handled by this
     * class. These are for the state manager (waiting for a state change) or a frame listener (waiting for a
     * particular type of frame to arrive). When an error occurs we need to notify these waiters so that they can
     * react appropriately.
     * @param e the exception to propagate
     */
    private void propagateExceptionToWaiters(AMQException e)
    {
        _stateManager.error(e);
        final Iterator it = _frameListeners.iterator();
        while (it.hasNext())
        {
            final AMQMethodListener ml = (AMQMethodListener) it.next();
            ml.error(e);
        }
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
