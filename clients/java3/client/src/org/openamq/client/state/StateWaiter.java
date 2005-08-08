package org.openamq.client.state;

import org.apache.log4j.Logger;
import org.openamq.AMQException;

/**
 * Waits for a particular state to be reached.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class StateWaiter implements StateListener
{
    private static final Logger _logger = Logger.getLogger(StateWaiter.class);

    private final AMQState _state;

    private volatile boolean _newStateAchieved;

    private volatile Throwable _throwable;

    private final Object _monitor = new Object();

    public StateWaiter(AMQState state)
    {
        _state = state;
    }

    public void waituntilStateHasChanged() throws AMQException
    {
        synchronized (_monitor)
        {
            //
            // The guard is required in case we are woken up by a spurious
            // notify().
            //
            while (!_newStateAchieved && _throwable == null)
            {
                try
                {
                    _logger.debug("State not achieved so waiting...");
                    _monitor.wait();
                }
                catch (InterruptedException e)
                {
                    ;
                }
            }
        }

        if (_throwable != null)
        {
            _logger.debug("Throwable reached state waiter: " + _throwable);
            if (_throwable instanceof AMQException)
            {
                throw (AMQException) _throwable;
            }
            else
            {
                throw new AMQException("Error: "  + _throwable, _throwable);
            }
        }
    }

    public void stateChanged(AMQState oldState, AMQState newState)
    {
        synchronized (_monitor)
        {
            if (_logger.isDebugEnabled())
            {
                _logger.debug("stateChanged called");
            }
            if (_state == newState)
            {
                _newStateAchieved = true;

                if (_logger.isDebugEnabled())
                {
                    _logger.debug("New state reached so notifying monitor");
                }
                _monitor.notifyAll();
            }
        }
    }

    public void error(Throwable t)
    {
        synchronized (_monitor)
        {
            if (_logger.isDebugEnabled())
            {
                _logger.debug("exceptionThrown called");
            }

            _throwable = t;
            _monitor.notifyAll();
        }
    }
}
