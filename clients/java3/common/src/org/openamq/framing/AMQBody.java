package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AMQBody
{
    protected abstract byte getType();
    
    /** 
     * Get the size of the body
     * @return unsigned short
     */
    protected abstract int getSize();
    
    protected abstract void writePayload(ByteBuffer buffer);
    
    protected abstract void populateFromBuffer(ByteBuffer buffer, long size) throws AMQFrameDecodingException;        
}
