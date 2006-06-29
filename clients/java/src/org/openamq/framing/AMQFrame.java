package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

public class AMQFrame extends AMQDataBlock implements EncodableAMQDataBlock
{
    public int channel;

    public AMQBody bodyFrame;

    public AMQFrame()
    {
    }

    public AMQFrame(int channel, AMQBody bodyFrame)
    {
        this.channel = channel;
        this.bodyFrame = bodyFrame;
    }

    public long getSize()
    {
        return 1 + 1 + 2 + 4 + bodyFrame.getSize() + 1;
    }

    public void writePayload(ByteBuffer buffer)
    {
        buffer.put(bodyFrame.getType());
        // this is a filler
        buffer.put((byte)0);
        // TODO: how does channel get populated
        EncodingUtils.writeUnsignedShort(buffer, channel);
        EncodingUtils.writeUnsignedInteger(buffer, bodyFrame.getSize());
        bodyFrame.writePayload(buffer);
        buffer.put((byte) 0xCE);
    }

    /**
     *
     * @param buffer
     * @param channel unsigned short
     * @param bodySize unsigned integer
     * @param bodyFactory
     * @throws AMQFrameDecodingException
     */
    public void populateFromBuffer(ByteBuffer buffer, int channel, long bodySize, BodyFactory bodyFactory)
            throws AMQFrameDecodingException
    {
        this.channel = channel;
        bodyFrame = bodyFactory.createBody(buffer);
        bodyFrame.populateFromBuffer(buffer, bodySize);
    }

    public String toString()
    {
        return "Frame channelId: " + channel + ", bodyFrame: " + String.valueOf(bodyFrame);
    }
}
