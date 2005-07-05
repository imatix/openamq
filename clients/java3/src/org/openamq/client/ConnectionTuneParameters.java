package org.openamq.client;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionTuneParameters
{
    private long _frameMax;

    private long _channelMax;

    private long _handleMax;

    private long _hearbeat;

    private long _txnLimit;

    public long getFrameMax()
    {
        return _frameMax;
    }

    public void setFrameMax(long frameMax)
    {
        _frameMax = frameMax;
    }

    public long getChannelMax()
    {
        return _channelMax;
    }

    public void setChannelMax(long channelMax)
    {
        _channelMax = channelMax;
    }

    public long getHandleMax()
    {
        return _handleMax;
    }

    public void setHandleMax(long handleMax)
    {
        _handleMax = handleMax;
    }

    public long getHearbeat()
    {
        return _hearbeat;
    }

    public void setHearbeat(long hearbeat)
    {
        _hearbeat = hearbeat;
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
