using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;

namespace OpenAMQ.XMS.Client.State
{
    public class AMQStateManager : IAMQMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(AMQStateManager));

        /// <summary>
        /// The current state
        /// </summary>
        private AMQState _currentState;

        /// <summary>
        /// Maps from an AMQState instance to a Map from Class to StateTransitionHandler.
        /// The class must be a subclass of AMQFrame.
        /// </summary>
        private readonly IDictionary _state2HandlersMap = new Hashtable();

        private CopyOnWriteArraySet _stateListeners = new CopyOnWriteArraySet();
        
        public AMQStateManager()
        {
            _currentState = AMQState.CONNECTION_NOT_STARTED;
            RegisterListeners();
        }

        private void RegisterListeners()
        {
            IDictionary frame2handlerMap = new Hashtable();

            // we need to register a map for the null (i.e. all state) handlers otherwise you get
            // a stack overflow in the handler searching code when you present it with a frame for which
            // no handlers are registered
            //
            _state2HandlersMap[AMQState.ALL] = frame2handlerMap;

            frame2handlerMap = new Hashtable();
            frame2handlerMap[typeof(ConnectionStartBody)] = ConnectionStartMethodHandler.GetInstance();
            frame2handlerMap[typeof(ConnectionCloseBody)] = ConnectionCloseMethodHandler.GetInstance();
            _state2HandlersMap[AMQState.CONNECTION_NOT_STARTED] = frame2handlerMap;

            frame2handlerMap = new Hashtable();
            frame2handlerMap[typeof(ConnectionTuneBody)] = ConnectionTuneMethodHandler.GetInstance();
            frame2handlerMap[typeof(ConnectionSecureBody)] = ConnectionSecureMethodHandler.GetInstance();
            frame2handlerMap[typeof(ConnectionCloseBody)] = ConnectionCloseMethodHandler.GetInstance();
            _state2HandlersMap[AMQState.CONNECTION_NOT_TUNED] = frame2handlerMap;

            frame2handlerMap = new Hashtable();
            frame2handlerMap[typeof(ConnectionOpenOkBody)] = ConnectionOpenOkMethodHandler.GetInstance();
            frame2handlerMap[typeof(ConnectionCloseBody)] = ConnectionCloseMethodHandler.GetInstance();
            _state2HandlersMap[AMQState.CONNECTION_NOT_OPENED] = frame2handlerMap;

            //
            // ConnectionOpen handlers
            //
            frame2handlerMap = new Hashtable();
            frame2handlerMap[typeof(ChannelCloseBody)] = ChannelCloseMethodHandler.GetInstance();
            frame2handlerMap[typeof(ConnectionCloseBody)] = ConnectionCloseMethodHandler.GetInstance();
            frame2handlerMap[typeof(BasicDeliverBody)] = BasicDeliverMethodHandler.GetInstance();
            frame2handlerMap[typeof(BasicReturnBody)] = BasicReturnMethodHandler.GetInstance();
            _state2HandlersMap[AMQState.CONNECTION_OPEN] = frame2handlerMap;
        }

        public AMQState CurrentState
        {
            get
            {
                return _currentState;
            }
        }

        /// <summary>
        /// Changes the state.
        /// </summary>
        /// <param name="newState">The new state.</param>
        /// <exception cref="AMQException">if there is an error changing state</exception>
        public void ChangeState(AMQState newState)
        {
            _logger.Debug("State changing to " + newState + " from old state " + _currentState);
            AMQState oldState = _currentState;
            _currentState = newState;

            foreach (IStateListener l in _stateListeners)
            {
                l.StateChanged(oldState, newState);
            }
        }

        public void Error(AMQException e)
        {
            _logger.Debug("State manager receive error notification: " + e);
            foreach (IStateListener l in _stateListeners)
            {
                l.Error(e);
            }
        }

        public bool MethodReceived(AMQMethodEvent evt)
        {
            IStateAwareMethodListener handler = FindStateTransitionHandler(_currentState, evt.Method);
            if (handler != null)
            {
                handler.MethodReceived(this, evt);
                return true;
            }
            return false;
        }

        /// <summary>
        /// Finds the state transition handler.
        /// </summary>
        /// <param name="currentState">State of the current.</param>
        /// <param name="frame">The frame.</param>
        /// <returns></returns>
        /// <exception cref="IllegalStateTransitionException">if the state transition if not allowed</exception>
        private IStateAwareMethodListener FindStateTransitionHandler(AMQState currentState,
                                                                     AMQMethodBody frame)                
        {
            Type clazz = frame.GetType();
            if (_logger.IsDebugEnabled)
            {
                _logger.Debug("Looking for state transition handler for frame " + clazz);
            }
            IDictionary classToHandlerMap = (IDictionary) _state2HandlersMap[currentState];

            if (classToHandlerMap == null)
            {
                // if no specialised per state handler is registered look for a
                // handler registered for "all" states
                return FindStateTransitionHandler(AMQState.ALL, frame);
            }
            IStateAwareMethodListener handler = (IStateAwareMethodListener) classToHandlerMap[clazz];
            if (handler == null)
            {
                if (currentState == AMQState.ALL)
                {
                    _logger.Debug("No state transition handler defined for receiving frame " + frame);
                    return null;
                }
                else
                {
                    // if no specialised per state handler is registered look for a
                    // handler registered for "all" states
                    return FindStateTransitionHandler(AMQState.ALL, frame);
                }
            }
            else
            {
                return handler;
            }
        }

        public void AddStateListener(IStateListener listener)
        {
            _logger.Debug("Adding state listener");
            _stateListeners.Add(listener);
        }

        public void RemoveStateListener(IStateListener listener)
        {
            _stateListeners.Remove(listener);
        }

        public void AttainState(AMQState s)
        {
            if (_currentState != s)
            {
                _logger.Debug("Adding state wait to reach state " + s);
                StateWaiter sw = new StateWaiter(s);
                AddStateListener(sw);
                sw.WaituntilStateHasChanged();
                // at this point the state will have changed.
            }
        }        
    }
}
