/*****************************************************************************
 * Filename    : FlowControllingBlockingQueue.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.client.util;

import edu.emory.mathcs.backport.java.util.concurrent.BlockingQueue;
import edu.emory.mathcs.backport.java.util.concurrent.LinkedBlockingQueue;

/**
 * A blocking queue that emits events above a user specified threshold allowing
 * the caller to take action (e.g. flow control) to try to prevent the queue
 * growing (much) further. The underlying queue itself is not bounded therefore
 * the caller is not obliged to react to the events.
 * <p/>
 * This implementation is <b>only</b> safe where we have a single thread adding
 * items and a single (different) thread removing items.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class FlowControllingBlockingQueue
{
    /**
     * This queue is bounded and is used to store messages before being dispatched to the consumer
     */
    private final BlockingQueue _queue = new LinkedBlockingQueue();

    private final int _flowControlThreshold;

    private final ThresholdListener _listener;

    /**
     * We require a separate count so we can track whether we have reached the
     * threshold
     */
    private int _count;

    public interface ThresholdListener
    {
        void aboveThreshold(int currentValue);

        void underThreshold(int currentValue);
    }

    public FlowControllingBlockingQueue(int threshold, ThresholdListener listener)
    {
        _flowControlThreshold = threshold;
        _listener = listener;
    }

    public Object take() throws InterruptedException
    {
        Object o = _queue.take();
        synchronized (_listener)
        {
            if (--_count == (_flowControlThreshold - 1))
            {
                _listener.underThreshold(_count);
            }
        }
        return o;
    }

    public void add(Object o)
    {
        _queue.add(o);
        synchronized (_listener)
        {
            if (++_count == _flowControlThreshold)
            {
                _listener.aboveThreshold(_count);
            }
        }
    }
}
