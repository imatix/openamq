package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;

public abstract class AMQMethodBody extends AMQBody
{
    public static final byte TYPE = 1;             
    
    public short flags;
    
    public short reserved;
    
    public long synchtag;
    
    /** unsigned short */
    protected abstract int getBodySize();
    
    protected abstract byte getClazz();
    
    protected abstract byte getMethod();
    
    protected abstract void writeMethodPayload(ByteBuffer buffer);
    
    protected byte getType()
    {
        return TYPE;
    }

    protected int getSize()
    { 
        return 1 + 1 + 1 + 1 + getBodySize();
    }
    
    protected void writePayload(ByteBuffer buffer)
    {
        buffer.put(getClazz());
        buffer.put(getMethod());
        EncodingUtils.writeUnsignedByte(buffer, flags);        
        EncodingUtils.writeUnsignedByte(buffer, reserved);                
        writeMethodPayload(buffer);
    }

    protected abstract void populateMethodBodyFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException;
    
    protected void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
    {
        flags = buffer.getUnsigned();
        reserved = buffer.getUnsigned();        
        populateMethodBodyFromBuffer(buffer);
    }
    
    public String toString()
    {
        StringBuffer buf = new StringBuffer(getClass().toString());
        buf.append("\n\tClass: ").append(getClazz());
        buf.append("\n\tMethod: ").append(getMethod());
        buf.append("\n\tFlags: ").append(flags);        
        buf.append("\n\tReserved: ").append(reserved);                
        return buf.toString();
    }
}
