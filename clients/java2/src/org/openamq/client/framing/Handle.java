package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;

import java.util.LinkedHashMap;

/**
 * Frames for the Handle command.
 */
public class Handle
{
    public static final class Open extends AMQCommandFrame
    {
        /* short int */
        public int channelId;

        /* short int */
        public int handleId;

        /* short int */
        public int serviceType;

        /* short int */
        public int confirmTag;

        public boolean producer;
        public boolean consumer;
        public boolean browser;
        public boolean temporary;

        /* short string */
        public String destName;

        /* short string */
        public String mimeType;

        /* short string */
        public String encoding;

        public FieldTable options;

        public static final short FRAME_TYPE = 30;

        protected long getCommandSize()
        {
            return FRAME_TYPE;
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, channelId);
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedShort(buffer, serviceType);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeBoolean(buffer, producer);
            EncodingUtils.writeBoolean(buffer, consumer);
            EncodingUtils.writeBoolean(buffer, browser);
            EncodingUtils.writeBoolean(buffer, temporary);
            EncodingUtils.writeShortStringBytes(buffer, destName);
            EncodingUtils.writeShortStringBytes(buffer, mimeType);
            EncodingUtils.writeShortStringBytes(buffer, encoding);
            EncodingUtils.writeFieldTableBytes(buffer, options);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            channelId = buffer.getUnsignedShort();
            handleId = buffer.getUnsignedShort();
            serviceType = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            producer = EncodingUtils.readBoolean(buffer);
            consumer = EncodingUtils.readBoolean(buffer);
            browser = EncodingUtils.readBoolean(buffer);
            temporary = EncodingUtils.readBoolean(buffer);
            destName = EncodingUtils.readShortString(buffer);
            mimeType = EncodingUtils.readShortString(buffer);
            encoding = EncodingUtils.readShortString(buffer);
            options = EncodingUtils.readFieldTable(buffer);
        }
    }

    public static final class Send extends AMQCommandFrame
    {
        /* short int */
        public int handleId;

        /* short int */
        public int  confirmTag;

        /* long int */
        public long  fragmentSize;

        public boolean partial;
        public boolean outOfBand;
        public boolean recovery;
        public boolean streaming;
        public String destName;

        public static final short FRAME_TYPE = 31;

        protected long getCommandSize()
        {
            return 12 + EncodingUtils.encodedShortStringLength(destName);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeUnsignedInteger(buffer, fragmentSize);
            EncodingUtils.writeBoolean(buffer, partial);
            EncodingUtils.writeBoolean(buffer, outOfBand);
            EncodingUtils.writeBoolean(buffer, recovery);
            EncodingUtils.writeBoolean(buffer, streaming);
            EncodingUtils.writeShortStringBytes(buffer, destName);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            fragmentSize = buffer.getUnsignedInt();
            partial = EncodingUtils.readBoolean(buffer);
            outOfBand = EncodingUtils.readBoolean(buffer);
            recovery =  EncodingUtils.readBoolean(buffer);
            streaming = EncodingUtils.readBoolean(buffer);
            destName = EncodingUtils.readShortString(buffer);
        }
    }

    public static final class Consume extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;

        /* short integer */
        public int confirmTag;

        /* short integer */
        public int prefetch;
        public boolean noLocal;
        public boolean noAck;

        /* short string */
        public String destName;

        /* short string */
        public String identifier;

        /* long string */
        public String selector;

        /* short string */
        public String mimeType;

        public static final short FRAME_TYPE = 32;

        protected long getCommandSize()
        {
            return 2 + 2 + 2 + 1 + 1 + EncodingUtils.encodedShortStringLength(destName) +
                   EncodingUtils.encodedShortStringLength(identifier) +
                   EncodingUtils.encodedShortStringLength(selector) +
                   EncodingUtils.encodedShortStringLength(mimeType);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeUnsignedShort(buffer, prefetch);
            EncodingUtils.writeBoolean(buffer, noLocal);
            EncodingUtils.writeBoolean(buffer, noAck);
            EncodingUtils.writeShortStringBytes(buffer, destName);
            EncodingUtils.writeShortStringBytes(buffer, identifier);
            EncodingUtils.writeLongStringBytes(buffer, selector);
            EncodingUtils.writeShortStringBytes(buffer, mimeType);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            prefetch = buffer.getUnsignedShort();
            noLocal = EncodingUtils.readBoolean(buffer);
            noAck = EncodingUtils.readBoolean(buffer);
            destName = EncodingUtils.readShortString(buffer);
            identifier = EncodingUtils.readShortString(buffer);
            selector = EncodingUtils.readLongString(buffer);
            mimeType = EncodingUtils.readShortString(buffer);
        }
    }

    public static final class Cancel extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;

        /* short integer */
        public int confirmTag;

        /* short string */
        public String destName;

        /* short string */
        public String identifier;

        public static final short FRAME_TYPE = 33;

        protected long getCommandSize()
        {
            return 2 + 2 + EncodingUtils.encodedShortStringLength(destName) +
                   EncodingUtils.encodedShortStringLength(identifier);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeShortStringBytes(buffer, destName);
            EncodingUtils.writeShortStringBytes(buffer, identifier);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag =  buffer.getUnsignedShort();
            destName = EncodingUtils.readShortString(buffer);
            identifier = EncodingUtils.readShortString(buffer);
        }
    }

    public static final class Flow extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;

        /* short integer */
        public int confirmTag;

        public boolean flowPause;

        public static final short FRAME_TYPE= 34;

        protected long getCommandSize()
        {
            return 2 + 2 + 1;
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeBoolean(buffer, flowPause);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            flowPause = EncodingUtils.readBoolean(buffer);
        }
    }

    public static final class Unget extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;

        /* short integer */
        public int confirmTag;

        /* long integer */
        public long messageNbr;

        public static final short FRAME_TYPE = 35;

        protected long getCommandSize()
        {
            return 2 + 2 + 4;
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeUnsignedInteger(buffer, messageNbr);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            messageNbr = buffer.getUnsignedInt();
        }
    }

    public static final class Query extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;

        /* long integer */
        public long messageNbr;

        /* short string */
        public String destName;

        /* long string */
        public String selector;

        /* short string */
        public String mimeType;

        public static final short FRAME_TYPE = 36;

        protected long getCommandSize()
        {
            return 2 + 4 + EncodingUtils.encodedShortStringLength(destName) +
                   EncodingUtils.encodedLongStringLength(selector) +
                   EncodingUtils.encodedShortStringLength(mimeType);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedInteger(buffer, messageNbr);
            EncodingUtils.writeShortStringBytes(buffer, destName);
            EncodingUtils.writeLongStringBytes(buffer, selector);
            EncodingUtils.writeShortStringBytes(buffer, mimeType);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            messageNbr = buffer.getUnsignedInt();
            destName = EncodingUtils.readShortString(buffer);
            selector = EncodingUtils.readLongString(buffer);
            mimeType = EncodingUtils.readShortString(buffer);
        }
    }

    public static final class Browse extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;

        /* short integer */
        public int confirmTag;

        public long messageNbr;

        public static final short FRAME_TYPE = 37;

        protected long getCommandSize()
        {
            return 2 + 2 + 4;
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeUnsignedInteger(buffer, messageNbr);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            messageNbr = buffer.getUnsignedInt();
        }
    }
}
