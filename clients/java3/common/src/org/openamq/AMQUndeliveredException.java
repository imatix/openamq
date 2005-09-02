package org.openamq;

import org.apache.log4j.Logger;

/**
 * Generic AMQ exception.
 */
public class AMQUndeliveredException extends AMQException
{
    private Object _bounced;
    
    public AMQUndeliveredException(int errorCode, String msg, Object bounced)
    {
        super(errorCode, msg);
        
        _bounced = bounced;
    }
    
    public Object getUndeliveredMessage() 
    {
        return _bounced;
    }
    
}    

    
