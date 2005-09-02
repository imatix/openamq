package org.openamq;

import org.apache.log4j.Logger;
import org.openamq.client.message.AbstractMessage;

/**
 * AMQ disconnected exception.
 */
public class AMQDisconnectedException extends AMQException
{
    public AMQDisconnectedException(String msg)
    {
        super(msg);
    }
}    

    
