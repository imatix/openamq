package org.openamq.client;

import edu.emory.mathcs.backport.java.util.concurrent.atomic.AtomicBoolean;

import javax.jms.JMSException;

/**
 * Provides support for orderly shutdown of an object.
 */
public abstract class Closeable
{
    /**
     * We use an atomic boolean so that we do not have to synchronized access to this flag. Synchronizing
     * access to this flag would mean have a synchronized block in every method.
     */
    protected final AtomicBoolean _closed = new AtomicBoolean(false);

    protected void checkNotClosed()
    {
        if (_closed.get())
        {
            throw new IllegalStateException("Object " + toString() + " has been closed");
        }
    }

    public boolean isClosed()
    {
        return _closed.get();
    }

    public abstract void close() throws JMSException;
}
