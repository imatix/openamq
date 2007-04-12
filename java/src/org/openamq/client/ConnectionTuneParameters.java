package org.openamq.client;

public class ConnectionTuneParameters
{
    private long _frameMax;

    private int _channelMax;

    private int _heartbeat;

    private long _txnLimit;

    public long getFrameMax()
    {
        return _frameMax;
    }

    public void setFrameMax(long frameMax)
    {
        _frameMax = frameMax;
    }

    public int getChannelMax()
    {
        return _channelMax;
    }

    public void setChannelMax(int channelMax)
    {
        _channelMax = channelMax;
    }    

    public int getHeartbeat()
    {
        return _heartbeat;
    }

    public void setHeartbeat(int hearbeat)
    {
        _heartbeat = hearbeat;
    }

    public long getTxnLimit()
    {
        return _txnLimit;
    }

    public void setTxnLimit(long txnLimit)
    {
        _txnLimit = txnLimit;
    }
}
