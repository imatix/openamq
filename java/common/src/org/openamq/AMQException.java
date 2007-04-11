package org.openamq;

import org.apache.log4j.Logger;

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
    
    public AMQException(Logger logger, String msg, Throwable t)
    {
        this(msg, t);
        logger.error(getMessage(), this);
    }
    
    public AMQException(Logger logger, String msg)
    {
        this(msg);
        logger.error(getMessage(), this);
    }
    
    public AMQException(Logger logger, int errorCode, String msg)
    {
        this(errorCode, msg);
        logger.error(getMessage(), this);
    }
    
    public int getErrorCode()
    {
        return _errorCode;
    }
}
