package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQMessage extends AMQFrame
{
    /* long integer */
    public long bodySize;

    public boolean persistent;
    public byte priority;
    /* long integer */
    public long expiration;
    /* short string */
    public String mimeType;
    /* short string */
    public String encoding;
    /* short string */
    public String identifier;
    public FieldTable headers;
    public byte[] message;

    public long getSize()
    {
        return 4 + 1 + 1 + 4 + EncodingUtils.encodedShortStringLength(mimeType) +
               EncodingUtils.encodedShortStringLength(encoding) +
               EncodingUtils.encodedShortStringLength(identifier) +
               EncodingUtils.encodedFieldTableLength(headers) +
               bodySize;
    }

    public void writePayload(ByteBuffer buffer)
    {
        EncodingUtils.writeUnsignedInteger(buffer, bodySize);
        EncodingUtils.writeBooleans(buffer, new boolean[]{persistent});
        EncodingUtils.writeUnsignedByte(buffer, priority);
        EncodingUtils.writeUnsignedInteger(buffer, expiration);
        EncodingUtils.writeShortStringBytes(buffer, mimeType);
        EncodingUtils.writeShortStringBytes(buffer, encoding);
        EncodingUtils.writeShortStringBytes(buffer, identifier);
        EncodingUtils.writeFieldTableBytes(buffer, headers);
        if (message != null)
        {
            buffer.put(message);
        }
    }

    public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
    {
        bodySize = buffer.getUnsignedInt();
        persistent = EncodingUtils.readBooleans(buffer)[0];
        priority = buffer.get();
        expiration = buffer.getUnsignedInt();
        encoding = EncodingUtils.readShortString(buffer);
        mimeType = EncodingUtils.readShortString(buffer);
        identifier = EncodingUtils.readShortString(buffer);
        headers = EncodingUtils.readFieldTable(buffer);
        if (bodySize > 0)
        {
            message = new byte[(int)bodySize];
            buffer.get(message, 0, (int)bodySize);
        }
    }
}
