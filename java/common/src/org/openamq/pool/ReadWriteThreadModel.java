package org.openamq.pool;

import org.apache.mina.common.ThreadModel;
import org.apache.mina.common.IoFilterChain;
import org.apache.mina.common.ReferenceCountingIoFilterWrapper;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ReadWriteThreadModel implements ThreadModel
{
    public void buildFilterChain(IoFilterChain chain) throws Exception
    {
        ReferenceCountingExecutorService executor = ReferenceCountingExecutorService.getInstance();
        PoolingFilter asyncRead = new PoolingFilter(executor, PoolingFilter.READ_EVENTS,
                                                    "AsynchronousReadFilter");
        PoolingFilter asyncWrite = new PoolingFilter(executor, PoolingFilter.WRITE_EVENTS,
                                                     "AsynchronousWriteFilter");

        chain.addFirst("AsynchronousReadFilter", new ReferenceCountingIoFilterWrapper(asyncRead));
        chain.addLast("AsynchronousWriteFilter", new ReferenceCountingIoFilterWrapper(asyncWrite));
    }
}
