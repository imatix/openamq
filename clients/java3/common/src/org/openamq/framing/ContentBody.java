package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ContentBody extends AMQBody
{
    public static final byte TYPE = 3;

    /** TODO: consider whether this should be a pointer into the ByteBuffer to avoid copying */
    public byte[] payload;

    protected byte getType()
    {
        return TYPE;
    }

    public int getSize()
    {
        return (payload == null?0:payload.length);
    }

    public void writePayload(ByteBuffer buffer)
    {
        if (payload != null)
        {
            buffer.put(payload);
        }
    }

    protected void populateFromBuffer(ByteBuffer buffer, long size) throws AMQFrameDecodingException
    {
        if (size > 0)
        {
            payload = new byte[(int)size];
            buffer.get(payload);
        }
    }

    public static AMQFrame createAMQFrame(int channelId, ContentBody body)
    {
        final AMQFrame frame = new AMQFrame();
        frame.channel = channelId;
        frame.bodyFrame = body;
        return frame;
    }
}
