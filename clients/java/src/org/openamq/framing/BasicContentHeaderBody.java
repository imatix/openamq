package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicContentHeaderBody extends AMQBody
{
    public static final byte TYPE = 2;

    public int classId;

    public int weight;

    /** unsigned long but java can't handle that anyway when allocating byte array */
    public long bodySize;

    /** must never be null */
    public BasicContentHeaderProperties properties;

    public BasicContentHeaderBody()
    {
    }

    public BasicContentHeaderBody(BasicContentHeaderProperties props, int classId)
    {
        properties = props;
        this.classId = classId;
    }

    public BasicContentHeaderBody(int classId, int weight, BasicContentHeaderProperties props, long bodySize)
    {
        this(props, classId);
        this.weight = weight;
        this.bodySize = bodySize;
    }

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
        properties = new BasicContentHeaderProperties();
        properties.populatePropertiesFromBuffer(buffer, propertyFlags, (int)size - 14);
    }

    public int getSize()
    {
        return 2 + 2 + 8 + 2 + properties.getPropertyListSize();
    }

    public void writePayload(ByteBuffer buffer)
    {
        EncodingUtils.writeUnsignedShort(buffer, classId);
        EncodingUtils.writeUnsignedShort(buffer, weight);
        buffer.putLong(bodySize);
        EncodingUtils.writeUnsignedShort(buffer, properties.getPropertyFlags());
        properties.writePropertyListPayload(buffer);
    }

    public static AMQFrame createAMQFrame(int channelId, int classId, int weight, BasicContentHeaderProperties properties,
                                          long bodySize)
    {
        final AMQFrame frame = new AMQFrame();
        frame.channel = channelId;
        frame.bodyFrame = new ContentHeaderBody(classId, weight, properties, bodySize);
        return frame;
    }

    public static AMQFrame createAMQFrame(int channelId, ContentHeaderBody body)
    {
        final AMQFrame frame = new AMQFrame();
        frame.channel = channelId;
        frame.bodyFrame = body;
        return frame;
    }
}
