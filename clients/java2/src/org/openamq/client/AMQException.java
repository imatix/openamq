package org.openamq.client;

/**
 * Generic AMQ exception.
 */
public class AMQException extends Exception
{
    public AMQException(String message)
    {
        super(message);
    }

    public AMQException(String msg, Throwable t)
    {
        super(msg, t);
    }
}
