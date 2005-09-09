package org.openamq;

import org.apache.log4j.Logger;

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

    
