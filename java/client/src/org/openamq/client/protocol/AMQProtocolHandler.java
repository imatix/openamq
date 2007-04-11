package org.openamq.client.protocol;

import edu.emory.mathcs.backport.java.util.concurrent.CopyOnWriteArraySet;
import edu.emory.mathcs.backport.java.util.concurrent.CountDownLatch;
import org.apache.log4j.Logger;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.SSLFilter;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.openamq.AMQDisconnectedException;
import org.openamq.AMQException;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.listener.ConnectionCloseOkListener;
import org.openamq.client.state.listener.SpecificMethodFrameListener;
import org.openamq.codec.AMQCodecFactory;
import org.openamq.framing.*;
import org.openamq.ssl.BogusSSLContextFactory;

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
     * Used only when determining whether to add the SSL filter or not. This should be made more
     * generic in future since we will potentially have many transport layer options
     */
    private boolean _useSSL;

    /**
     * Our wrapper for a protocol session that provides access to session values
     * in a typesafe manner.
     */
    private volatile AMQProtocolSession _protocolSession;

    private AMQStateManager _stateManager = new AMQStateManager();

    private final CopyOnWriteArraySet _frameListeners = new CopyOnWriteArraySet();

    /**
     * We create the failover handler when the session is created since it needs a reference to the IoSession in order
     * to be able to send errors during failover back to the client application. The session won't be available in the
     * case where we failing over due to a Connection.Redirect message from the broker.
     */
    private FailoverHandler _failoverHandler;

    /**
     * This flag is used to track whether failover is being attempted. It is used to prevent the application constantly
     * attempting failover where it is failing.
     */
    private FailoverState _failoverState = FailoverState.NOT_STARTED;

    private CountDownLatch _failoverLatch;

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

        /**
         * Used where forcing the failover host
         */
        private String _host;

        /**
         * Used where forcing the failover port
         */
        private int _port;

        public FailoverHandler(IoSession session)
        {
            _session = session;
        }

        public void run()
        {
            _failoverLatch = new CountDownLatch(1);
            // we wake up listeners. If they can handle failover, they will extend the
            // FailoverSupport class and will in turn block on the latch until failover
            // has completed before retrying the operation
            propagateExceptionToWaiters(new FailoverException("Failing over about to start"));
            // since failover impacts several structures we protect them all with a single mutex. These structures
            // are also in child objects of the connection. This allows us to manipulate them without affecting
            // client code which runs in a separate thread
            synchronized (_connection.getFailoverMutex())
            {
                _logger.info("Starting failover process");
                // we switch in a new state manager temporarily so that the interaction to get to the "connection open"
                // state works, without us having to terminate any existing "state waiters". We could theoretically
                // have a state waiter waiting until the connection is closed for some reason. Or in future we may have
                // a slightly more complex state model therefore I felt it was worthwhile doing this
                AMQStateManager existingStateManager = _stateManager;
                _stateManager = new AMQStateManager();
                if (!_connection.firePreFailover(_host != null))
                {
                    _stateManager = existingStateManager;
                    if (_host != null)
                    {
                        _connection.exceptionReceived(new AMQDisconnectedException("Redirect was vetoed by client"));
                    }
                    else
                    {
                        _connection.exceptionReceived(new AMQDisconnectedException("Failover was vetoed by client"));
                    }
                    _failoverLatch.countDown();
                    _failoverLatch = null;
                    return;
                }
                boolean failoverSucceeded;
                // when host is non null we have a specified failover host otherwise we all the client to cycle through
                // all specified hosts
                if (_host != null)
                {
                    failoverSucceeded = _connection.attemptReconnection(_host, _port, _useSSL);
                }
                else
                {
                    failoverSucceeded = _connection.attemptReconnection();
                }
                if (!failoverSucceeded)
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
                            _logger.info("Resubscribing on new connection");
                            _connection.resubscribeSessions();
                        }
                        else
                        {
                            _logger.info("Client vetoed automatic resubscription");
                        }
                        _connection.fireFailoverComplete();
                        _failoverState = FailoverState.NOT_STARTED;
                        _logger.info("Connection failover completed successfully");
                    }
                    catch (Exception e)
                    {
                        _logger.info("Failover process failed - exception being propagated by protocol handler");
                        _failoverState = FailoverState.FAILED;
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
            _failoverLatch.countDown();
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

            public void error(Exception e)
            {
                _stateManager.error(e);
            }
        });
    }

    public boolean isUseSSL()
    {
        return _useSSL;
    }

    public void setUseSSL(boolean useSSL)
    {
        _useSSL = useSSL;
    }

    public void sessionCreated(IoSession session) throws Exception
    {
        _logger.debug("Protocol session created for session " + System.identityHashCode(session));
        _failoverHandler = new FailoverHandler(session);

        final ProtocolCodecFilter pcf = new ProtocolCodecFilter(new AMQCodecFactory(false));

        if (Boolean.getBoolean("amqj.shared_read_write_pool"))
        {
            session.getFilterChain().addBefore("AsynchronousWriteFilter", "protocolFilter", pcf);
        }
        else
        {
            session.getFilterChain().addLast("protocolFilter", pcf);
        }
        // we only add the SSL filter where we have an SSL connection
        if (_useSSL)
        {
            SSLFilter sslFilter = new SSLFilter(BogusSSLContextFactory.getInstance(false));
            sslFilter.setUseClientMode(true);
            session.getFilterChain().addBefore("protocolFilter", "ssl", sslFilter);
        }

        _protocolSession = new AMQProtocolSession(this, session, _connection);
        _protocolSession.init();
    }

    public void sessionOpened(IoSession session) throws Exception
    {
    }

    /**
     * When the broker connection dies we can either get sessionClosed() called or exceptionCaught() followed by
     * sessionClosed() depending on whether we were trying to send data at the time of failure.
     * @param session
     * @throws Exception
     */
    public void sessionClosed(IoSession session) throws Exception
    {
        _logger.info("Session closed called with failover state currently " + _failoverState);

        if (_failoverState == FailoverState.NOT_STARTED && !_connection.isClosed())
        {
            _failoverState = FailoverState.IN_PROGRESS;
            // see javadoc for FailoverHandler to see rationale for separate thread
            new Thread(_failoverHandler).start();
        }

        _logger.info("Protocol Session [" + this + "] closed");
    }

    public void sessionIdle(IoSession session, IdleStatus status) throws Exception
    {
        _logger.debug("Protocol Session [" + this  + ":" + session + "] idle: " + status);
        if(IdleStatus.WRITER_IDLE.equals(status))
        {
            //write heartbeat frame:
            _logger.debug("Sent heartbeat");
            session.write(HeartbeatBody.FRAME);
            HeartbeatDiagnostics.sent();
        }
        else if(IdleStatus.READER_IDLE.equals(status))
        {
            //failover:
            HeartbeatDiagnostics.timeout();
            _logger.warn("Timed out while waiting for heartbeat from peer.");
            session.close();
        }
    }

    public void exceptionCaught(IoSession session, Throwable cause) throws Exception
    {
        if (_failoverState == FailoverState.NOT_STARTED)
        {
            _logger.info("Exception caught therefore going to attempt failover: " + cause, cause);
            // this will attemp failover
            sessionClosed(session);
        }
        // we reach this point if failover was attempted and failed therefore we need to let the calling app
        // know since we cannot recover the situation
        else if (_failoverState == FailoverState.FAILED)
        {
            _logger.error("Exception caught by protocol handler: " + cause, cause);
            _connection.exceptionReceived(cause);
            // we notify the state manager of the error in case we have any clients waiting on a state
            // change. Those "waiters" will be interrupted and can handle the exception
            AMQException amqe = new AMQException("Protocol handler error: " + cause, cause);
            propagateExceptionToWaiters(amqe);
        }
    }

    /**
     * There are two cases where we have other threads potentially blocking for events to be handled by this
     * class. These are for the state manager (waiting for a state change) or a frame listener (waiting for a
     * particular type of frame to arrive). When an error occurs we need to notify these waiters so that they can
     * react appropriately.
     * @param e the exception to propagate
     */
    private void propagateExceptionToWaiters(Exception e)
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
            _logger.debug("Received " + _messageReceivedCount + " protocol messages");
        }
        Iterator it = _frameListeners.iterator();
        AMQFrame frame = (AMQFrame) message;

        HeartbeatDiagnostics.received(frame.bodyFrame instanceof HeartbeatBody);

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
        else if (frame.bodyFrame instanceof HeartbeatBody)
        {
            _logger.debug("Received heartbeat");
        }
        _connection.bytesReceived(_protocolSession.getIoSession().getReadBytes());
    }

    private static int _messagesOut;

    public void messageSent(IoSession session, Object message) throws Exception
    {
        if (_messagesOut++ % 1000 == 0)
        {
            _logger.debug("Sent " + _messagesOut + " protocol messages");
        }
        _connection.bytesSent(session.getWrittenBytes());
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

    public void writeFrame(AMQDataBlock frame, boolean wait)
    {
        _protocolSession.writeFrame(frame, wait);
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

    public void failover(String host, int port)
    {
        _failoverHandler._host = host;
        _failoverHandler._port = port;
        // see javadoc for FailoverHandler to see rationale for separate thread
        new Thread(_failoverHandler).start();
    }

    public void blockUntilNotFailingOver() throws InterruptedException
    {
        if (_failoverLatch != null)
        {
            _failoverLatch.await();
        }
    }


    public String generateQueueName()
    {
        return _protocolSession.generateQueueName();
    }
}
