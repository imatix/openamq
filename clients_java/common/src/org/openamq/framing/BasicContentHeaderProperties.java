package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.log4j.Logger;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicContentHeaderProperties implements ContentHeaderProperties
{
    private static final Logger _logger = Logger.getLogger(BasicContentHeaderProperties.class);

    public String contentType;

    public String encoding;

    public FieldTable headers;

    public String messageId;

    public String replyTo;

    public BasicContentHeaderProperties()
    {
    }

    public int getPropertyListSize()
    {
        return EncodingUtils.encodedShortStringLength(contentType) +
               EncodingUtils.encodedShortStringLength(encoding) +
               EncodingUtils.encodedFieldTableLength(headers) +
               EncodingUtils.encodedShortStringLength(messageId) +
               EncodingUtils.encodedShortStringLength(replyTo);
    }

    public int getPropertyFlags()
    {
        int value = 0;

        // for now we just blast in all properties
        for (int i = 0; i < 5; i++)
        {
            value += (1 << (15-i));
        }
        return value;
    }

    public void writePropertyListPayload(ByteBuffer buffer)
    {
        EncodingUtils.writeShortStringBytes(buffer, contentType);
        EncodingUtils.writeShortStringBytes(buffer, encoding);
        EncodingUtils.writeFieldTableBytes(buffer, headers);
        EncodingUtils.writeShortStringBytes(buffer, messageId);
        EncodingUtils.writeShortStringBytes(buffer, replyTo);
    }

    public void populatePropertiesFromBuffer(ByteBuffer buffer, int propertyFlags) throws AMQFrameDecodingException
    {
        _logger.debug("Property flags: " + propertyFlags);
        if ((propertyFlags & (1 << 15)) > 0)
            contentType = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 14)) > 0)
            encoding = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 13)) > 0)
            headers = EncodingUtils.readFieldTable(buffer);
        if ((propertyFlags & (1 << 12)) > 0)
            messageId = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 11)) > 0)
            replyTo = EncodingUtils.readShortString(buffer);
    }
}
