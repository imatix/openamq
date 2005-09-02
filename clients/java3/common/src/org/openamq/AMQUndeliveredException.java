package org.openamq;

import org.apache.log4j.Logger;
import org.openamq.client.message.AbstractMessage;

/**
 * Generic AMQ exception.
 */
public class AMQUndeliveredException extends AMQException
{
    private AbstractMessage _bounced;
    
    public AMQUndeliveredException(int errorCode, String msg, AbstractMessage bounced)
    {
        super(errorCode, msg);
        
        _bounced = bounced;
    }
    
    public AbstractMessage getUndeliveredMessage() 
    {
        return _bounced;
    }
    
}    

    
