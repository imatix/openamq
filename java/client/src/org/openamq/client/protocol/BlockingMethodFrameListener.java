package org.openamq.client.protocol;

import org.openamq.AMQException;
import org.openamq.framing.AMQMethodBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class BlockingMethodFrameListener implements AMQMethodListener
{
    private volatile boolean _ready = false;

    public abstract boolean processMethod(int channelId, AMQMethodBody frame) throws AMQException;

    private final Object _lock = new Object();

    /**
     * This is set if there is an exception thrown from processCommandFrame and the
     * exception is rethrown to the caller of blockForFrame()
     */
    private volatile Exception _error;

    protected int _channelId;

    protected AMQMethodEvent _doneEvt = null;

    public BlockingMethodFrameListener(int channelId)
    {
        _channelId = channelId;
    }

    /**
     * This method is called by the MINA dispatching thread. Note that it could
     * be called before blockForFrame() has been called.
     * @param evt the frame event
     * @return true if the listener has dealt with this frame
     * @throws AMQException
     */
    public boolean methodReceived(AMQMethodEvent evt) throws AMQException
    {
        AMQMethodBody method = evt.getMethod();

        try
        {
            boolean ready = (evt.getChannelId() == _channelId) && processMethod(evt.getChannelId(), method);
            if (ready)
            {
                // we only update the flag from inside the synchronized block
                // so that the blockForFrame method cannot "miss" an update - it
                // will only ever read the flag from within the synchronized block
                synchronized (_lock)
                {
                    _doneEvt = evt;
                    _ready = ready;
                    _lock.notify();
                }
            }
            return ready;
        }
        catch (AMQException e)
        {
            error(e);
            // we rethrow the error here, and the code in the frame dispatcher will go round
            // each listener informing them that an exception has been thrown
            throw e;
        }
    }

    /**
     * This method is called by the thread that wants to wait for a frame.
     */
    public AMQMethodEvent blockForFrame() throws AMQException
    {
        synchronized (_lock)
        {
            while (!_ready)
            {
                try
                {
                    _lock.wait();
                }
                catch (InterruptedException e)
                {
                    // IGNORE
                }
            }
        }
        if (_error != null)
        {
            if (_error instanceof AMQException)
            {
                throw (AMQException)_error;
            }
            else
            {
                throw new AMQException("Woken up due to exception", _error);
            }
        }

        return _doneEvt;
    }

    /**
     * This is a callback, called by the MINA dispatcher thread only. It is also called from within this
     * class to avoid code repetition but again is only called by the MINA dispatcher thread.
     * @param e
     */
    public void error(Exception e)
    {
        // set the error so that the thread that is blocking (against blockForFrame())
        // can pick up the exception and rethrow to the caller
        _error = e;
        synchronized (_lock)
        {
            _ready = true;
            _lock.notify();
        }
    }
}
