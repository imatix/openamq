package org.openamq.client.state;

import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.Session;
import org.apache.mina.protocol.ProtocolHandler;
import org.apache.mina.protocol.ProtocolSession;
import org.openamq.client.AMQException;
import org.openamq.client.protocol.ConnectionChallengeHandler;
import org.openamq.client.protocol.ConnectionTuneHandler;
import org.openamq.client.framing.Connection;

import java.util.HashMap;
import java.util.Map;

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
    /**
     * Maps from an AMQState instance to a Map from Class to StateTransitionHandler.
     * The class must be a subclass of AMQFrame.
     */
    private final Map _statesToHandlersMap = new HashMap();

    private String _username;

    private String _password;

    private String _clientName;

    private String _virtualPath;

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
                                     String clientName, String virtualPath)
    {
        _username = username;
        _password = password;
        _clientName = clientName;
        _virtualPath = virtualPath;

        final Map frame2handlerMap = new HashMap();

        frame2handlerMap.put(Connection.Challenge.class, new ConnectionChallengeHandler());
        frame2handlerMap.put(Connection.Tune.class, new ConnectionTuneHandler());
        _statesToHandlersMap.put(AMQState.CONNECTION_NOT_AUTHENTICATED, frame2handlerMap);
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
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void sessionOpened(ProtocolSession session) throws Exception
    {
        // default state is CONNECTION_NOT_AUTHENTICATED - this is the first thing the server will
        // attempt to establish
        session.setAttribute(ProtocolSessionAttributes.CURRENT_STATE_ATTRIBUTE_KEY, AMQState.CONNECTION_NOT_AUTHENTICATED);
        // set up the session to contain mandatory items
        session.setAttribute(ProtocolSessionAttributes.CLIENT_NAME, _clientName);
        session.setAttribute(ProtocolSessionAttributes.USERNAME, _username);
        session.setAttribute(ProtocolSessionAttributes.PASSWORD, _password);
        session.setAttribute(ProtocolSessionAttributes.VIRTUAL_PATH, _virtualPath);

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
        //To change body of implemented methods use File | Settings | File Templates.
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
        if (sf.frame != null)
        {
            session.write(sf.frame);
        }
    }

    public void messageSent(ProtocolSession session, Object message) throws Exception
    {
        System.out.println("Message sent called with message " + message);
    }
}
