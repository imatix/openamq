package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

public class HeartbeatBody extends AMQBody
{
    public static final byte TYPE = 8;
    public static AMQFrame FRAME = new HeartbeatBody().toFrame();

    protected byte getType()
    {
        return TYPE;
    }

    protected int getSize()
    {
        return 0;//heartbeats we generate have no payload
    }

    protected void writePayload(ByteBuffer buffer)
    {
    }

    protected void populateFromBuffer(ByteBuffer buffer, long size) throws AMQFrameDecodingException
    {
        if(size > 0)
        {
            //allow other implementations to have a payload, but ignore it:
            buffer.skip((int) size);
        }
    }

    public AMQFrame toFrame()
    {
        return new AMQFrame(0, this);
    }
}
