package org.openamq.client;


public class IdFactory
{
    private Object[] _locks = new Object[1];

    private short[] _counters = new short[1];

    private static final int CHANNEL_ID = 0;

    public IdFactory()
    {
        for (int i = 0; i < _locks.length; i++)
        {
            _locks[i] = new Object();
            // need to initialise our ids to 1
            _counters[i] = 1;
        }
    }

    public short getChannelId()
    {
        synchronized (_locks[CHANNEL_ID])
        {
            return _counters[CHANNEL_ID]++;
        }
    }
}
