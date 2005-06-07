package org.openamq.client.state;

import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.Session;
import org.apache.mina.protocol.ProtocolHandler;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQException;
import org.openamq.client.AMQSession;
import org.openamq.client.framing.AMQFrame;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.Channel;
import org.openamq.client.framing.Handle;
import org.openamq.client.protocol.*;

import java.util.*;

/**
 * Manages states and the transitions between states, by acting on protocol "messages".
 * In order to implement the AMQ protocol, StateAwareProtoclHandler instances are registered
 * with this class. Each state aware protocol handler can decide what to do given an
 * incoming message and the current system state.
 *
 * Although each protocol handler instance is effectively stateless, arbitrary data can
 * be associated with a protocol session.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class StateAwareProtocolHandler implements ProtocolHandler
{
    private static final Logger _logger = Logger.getLogger(StateAwareProtocolHandler.class);

    /**
     * Maps from an AMQState instance to a Map from Class to StateTransitionHandler.
     * The class must be a subclass of AMQFrame.
     */
    private final Map _statesToHandlersMap = new HashMap();

    private String _username;

    private String _password;

    private String _clientName;

    private String _virtualPath;

    private ProtocolSession _protocolSession;

    private AMQConnection _connection;

    /**
     * Maps from a frame class to a list of blocking listeners
     */
    private Map _blockingFrameListeners = new HashMap();

    private final Object _blockingFrameListenersLock = new Object();

    /**
     * Create a protocol handler. The argument to this instance are eventually
     * fed into the protocol session to be available for use by the protocol
     * handlers.
     * @param username user name to use for authentication
     * @param password password to use for authentication
     * @param clientName client identifier, must be unique
     * @param virtualPath virtual path to connect to
     */
    public StateAwareProtocolHandler(String username, String password,
                                     String clientName, String virtualPath,
                                     AMQConnection connection)
    {
        _username = username;
        _password = password;
        _clientName = clientName;
        _virtualPath = virtualPath;
        _connection = connection;

        connection.setProtocolHandler(this);

        // global handlers
        Map frame2handlerMap = new HashMap();
        frame2handlerMap.put(Connection.Close.class, new ConnectionCloseHandler());
        _statesToHandlersMap.put(null, frame2handlerMap);

        // Connection Not Authenticated Handlers
        frame2handlerMap = new HashMap();
        frame2handlerMap.put(Connection.Challenge.class, new ConnectionChallengeHandler());
        frame2handlerMap.put(Connection.Tune.class, new ConnectionTuneHandler());
        _statesToHandlersMap.put(AMQState.CONNECTION_NOT_AUTHENTICATED, frame2handlerMap);

        // Connection Tuned Handlers
        frame2handlerMap = new HashMap();
        frame2handlerMap.put(Channel.Reply.class, new NoopStateTransitionHandler());
        frame2handlerMap.put(Handle.Reply.class, new NoopStateTransitionHandler());
        frame2handlerMap.put(Handle.Notify.class, new HandleNotifyHandler());
        _statesToHandlersMap.put(AMQState.CONNECTION_TUNED, frame2handlerMap);
    }

    /**
     * Transitions from one state to another state.
     * @param currentState
     * @param frame
     * @return
     */
    public StateAndFrame transitionState(AMQState currentState, Object frame, Session session)
            throws AMQException
    {
        final StateTransitionHandler handler = findStateTransitionHandler(currentState, frame);
        return handler.messageReceived(currentState, frame, session);
    }

    private StateTransitionHandler findStateTransitionHandler(AMQState currentState,
                                                              Object frame)
            throws IllegalStateTransitionException
    {
        final Class clazz = frame.getClass();
        final Map classToHandlerMap = (Map) _statesToHandlersMap.get(currentState);

        if (classToHandlerMap == null)
        {
            // if no specialised per state handler is registered look for a
            // handler registered for "all" states
            return findStateTransitionHandler(null, frame);
        }
        final StateTransitionHandler handler = (StateTransitionHandler) classToHandlerMap.get(clazz);
        if (handler == null)
        {
            if (currentState == null)
            {
                throw new IllegalStateTransitionException(currentState, clazz);
            }
            else
            {
                // if no specialised per state handler is registered look for a
                // handler registered for "all" states
                return findStateTransitionHandler(null, frame);
            }
        }
        else
        {
            return handler;
        }
    }

    public void sessionCreated(ProtocolSession session) throws Exception
    {
        // TODO: check when this is actually called
        _protocolSession = session;
    }

    public void sessionOpened(ProtocolSession session) throws Exception
    {
        _protocolSession = session;
        // default state is CONNECTION_NOT_AUTHENTICATED - this is the first thing the server will
        // attempt to establish
        session.setAttribute(ProtocolSessionAttributes.CURRENT_STATE_ATTRIBUTE_KEY, AMQState.CONNECTION_NOT_AUTHENTICATED);
        // set up the session to contain mandatory items
        session.setAttribute(ProtocolSessionAttributes.CLIENT_NAME, _clientName);
        session.setAttribute(ProtocolSessionAttributes.USERNAME, _username);
        session.setAttribute(ProtocolSessionAttributes.PASSWORD, _password);
        session.setAttribute(ProtocolSessionAttributes.VIRTUAL_PATH, _virtualPath);
        session.setAttribute(ProtocolSessionAttributes.AMQ_CONNECTION, _connection);

        session.write(new Connection.Initiation());
    }

    public void sessionClosed(ProtocolSession session) throws Exception
    {
        System.out.println("Session closed called");
    }

    public void sessionIdle(ProtocolSession session, IdleStatus status) throws Exception
    {
        System.out.println("Session idle called");
    }

    public void exceptionCaught(ProtocolSession session, Throwable cause) throws Exception
    {
        _logger.error("Error in protocol handler: " + cause, cause);
    }

    public void messageReceived(ProtocolSession session, Object message) throws Exception
    {
        final AMQState currentState = (AMQState) session.getAttribute(ProtocolSessionAttributes.CURRENT_STATE_ATTRIBUTE_KEY);
        if (currentState == null)
        {
            throw new AMQException("Fatal internal error: protocol state not known");
        }
        final StateTransitionHandler handler = findStateTransitionHandler(currentState, message);
        final StateAndFrame sf = handler.messageReceived(currentState, message, session);
        session.setAttribute(ProtocolSessionAttributes.CURRENT_STATE_ATTRIBUTE_KEY, sf.state);
        notifyFrameListeners((AMQFrame) message);
        if (sf.frame != null)
        {
            session.write(sf.frame);
        }
    }

    public void messageSent(ProtocolSession session, Object message) throws Exception
    {
        System.out.println("Message sent called with message " + message);
    }

    public void writeFrameToSession(AMQFrame frame, BlockingFrameListener listener)
    {
        addListenerToBlockingMap(listener);
        _protocolSession.write(frame);
        while (!listener.readyToContinue())
        {
            synchronized (listener)
            {
                try
                {
                    listener.wait();
                }
                catch (InterruptedException e)
                {
                    // IGNORE
                }
            }
        }
    }

    private void addListenerToBlockingMap(BlockingFrameListener listener)
    {
        final Class interestingFrame = listener.getInterestingFrame();
        synchronized (_blockingFrameListenersLock)
        {
            LinkedList listeners = (LinkedList) _blockingFrameListeners.get(interestingFrame);
            if (listeners == null)
            {
                listeners = new LinkedList();
                _blockingFrameListeners.put(interestingFrame, listeners);
            }
            listeners.add(listener);
        }
    }

    /**
     * The lock must be held on the list before calling this method.
     * @param listener
     */
    private void removeListenerFromBlockingMap(BlockingFrameListener listener)
    {
        final Class interestingFrame = listener.getInterestingFrame();

        LinkedList listeners = (LinkedList) _blockingFrameListeners.get(interestingFrame);
        listeners.remove(listener);
    }

    private void notifyFrameListeners(AMQFrame frame)
    {
        LinkedList listeners = (LinkedList) _blockingFrameListeners.get(frame.getClass());
        if (listeners == null)
        {
            return;
        }
        else
        {
            final Iterator it = listeners.iterator();

            LinkedList removeCandidates = new LinkedList();
            while (it.hasNext())
            {
                BlockingFrameListener listener = (BlockingFrameListener) it.next();
                listener.frameReceived(frame);
                if (listener.readyToContinue())
                {
                    removeCandidates.add(listener);
                    synchronized (listener)
                    {
                        // Note: really there should only be a single waiter...
                        listener.notifyAll();
                    }
                }
            }

            final Iterator removeIt = removeCandidates.iterator();
            synchronized (_blockingFrameListenersLock)
            {
                while (removeIt.hasNext())
                {
                    removeListenerFromBlockingMap((BlockingFrameListener) removeIt.next());
                }
            }
        }
    }

    public void addSessionByHandle(int handleId,AMQSession session)
    {
    	Map map = (Map)_protocolSession.getAttribute(ProtocolSessionAttributes.AMQ_HANDLE_TO_SESSION_MAP);

    	if (map == null)
    	{
    		map = new HashMap();

    		_protocolSession.setAttribute(ProtocolSessionAttributes.AMQ_HANDLE_TO_SESSION_MAP,map);
    	}

    	map.put(new Integer(handleId),session);
    }
}
