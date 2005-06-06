package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;

import java.nio.charset.CharacterCodingException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Channel
{
    public static final class Open extends AMQCommandFrame
    {
        /* short integer */
        public int channelId;

        /* short integer */
        public int confirmTag;

        public boolean transacted;

        public boolean restartable;

        public FieldTable options;

        /* short string */
        public String outOfBand;

        public static short FRAME_TYPE = 20;

        protected long getCommandSize()
        {
            return 2 + 2 + 1 + 1 + EncodingUtils.encodedFieldTableLength(options) +
                   EncodingUtils.encodedShortStringLength(outOfBand);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, channelId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeBoolean(buffer, transacted);
            EncodingUtils.writeBoolean(buffer, restartable);
            EncodingUtils.writeFieldTableBytes(buffer, options);
            EncodingUtils.writeShortStringBytes(buffer, outOfBand);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            channelId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            transacted = EncodingUtils.readBoolean(buffer);
            restartable = EncodingUtils.readBoolean(buffer);
            options = EncodingUtils.readFieldTable(buffer);
            outOfBand = EncodingUtils.readShortString(buffer);
        }
    }

    public static final class Ack extends AMQCommandFrame
    {
        /* short integer */
        public int channelId;

        /* short integer */
        public int confirmTag;

        /* long integer */
        public long messageNbr;

        protected long getCommandSize()
        {
            return 8;
        }

        public static final short FRAME_TYPE = 21;

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, channelId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeUnsignedInteger(buffer, messageNbr);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            channelId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            messageNbr = buffer.getUnsignedInt();
        }
    }

    public static final class Commit extends AMQCommandFrame
    {
        /* short integer */
        public int channelId;

        /* short integer */
        public int confirmTag;

        public FieldTable options;

        protected long getCommandSize()
        {
            return  4 + EncodingUtils.encodedFieldTableLength(options);
        }

        public static final short FRAME_TYPE = 22;

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, channelId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeFieldTableBytes(buffer, options);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            channelId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            options = EncodingUtils.readFieldTable(buffer);
        }
    }

    public static final class Rollback extends AMQCommandFrame
    {
        /* short integer */
        public int channelId;

        /* short integer */
        public int confirmTag;

        public FieldTable options;

        protected long getCommandSize()
        {
            return  4 + EncodingUtils.encodedFieldTableLength(options);
        }

        public static short FRAME_TYPE = 23;

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, channelId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeFieldTableBytes(buffer, options);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            channelId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            options = EncodingUtils.readFieldTable(buffer);
        }
    }

    public static final class Reply extends AMQCommandFrame
    {
        /* short int */
        public int channelId;

        /* short int */
        public int confirmTag;

        /* short int */
        public int replyCode;

        /* short string*/
        public String replyText;

        public static final short FRAME_TYPE = 24;

        protected long getCommandSize()
        {
            return 6 + EncodingUtils.encodedShortStringLength(replyText);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, channelId);
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeUnsignedShort(buffer, replyCode);
            EncodingUtils.writeShortStringBytes(buffer, replyText);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            channelId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            replyCode = buffer.getUnsignedShort();
            replyText = EncodingUtils.readShortString(buffer);
        }
    }

    public static final class Close extends AMQCommandFrame
    {
        /* short integer */
        public int channelId;

        /* short integer */
        public int replyCode;

        /* short string */
        public String replyText;

        public static final short FRAME_TYPE = 25;

        protected long getCommandSize()
        {
            return 4 + EncodingUtils.encodedShortStringLength(replyText);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, channelId);
            EncodingUtils.writeUnsignedShort(buffer, replyCode);
            EncodingUtils.writeShortStringBytes(buffer, replyText);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            channelId = buffer.getUnsignedShort();
            replyCode = buffer.getUnsignedShort();
            replyText = EncodingUtils.readShortString(buffer);
        }
    }
}
