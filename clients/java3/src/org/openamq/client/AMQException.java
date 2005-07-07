package org.openamq.client;

/**
 * Generic AMQ exception.
 */
public class AMQException extends Exception
{
    private int _errorCode;

    public AMQException(String message)
    {
        super(message);
    }

    public AMQException(String msg, Throwable t)
    {
        super(msg, t);
    }

    public AMQException(int errorCode, String msg, Throwable t)
    {
        super(msg + " [error code " + errorCode + ']', t);
        _errorCode = errorCode;
    }

    public AMQException(int errorCode, String msg)
    {
        super(msg + " [error code " + errorCode + ']');
        _errorCode = errorCode;
    }
    
    public int getErrorCode()
    {
        return _errorCode;
    }
}
