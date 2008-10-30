package org.openamq.client.protocol;

import org.apache.mina.common.IoFilterAdapter;
import org.apache.mina.common.IoSession;
import org.apache.log4j.Logger;

/**
 * A MINA filter that monitors the numbers of messages pending to be sent by MINA. It outputs a message
 * when a threshold has been exceeded, and has a frequency configuration so that messages are not output
 * too often.
 *
 */
public class ProtocolBufferMonitorFilter extends IoFilterAdapter
{
    private static final Logger _logger = Logger.getLogger(ProtocolBufferMonitorFilter.class);

    public static long DEFAULT_FREQUENCY = 5000;

    public static int DEFAULT_THRESHOLD = 3000;

    private int  _bufferedMessages = 0;

    private int _threshold;

    private long _lastMessageOutputTime;

    private long _outputFrequencyInMillis;

    public ProtocolBufferMonitorFilter()
    {
        _threshold = DEFAULT_THRESHOLD;
        _outputFrequencyInMillis = DEFAULT_FREQUENCY;
    }

    public ProtocolBufferMonitorFilter(int threshold, long frequency)
    {
        _threshold = threshold;
        _outputFrequencyInMillis = frequency;
    }

    public void messageReceived( NextFilter nextFilter, IoSession session, Object message ) throws Exception
    {
        _bufferedMessages++;
        if (_bufferedMessages > _threshold)
        {
            long now = System.currentTimeMillis();
            if ((now - _lastMessageOutputTime) > _outputFrequencyInMillis)
            {
                _logger.warn("Protocol message buffer exceeded threshold of " + _threshold + ". Current backlog: " +
                             _bufferedMessages);
                _lastMessageOutputTime = now;
            }
        }

        nextFilter.messageReceived(session, message);
    }

    public void messageSent( NextFilter nextFilter, IoSession session, Object message ) throws Exception
    {
        _bufferedMessages--;
        nextFilter.messageSent(session, message);
    }

    public int getBufferedMessages()
    {
        return _bufferedMessages;
    }

    public int getThreshold()
    {
        return _threshold;
    }

    public void setThreshold(int threshold)
    {
        _threshold = threshold;
    }

    public long getOutputFrequencyInMillis()
    {
        return _outputFrequencyInMillis;
    }

    public void setOutputFrequencyInMillis(long outputFrequencyInMillis)
    {
        _outputFrequencyInMillis = outputFrequencyInMillis;
    }

    public long getLastMessageOutputTime()
    {
        return _lastMessageOutputTime;
    }
}
