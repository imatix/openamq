package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.mina.protocol.codec.MessageDecoderResult;

/**
 */
public final class Connection
{
    public static final class Initiation extends AMQFrame
    {
        public byte protocolId = (byte) 128;
        public byte protocolVersion = 1;

        public long getSize()
        {
            return 2;
        }

        public void writePayload(ByteBuffer buffer)
        {
            buffer.put(protocolId);
            buffer.put(protocolVersion);
        }

        public static class Decoder extends AMQFrameDecoder
        {
            protected Object createAndPopulateFrame(ByteBuffer in)
            {
                final Initiation result = new Initiation();
                result.protocolId = in.get();
                result.protocolVersion = in.get();
                return result;
            }

            public MessageDecoderResult decodable(ProtocolSession session, ByteBuffer in)
            {
                // clients can never decode this
                return MessageDecoderResult.NOT_OK;
            }
        }
    }

    public static final class Challenge extends AMQCommandFrame
    {
        public byte version;
        public String mechanisms;
        public FieldTable challenges;

        public static short FRAME_TYPE = 10;

        public void populate(byte version, String mechanisms, FieldTable challenges)
        {
            this.version = version;
            this.mechanisms = mechanisms;
            this.challenges = challenges;
        }

        public long getCommandSize()
        {
            return 1 + EncodingUtils.encodedShortStringLength(mechanisms) +
                   EncodingUtils.encodedFieldTableLength(challenges);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            buffer.put(version);
            EncodingUtils.writeShortStringBytes(buffer, mechanisms);
            EncodingUtils.writeFieldTableBytes(buffer, challenges);
        }

        public void populateFromBuffer(ByteBuffer buffer)
                throws AMQFrameDecodingException
        {
            version = buffer.get();
            mechanisms = EncodingUtils.readShortString(buffer);
            challenges = EncodingUtils.readFieldTable(buffer);
        }
    }

    public static final class Response extends AMQCommandFrame
    {
        /** short string */
        public String mechanism;

        /** long string */
        public FieldTable responses;

        public static short FRAME_TYPE = 11;

        public void populate(String mechanism, FieldTable responses)
        {
            this.mechanism = mechanism;
            this.responses = responses;
        }

        protected long getCommandSize()
        {
            return EncodingUtils.encodedShortStringLength(mechanism) +
                   EncodingUtils.encodedFieldTableLength(responses);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeShortStringBytes(buffer, mechanism);
            EncodingUtils.writeFieldTableBytes(buffer, responses);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            mechanism = EncodingUtils.readShortString(buffer);
            responses = EncodingUtils.readFieldTable(buffer);
        }
    }

    public static final class Tune extends AMQCommandFrame
    {
        /** short string */
        public FieldTable options;

        /** long string */
        public FieldTable dictionary;

        public static short FRAME_TYPE = 12;

        public void populate(FieldTable options, FieldTable dictionary)
        {
            this.options = options;
            this.dictionary = dictionary;
        }

        protected long getCommandSize()
        {
            return EncodingUtils.encodedFieldTableLength(options) +
                   EncodingUtils.encodedFieldTableLength(dictionary);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeFieldTableBytes(buffer, options);
            EncodingUtils.writeFieldTableBytes(buffer, dictionary);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            options = EncodingUtils.readFieldTable(buffer);
            dictionary = EncodingUtils.readFieldTable(buffer);
        }
    }

    public static final class Open extends AMQCommandFrame
    {
        /** unsigned short */
        public int confirmTag;

        /** short string */
        public String virtualPath;

        /** short string */
        public String clientName;

        public FieldTable options;

        public static short FRAME_TYPE = 13;

        public void populate(int confirmTag, String virtualPath, String clientName, FieldTable options)
        {
            this.confirmTag = confirmTag;
            this.virtualPath = virtualPath;
            this.clientName = clientName;
            this.options = options;
        }

        protected long getCommandSize()
        {
            return 2 + EncodingUtils.encodedShortStringLength(virtualPath) +
                   EncodingUtils.encodedShortStringLength(clientName) +
                   EncodingUtils.encodedFieldTableLength(options);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, confirmTag);
            EncodingUtils.writeShortStringBytes(buffer, virtualPath);
            EncodingUtils.writeShortStringBytes(buffer, clientName);
            EncodingUtils.writeFieldTableBytes(buffer, options);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            confirmTag = buffer.getUnsignedShort();
            virtualPath = EncodingUtils.readShortString(buffer);
            clientName = EncodingUtils.readShortString(buffer);
            options = EncodingUtils.readFieldTable(buffer);
        }
    }

    public static final class Ping
    {
        public boolean respond;

        public void populate(boolean respond)
        {
            this.respond = respond;
        }
    }

    public static final class Reply
    {
        public short confirmTag;
        public short replyCode;
        public String replyText;

        public void populate(short confirmTag, short replyCode, String replyText)
        {
            this.confirmTag = confirmTag;
            this.replyCode = replyCode;
            this.replyText = replyText;
        }
    }

    public static final class Close extends AMQCommandFrame
    {
        public static short FRAME_TYPE = 19;

        /* short integer */
        public int replyCode;

        /* short string */
        public String replyText;

        public void populate(int replyCode, String replyText)
        {
            this.replyCode = replyCode;
            this.replyText = replyText;
        }

        protected long getCommandSize()
        {
            return 2 + EncodingUtils.encodedShortStringLength(replyText);
        }

        public short getType()
        {
            return FRAME_TYPE;
        }

        protected void writeCommandPayload(ByteBuffer buffer)
        {
            EncodingUtils.writeUnsignedShort(buffer, replyCode);
            EncodingUtils.writeShortStringBytes(buffer, replyText);
        }

        public void populateFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException
        {
            replyCode = buffer.getUnsignedShort();
            replyText = EncodingUtils.readShortString(buffer);
        }
    }
}
