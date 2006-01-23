package org.openamq;

/**
 * AMQ channel closed exception.
 */
public class AMQConnectionClosedException extends AMQException
{
    public AMQConnectionClosedException(int errorCode, String msg)
    {
        super(errorCode, msg);
    }
}


