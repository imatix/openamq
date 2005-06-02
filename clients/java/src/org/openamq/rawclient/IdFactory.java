package org.openamq.rawclient;


public class IdFactory
{
    /**
     * Each counter has its own lock for threadsafe access
     */
    private final Object[] _locks = new Object[2];
    
    private int[] _counters = new int[2];
    
    private static final int CHANNEL_ID = 0;
    
    private static final int HANDLE_ID = 1;

    private static final IdFactory _instance = new IdFactory();

    public static IdFactory getInstance()
    {
        return _instance;
    }

    private IdFactory()
    {
        for (int i = 0; i < _locks.length; i++)
        {
            _locks[i] = new Object();
            _counters[i] = 1;
        }
    }
    
    public int getChannelId()
    {
        synchronized (_locks[CHANNEL_ID])
        {
            return _counters[CHANNEL_ID]++;
        }
    }
    
    public int getHandleId()
    {
        synchronized (_locks[HANDLE_ID])
        {
            return _counters[HANDLE_ID]++;
        }
    }
}
