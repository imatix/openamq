package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.codec.MessageDecoder;
import org.apache.mina.protocol.codec.MessageDecoderResult;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.mina.protocol.ProtocolDecoderOutput;
import org.apache.mina.protocol.ProtocolViolationException;
import org.openamq.AMQException;

public class ProtocolInitiation extends AMQDataBlock implements EncodableAMQDataBlock
{
    public char[] header = new char[]{'A','M','Q','P'};
    public byte protocolClass = 1;
    public byte protocolInstance = 1;
    public byte protocolMajor = 10;
    public byte protocolMinor = 2;

    public long getSize()
    {
        return 4 + 1 + 1 + 1 + 1;
    }

    public void writePayload(ByteBuffer buffer)
    {
        for (int i = 0; i < header.length; i++)
        {
            buffer.put((byte) header[i]);
        }
        buffer.put(protocolClass);
        buffer.put(protocolInstance);
        buffer.put(protocolMajor);
        buffer.put(protocolMinor);
    }

    public void populateFromBuffer(ByteBuffer buffer) throws AMQException
    {
        throw new AMQException("Method not implemented");
    }

    public static class Decoder implements MessageDecoder
    {
        private boolean _disabled = false;

        public MessageDecoderResult decodable(ProtocolSession session, ByteBuffer in)
        {
            if (_disabled)
            {
                return MessageDecoderResult.NOT_OK;
            }
            if (in.remaining() < 8)
            {
                return MessageDecoderResult.NEED_DATA;
            }
            else
            {
                char[] expected = new char[]{'A', 'M', 'Q', 'P'};
                for (int i = 0; i < 4; i++)
                {
                    if (((char) in.get()) != expected[i])
                    {
                        return MessageDecoderResult.NOT_OK;
                    }
                }
                return MessageDecoderResult.OK;
            }
        }

        public MessageDecoderResult decode(ProtocolSession session, ByteBuffer in, ProtocolDecoderOutput out)
            throws ProtocolViolationException
        {
            byte[] header = new byte[4];
            in.get(header);
            ProtocolInitiation pi = new ProtocolInitiation();
            pi.header = new char[]{'A','M','Q','P'};
            pi.protocolClass = in.get();
            pi.protocolInstance = in.get();
            pi.protocolMajor = in.get();
            pi.protocolMinor = in.get();
            out.write(pi);
            return MessageDecoderResult.OK;
        }

        public void setDisabled(boolean disabled)
        {
            _disabled = disabled;
        }
    }
}
