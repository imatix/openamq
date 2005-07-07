package org.openamq.client.framing;

import java.util.HashMap;
import java.util.Map;

import org.apache.mina.common.ByteBuffer;
import org.openamq.client.AMQException;

public class AMQFrame extends AMQDataBlock implements EncodableAMQDataBlock
{    
    public short channel;        
    
    public AMQBody bodyFrame;
    
    public long getSize()
    {
        return 1 + 2 + 2 + bodyFrame.getSize() + 1;
    }

    public void writePayload(ByteBuffer buffer)
    {
        buffer.put(bodyFrame.getType());
        // TODO: how does channel get populated
        EncodingUtils.writeUnsignedByte(buffer, channel);        
        EncodingUtils.writeUnsignedShort(buffer, bodyFrame.getSize() + 1);
        bodyFrame.writePayload(buffer);
        buffer.put((byte)0xCE);
    }

    public void populateFromBuffer(ByteBuffer buffer, byte channel, int bodySize, BodyFactory bodyFactory) throws AMQFrameDecodingException
    {
        this.channel = channel;        
        bodyFrame = bodyFactory.createBody(buffer);
        bodyFrame.populateFromBuffer(buffer);        
    }
    
    public String toString()
    {
        return "Frame channelId: " + channel + ", bodyFrame: " + String.valueOf(bodyFrame);
    }
}
