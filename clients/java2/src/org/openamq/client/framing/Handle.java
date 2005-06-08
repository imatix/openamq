package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.codec.MessageDecoderResult;
import org.apache.mina.protocol.ProtocolSession;

/**
 * Frames for the Handle command.
 */
public class Handle
{
    public static final class Open extends AMQCommandFrame
    {
        public static final int SERVICE_TYPE_QUEUE = 1;

        public static final int SERVICE_TYPE_TOPIC = 2;

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

        /* short string */
        public String mimeType;

        /* short string */
        public String encoding;

        public FieldTable options;

        public static final short FRAME_TYPE = 30;
        private static int _handleNotifyDecodes;

        protected long getCommandSize()
        {
            return 2 + 2 + 2 + 2 + 1 +
                   EncodingUtils.encodedShortStringLength(mimeType) +
                   EncodingUtils.encodedShortStringLength(encoding) +
                   EncodingUtils.encodedFieldTableLength(options);
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
            EncodingUtils.writeBooleans(buffer, new boolean[]{producer, consumer, browser});
            EncodingUtils.writeShortStringBytes(buffer, mimeType);
            EncodingUtils.writeShortStringBytes(buffer, encoding);
            EncodingUtils.writeFieldTableBytes(buffer, options);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            _handleNotifyDecodes++;
            if (_handleNotifyDecodes%1000==0)
            {
                System.out.println("Received " + _handleNotifyDecodes + " frames");
            }
            channelId = buffer.getUnsignedShort();
            handleId = buffer.getUnsignedShort();
            serviceType = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            boolean[] bools = EncodingUtils.readBooleans(buffer);
            producer = bools[0];
            consumer = bools[1];
            browser = bools[2];                        
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
        public AMQMessage message;

        public static final short FRAME_TYPE = 31;

         public void writePayload(ByteBuffer buffer)
        {
            // size is payload + 1 for type + 1 byte for the end of frame marker
            final long size = getCommandSize() + 2;
            if (size > 0xFFFE)
            {
                EncodingUtils.writeUnsignedShort(buffer, 0xFFFF);
                EncodingUtils.writeUnsignedInteger(buffer, size);
            }
            else
            {
                EncodingUtils.writeUnsignedShort(buffer, (int) size);
            }

            EncodingUtils.writeUnsignedByte(buffer, getType());
            writeCommandPayload(buffer);
            // write the end of command frame marker
            buffer.put((byte)0xCE);
            if (message != null)
            {
                message.writePayload(buffer);
            }
        }

        protected long getCommandSize()
        {
            return 2 + 2 + 4 + 1 + EncodingUtils.encodedShortStringLength(destName);
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
            EncodingUtils.writeBooleans(buffer, new boolean[]{partial, outOfBand, recovery,
                                                              streaming});
            EncodingUtils.writeShortStringBytes(buffer, destName);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            fragmentSize = buffer.getUnsignedInt();
            boolean[] bools = EncodingUtils.readBooleans(buffer);
            partial = bools[0];
            outOfBand = bools[1];
            recovery =  bools[2];
            streaming = bools[3];
            destName = EncodingUtils.readShortString(buffer);
            message= new AMQMessage();
            message.populateFromBuffer(buffer);
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

        public boolean dynamic;
        public boolean exclusive;

        /* short string */
        public String destName;

        public FieldTable selector;

        public static final short FRAME_TYPE = 32;

        protected long getCommandSize()
        {
            return 2 + 2 + 2 + 1 + EncodingUtils.encodedShortStringLength(destName) +
                   EncodingUtils.encodedFieldTableLength(selector);
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
            EncodingUtils.writeBooleans(buffer, new boolean[] {noLocal,noAck,dynamic,exclusive});
            EncodingUtils.writeShortStringBytes(buffer, destName);
            EncodingUtils.writeFieldTableBytes(buffer, selector);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            prefetch = buffer.getUnsignedShort();

            boolean[] bools = EncodingUtils.readBooleans(buffer);

            noLocal = bools[0];
            noAck = bools[1];
            dynamic = bools[2];
            exclusive = bools[3];

            destName = EncodingUtils.readShortString(buffer);
            selector = EncodingUtils.readFieldTable(buffer);
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
            EncodingUtils.writeBooleans(buffer, new boolean[]{flowPause});
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            confirmTag = buffer.getUnsignedShort();
            flowPause = EncodingUtils.readBooleans(buffer)[0];
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

    public static final class Created extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;
        /* short string */
        public String destName;

        public static final short FRAME_TYPE = 40;
        protected long getCommandSize()
        {
            return 2 + EncodingUtils.encodedShortStringLength(destName);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeShortStringBytes(buffer, destName);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            destName = EncodingUtils.readShortString(buffer);
        }
    }

    public static final class Notify extends AMQCommandFrame
    {
        /* short integer */
        public int handleId;
        /* long integer */
        public long messageNbr;
        /* long integer */
        public long fragmentSize;
        public boolean partial;
        public boolean outOfBand;
        public boolean recovery;
        public boolean delivered;
        public boolean redelivered;
        /* short string */
        public String destName;
        public AMQMessage messageFragment;

        public static final short FRAME_TYPE = 41;

        protected long getCommandSize()
        {
            return 2 + 4 + 4 + 1 + EncodingUtils.encodedShortStringLength(destName);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, handleId);
            EncodingUtils.writeUnsignedInteger(buffer, messageNbr);
            EncodingUtils.writeUnsignedInteger(buffer, fragmentSize);
            EncodingUtils.writeBooleans(buffer, new boolean[]{partial, outOfBand, recovery, delivered, redelivered});
            EncodingUtils.writeShortStringBytes(buffer, destName);
            messageFragment.writePayload(buffer);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            handleId = buffer.getUnsignedShort();
            messageNbr = buffer.getUnsignedInt();
            fragmentSize = buffer.getUnsignedInt();
            boolean[] bools = EncodingUtils.readBooleans(buffer);
            partial = bools[0];
            outOfBand = bools[1];
            recovery = bools[2];
            delivered = bools[3];
            redelivered = bools[4];
            destName = EncodingUtils.readShortString(buffer);
            byte endOfFrameMarker = buffer.get();
            if (endOfFrameMarker != (byte)0xCE)
            {
                throw new AMQFrameDecodingException("Unexpected value found for end of frame marker: " + endOfFrameMarker);
            }
            messageFragment = new AMQMessage();
            messageFragment.populateFromBuffer(buffer);
        }

        public static final class Decoder extends AMQCommandFrameDecoder
        {
            public MessageDecoderResult decodable(ProtocolSession session, ByteBuffer in)
            {
                final long frameSize = readFrameSize(in);
                final int totalRemaining = in.remaining();
                if (frameSize < 0)
                {
                    return MessageDecoderResult.NOT_OK;
                }

                if (frameSize == 0 || in.remaining() < frameSize)
                {
                    return MessageDecoderResult.NEED_DATA;
                }

                short frameType = in.getUnsigned();

                if (frameType == FRAME_TYPE)
                {
                    int handleId = in.getUnsignedShort();
                    long messageNbr = in.getUnsignedInt();
                    long fragmentSize = in.getUnsignedInt();
                    if (totalRemaining < frameSize + fragmentSize)
                    {
                        return MessageDecoderResult.NEED_DATA;
                    }
                    else
                    {
                        return MessageDecoderResult.OK;
                    }
                }
                /*else if (size < frameSize)
                {
                    return MessageDecoderResult.NEED_DATA;
                }*/
                else
                {
                    return MessageDecoderResult.NOT_OK;
                }
            }

            protected Object createAndPopulateFrame(ByteBuffer in)
                    throws AMQFrameDecodingException
            {
                long frameSize = readFrameSize(in);

                short frameType = in.getUnsigned();
                Handle.Notify frame = new Handle.Notify();

                frame.populateFromBuffer(in);

                return frame;
            }
        }
    }

    public static final class Reply extends AMQCommandFrame
        {
            /* short int */
            public int handleId;

            /* short int */
            public int confirmTag;

            /* short int */
            public int replyCode;

            /* short string*/
            public String replyText;

            public static final short FRAME_TYPE = 48;

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
                EncodingUtils.writeUnsignedShort(buffer, handleId);
                EncodingUtils.writeUnsignedShort(buffer, confirmTag);
                EncodingUtils.writeUnsignedShort(buffer, replyCode);
                EncodingUtils.writeShortStringBytes(buffer, replyText);
            }

            public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
            {
                handleId = buffer.getUnsignedShort();
                confirmTag = buffer.getUnsignedShort();
                replyCode = buffer.getUnsignedShort();
                replyText = EncodingUtils.readShortString(buffer);
            }
        }

}
