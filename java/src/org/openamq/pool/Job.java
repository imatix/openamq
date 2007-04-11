package org.openamq.pool;

import org.apache.mina.common.IoSession;

import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Holds events for a session that will be processed asynchronously by
 * the thread pool in PoolingFilter.
 */
class Job implements Runnable
{
    private final int _maxEvents;
    private final IoSession _session;
    private final java.util.Queue<Event> _eventQueue = new ConcurrentLinkedQueue<Event>();
    private final AtomicBoolean _active = new AtomicBoolean();
    private final AtomicInteger _refCount = new AtomicInteger();
    private final JobCompletionHandler _completionHandler;

    Job(IoSession session, JobCompletionHandler completionHandler, int maxEvents)
    {
        _session = session;
        _completionHandler = completionHandler;
        _maxEvents = maxEvents;
    }

    void acquire()
    {
        _refCount.incrementAndGet();
    }

    void release()
    {
        _refCount.decrementAndGet();
    }

    boolean isReferenced()
    {
        return _refCount.get() > 0;
    }

    void add(Event evt)
    {
        _eventQueue.add(evt);
    }

    void processAll()
    {
        //limit the number of events processed in one run
        for (int i = 0; i < _maxEvents; i++)
        {
            Event e = _eventQueue.poll();
            if (e == null)
            {
                break;
            }
            else
            {
                e.process(_session);
            }
        }
    }

    boolean isComplete()
    {
        return _eventQueue.peek() == null;
    }

    boolean activate()
    {
        return _active.compareAndSet(false, true);
    }

    void deactivate()
    {
        _active.set(false);
    }

    public void run()
    {
        processAll();
        deactivate();
        _completionHandler.completed(_session, this);
    }


    static interface JobCompletionHandler
    {
        public void completed(IoSession session, Job job);
    }
}
