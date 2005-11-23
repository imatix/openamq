package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.log4j.Logger;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JmsContentHeaderProperties implements ContentHeaderProperties
{
    private static final Logger _logger = Logger.getLogger(JmsContentHeaderProperties.class);

    public String contentType;

    public String encoding;

    public FieldTable headers;

    public byte deliveryMode;

    public byte priority;

    public String correlationId;

    public long expiration;

    public String replyTo;

    public String messageId;

    public long timestamp;

    public String type;

    public String userId;

    public String appId;

    public JmsContentHeaderProperties()
    {
    }

    public int getPropertyListSize()
    {
        return EncodingUtils.encodedShortStringLength(contentType) +
               EncodingUtils.encodedShortStringLength(encoding) +
               EncodingUtils.encodedFieldTableLength(headers) +
               1 + 1 +
               EncodingUtils.encodedShortStringLength(correlationId) +
               EncodingUtils.encodedShortStringLength(replyTo) +
               EncodingUtils.encodedShortStringLength(String.valueOf(expiration)) +
               EncodingUtils.encodedShortStringLength(messageId) +
               8 +
               EncodingUtils.encodedShortStringLength(type) +
               EncodingUtils.encodedShortStringLength(userId) +
               EncodingUtils.encodedShortStringLength(appId);
    }

    public int getPropertyFlags()
    {
        int value = 0;

        // for now we just blast in all properties
        for (int i = 0; i < 14; i++)
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
        buffer.put(deliveryMode);
        buffer.put(priority);
        EncodingUtils.writeShortStringBytes(buffer, correlationId);
        EncodingUtils.writeShortStringBytes(buffer, replyTo);
        EncodingUtils.writeShortStringBytes(buffer, String.valueOf(expiration));
        EncodingUtils.writeShortStringBytes(buffer, messageId);
        EncodingUtils.writeUnsignedInteger(buffer, 0/*timestamp msb*/);
        EncodingUtils.writeUnsignedInteger(buffer, timestamp);
        EncodingUtils.writeShortStringBytes(buffer, type);
        EncodingUtils.writeShortStringBytes(buffer, userId);
        EncodingUtils.writeShortStringBytes(buffer, appId);
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
            deliveryMode = buffer.get();
        if ((propertyFlags & (1 << 11)) > 0)
            priority = buffer.get();
        if ((propertyFlags & (1 << 10)) > 0)
            correlationId = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 9)) > 0)
            replyTo = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 8)) > 0)
            expiration = Long.valueOf(EncodingUtils.readShortString(buffer)).longValue();
        if ((propertyFlags & (1 << 7)) > 0)
            messageId = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 6)) > 0)
        {
            // Discard msb from AMQ timestamp
            buffer.getInt();
            timestamp = buffer.getInt();
        }    
        if ((propertyFlags & (1 << 5)) > 0)
            type = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 4)) > 0)
            userId = EncodingUtils.readShortString(buffer);
        if ((propertyFlags & (1 << 3)) > 0)
            appId = EncodingUtils.readShortString(buffer);
    }
}
