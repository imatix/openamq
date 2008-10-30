package org.openamq;

/**
 * AMQ channel closed exception.
 */
public class AMQChannelClosedException extends AMQException
{
    public AMQChannelClosedException(int errorCode, String msg)
    {
        super(errorCode, msg);
    }
}


