package org.openamq.pool;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * We share the executor service among several PoolingFilters. This class reference counts
 * how many filter chains are using the executor service and destroys the service, thus
 * freeing up its threads, when the count reaches zero. It recreates the service when
 * the count is incremented.
 *
 * This is particularly important on the client where failing to destroy the executor
 * service prevents the JVM from shutting down due to the existence of non-daemon threads.
 *
 */
public class ReferenceCountingExecutorService
{
    private ExecutorService _pool;

    private int _refCount = 0;

    /**
     * We need to be able to check the current reference count and if necessary
     * create the executor service atomically.
     */
    private final Object _lock = new Object();

    private int _poolSize = Integer.getInteger("amqj.read_write_pool_size", 4);

    private static final ReferenceCountingExecutorService _instance = new ReferenceCountingExecutorService();

    public static ReferenceCountingExecutorService getInstance()
    {
        return _instance;
    }

    private ReferenceCountingExecutorService()
    {
    }

    ExecutorService acquireExecutorService()
    {
        synchronized (_lock)
        {
            if (_refCount++ == 0)
            {
                _pool = Executors.newFixedThreadPool(_poolSize);
            }
            return _pool;
        }
    }

    void releaseExecutorService()
    {
        synchronized (_lock)
        {
            if (--_refCount == 0)
            {
                _pool.shutdownNow();
            }
        }
    }

    /**
     * The filters that use the executor service should call this method to get access
     * to the service. Note that this method does not alter the reference count.
     *
     * @return the underlying executor service
     */
    public ExecutorService getPool()
    {
        return _pool;
    }
}
