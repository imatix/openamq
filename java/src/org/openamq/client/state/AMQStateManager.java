package org.openamq.client.state;

import edu.emory.mathcs.backport.java.util.concurrent.CopyOnWriteArraySet;
import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.framing.AMQMethodBody;
import org.openamq.client.protocol.*;
import org.openamq.client.handler.*;
import org.openamq.framing.*;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * The state manager is responsible for managing the state of the protocol session.
 * <p/>
 * For each AMQProtocolHandler there is a separate state manager.
 *
 */
public class AMQStateManager implements AMQMethodListener
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

    private final CopyOnWriteArraySet _stateListeners = new CopyOnWriteArraySet();

    public AMQStateManager()
    {
        this(AMQState.CONNECTION_NOT_STARTED, true);
    }

    protected AMQStateManager(AMQState state, boolean register)
    {
        _currentState = state;
        if(register)
        {
            registerListeners();
        }
    }

    protected void registerListeners()
    {
        Map frame2handlerMap = new HashMap();

        // we need to register a map for the null (i.e. all state) handlers otherwise you get
        // a stack overflow in the handler searching code when you present it with a frame for which
        // no handlers are registered
        //
        _state2HandlersMap.put(null, frame2handlerMap);

        frame2handlerMap = new HashMap();
        frame2handlerMap.put(ConnectionStartBody.class, ConnectionStartMethodHandler.getInstance());
        frame2handlerMap.put(ConnectionCloseBody.class, ConnectionCloseMethodHandler.getInstance());
        _state2HandlersMap.put(AMQState.CONNECTION_NOT_STARTED, frame2handlerMap);

        frame2handlerMap = new HashMap();
        frame2handlerMap.put(ConnectionTuneBody.class, ConnectionTuneMethodHandler.getInstance());
        frame2handlerMap.put(ConnectionSecureBody.class, ConnectionSecureMethodHandler.getInstance());
        frame2handlerMap.put(ConnectionCloseBody.class, ConnectionCloseMethodHandler.getInstance());
        _state2HandlersMap.put(AMQState.CONNECTION_NOT_TUNED, frame2handlerMap);

        frame2handlerMap = new HashMap();
        frame2handlerMap.put(ConnectionOpenOkBody.class, ConnectionOpenOkMethodHandler.getInstance());
        frame2handlerMap.put(ConnectionCloseBody.class, ConnectionCloseMethodHandler.getInstance());
        _state2HandlersMap.put(AMQState.CONNECTION_NOT_OPENED, frame2handlerMap);

        //
        // ConnectionOpen handlers
        //
        frame2handlerMap = new HashMap();
        frame2handlerMap.put(ChannelCloseBody.class, ChannelCloseMethodHandler.getInstance());
        frame2handlerMap.put(ConnectionCloseBody.class, ConnectionCloseMethodHandler.getInstance());
        frame2handlerMap.put(BasicDeliverBody.class, BasicDeliverMethodHandler.getInstance());
        frame2handlerMap.put(BasicReturnBody.class, BasicReturnMethodHandler.getInstance());
        frame2handlerMap.put(ChannelFlowOkBody.class, ChannelFlowOkMethodHandler.getInstance());
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

        synchronized (_stateListeners)
        {
            final Iterator it = _stateListeners.iterator();
            while (it.hasNext())
            {
                final StateListener l = (StateListener) it.next();
                l.stateChanged(oldState, newState);
            }
        }
    }

    public void error(Exception e)
    {
        _logger.debug("State manager receive error notification: " + e);
        synchronized (_stateListeners)
        {
            final Iterator it = _stateListeners.iterator();
            while (it.hasNext())
            {
                final StateListener l = (StateListener) it.next();
                l.error(e);
            }
        }
    }

    public boolean methodReceived(AMQMethodEvent evt) throws AMQException
    {
        StateAwareMethodListener handler = findStateTransitionHandler(_currentState, evt.getMethod());
        if (handler != null)
        {
            handler.methodReceived(this, evt);
            return true;
        }
        return false;
    }

    protected StateAwareMethodListener findStateTransitionHandler(AMQState currentState,
                                                               AMQMethodBody frame)
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
        final StateAwareMethodListener handler = (StateAwareMethodListener) classToHandlerMap.get(clazz);
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
        boolean needToWait = false;
        StateWaiter sw = null;
        synchronized (_stateListeners)
        {
            if (_currentState != s)
            {
                _logger.debug("Adding state wait to reach state " + s);
                sw = new StateWaiter(s);
                addStateListener(sw);
                // we use a boolean since we must release the lock before starting to wait
                needToWait = true;
            }
        }
        if (needToWait)
        {
            sw.waituntilStateHasChanged();
        }
        // at this point the state will have changed.
    }
}
