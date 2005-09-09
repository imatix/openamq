package org.openamq;

import org.apache.log4j.Logger;

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

    
