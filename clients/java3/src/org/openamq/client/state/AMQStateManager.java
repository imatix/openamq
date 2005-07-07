package org.openamq.client.state;

import org.openamq.client.protocol.*;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.AMQFrame;
import org.openamq.client.framing.Handle;
import org.openamq.client.framing.Channel;
import org.openamq.client.AMQException;
import org.apache.log4j.Logger;

import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;

import edu.emory.mathcs.backport.java.util.concurrent.CopyOnWriteArraySet;

/**
 * The state manager is responsible for managing the state of the protocol session.
 * <p/>
 * For each AMQProtocolHandler there is a separate state manager.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQStateManager implements FrameListener
{
    private static final Logger _logger = Logger.getLogger(AMQStateManager.class);

    /**
     * The current state
     */
    private AMQState _currentState;

    /**
     * Maps from an AMQState instance to a Map from Class to StateTransitionHandler.
     * The class must be a subclass of AMQFrame.
     */
    private final Map _state2HandlersMap = new HashMap();

    private CopyOnWriteArraySet _stateListeners = new CopyOnWriteArraySet();

    public AMQStateManager()
    {
        _currentState = AMQState.CONNECTION_NOT_AUTHENTICATED;
        registerListeners();
    }

    private void registerListeners()
    {
        Map frame2handlerMap = new HashMap();

        // we need to register a map for the null (i.e. all state) handlers otherwise you get
        // a stack overflow in the handler searching code when you present it with a frame for which
        // no handlers are registered
        //
        _state2HandlersMap.put(null, frame2handlerMap);

        frame2handlerMap = new HashMap();
        frame2handlerMap.put(Connection.Close.class, ConnectionCloseHandler.getInstance());
        _state2HandlersMap.put(AMQState.CONNECTION_NOT_AUTHENTICATED, frame2handlerMap);

        frame2handlerMap = new HashMap();
        frame2handlerMap.put(Connection.Challenge.class, ConnectionChallengeHandler.getInstance());
        frame2handlerMap.put(Connection.Tune.class, ConnectionTuneHandler.getInstance());
        frame2handlerMap.put(Connection.Close.class, ConnectionCloseHandler.getInstance());
        _state2HandlersMap.put(AMQState.CONNECTION_NOT_AUTHENTICATED, frame2handlerMap);

        //
        // ConnectionOpen handlers
        //
        frame2handlerMap = new HashMap();
        frame2handlerMap.put(Handle.Notify.class, HandleNotifyHandler.getInstance());
        frame2handlerMap.put(Channel.Close.class, ChannelCloseHandler.getInstance());
        frame2handlerMap.put(Connection.Close.class, ConnectionCloseHandler.getInstance());
        _state2HandlersMap.put(AMQState.CONNECTION_OPEN, frame2handlerMap);
    }

    public AMQState getCurrentState()
    {
        return _currentState;
    }

    public void changeState(AMQState newState) throws AMQException
    {
        _logger.debug("State changing to " + newState + " from old state " + _currentState);
        final AMQState oldState = _currentState;
        _currentState = newState;

        final Iterator it = _stateListeners.iterator();
        while (it.hasNext())
        {
            final StateListener l = (StateListener) it.next();
            l.stateChanged(oldState, newState);
        }
    }

    public void error(AMQException e)
    {
        _logger.debug("State manager receive error notification: " + e);
        final Iterator it = _stateListeners.iterator();
        while (it.hasNext())
        {
            final StateListener l = (StateListener) it.next();
            l.error(e);
        }
    }

    public void frameReceived(FrameEvent evt) throws AMQException
    {
        StateAwareFrameListener handler = findStateTransitionHandler(_currentState, evt.getFrame());
        if (handler != null)
        {
            handler.frameReceived(this, evt);
        }
    }

    private StateAwareFrameListener findStateTransitionHandler(AMQState currentState,
                                                               AMQFrame frame)
            throws IllegalStateTransitionException
    {
        final Class clazz = frame.getClass();
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Looking for state transition handler for frame " + clazz);
        }
        final Map classToHandlerMap = (Map) _state2HandlersMap.get(currentState);

        if (classToHandlerMap == null)
        {
            // if no specialised per state handler is registered look for a
            // handler registered for "all" states
            return findStateTransitionHandler(null, frame);
        }
        final StateAwareFrameListener handler = (StateAwareFrameListener) classToHandlerMap.get(clazz);
        if (handler == null)
        {
            if (currentState == null)
            {
                _logger.debug("No state transition handler defined for receiving frame " + frame);
                return null;
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

    public void addStateListener(StateListener listener)
    {
        _logger.debug("Adding state listener");
        _stateListeners.add(listener);
    }

    public void removeStateListener(StateListener listener)
    {
        _stateListeners.remove(listener);
    }

    public void attainState(AMQState s) throws AMQException
    {
        if (_currentState != s)
        {
            _logger.debug("Adding state wait to reach state " + s);
            StateWaiter sw = new StateWaiter(s);
            addStateListener(sw);
            sw.waituntilStateHasChanged();
            // at this point the state will have changed.
        }
    }
}
