package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class ContentHeaderBody extends AMQBody
{
    public static final byte TYPE = 2;

    public int classId;

    public int weight;

    /** unsigned long but java can't handle that anyway when allocating byte array */
    public long bodySize;

    protected abstract int getPropertyListSize();

    protected byte getType()
    {
        return TYPE;
    }

    protected void populateFromBuffer(ByteBuffer buffer, long size) throws AMQFrameDecodingException
    {
        classId = buffer.getUnsignedShort();
        weight = buffer.getUnsignedShort();
        bodySize = buffer.getLong();
        int propertyFlags = buffer.getUnsignedShort();
        populatePropertiesFromBuffer(buffer, propertyFlags);
    }

    /** @return unsigned short */
    protected abstract int getPropertyFlags();

    protected abstract void writePropertyListPayload(ByteBuffer buffer);

    protected abstract void populatePropertiesFromBuffer(ByteBuffer buffer, int propertyFlags)
        throws AMQFrameDecodingException;

    public int getSize()
    {
        return 2 + 2 + 8 + 2 + getPropertyListSize();
    }

    public void writePayload(ByteBuffer buffer)
    {
        EncodingUtils.writeUnsignedShort(buffer, classId);
        EncodingUtils.writeUnsignedShort(buffer, weight);
        buffer.putLong(bodySize);
        EncodingUtils.writeUnsignedShort(buffer, getPropertyFlags());
        writePropertyListPayload(buffer);
    }

    public static AMQFrame createAMQFrame(int channelId, ContentHeaderBody body)
    {
        final AMQFrame frame = new AMQFrame();
        frame.channel = channelId;
        frame.bodyFrame = body;
        return frame;
    }
}
