package org.openamq.client.protocol;

import org.openamq.client.AMQException;
import org.openamq.client.framing.AMQDataBlock;
import org.openamq.client.framing.AMQCommandFrame;
import org.openamq.client.framing.AMQMethodBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class BlockingCommandFrameListener implements AMQMethodListener
{
    private volatile boolean _ready = false;

    public abstract boolean processMethod(short channelId, AMQMethodBody frame) throws AMQException;

    private final Object _lock = new Object();

    /**
     * This is set if there is an exception thrown from processCommandFrame and the
     * exception is rethrown to the caller of blockForFrame()
     */
    private volatile AMQException _error;

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
            boolean ready = processMethod(evt.getChannelId(), method);
            if (ready)
            {
                // we only update the flag from inside the synchronized block
                // so that the blockForFrame method cannot "miss" an update - it
                // will only ever read the flag from within the synchronized block
                synchronized (_lock)
                {
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
    public void blockForFrame() throws AMQException
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
            throw _error;
        }
    }

    /**
     * This is a callback, called by the MINA dispatcher thread only. It is also called from within this
     * class to avoid code repetition but again is only called by the MINA dispatcher thread.
     * @param e
     */
    public void error(AMQException e)
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
