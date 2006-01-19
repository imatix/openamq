package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.demux.MessageDecoderResult;
import org.apache.mina.filter.codec.demux.MessageDecoder;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;
import org.openamq.AMQException;

public class ProtocolInitiation extends AMQDataBlock implements EncodableAMQDataBlock
{
    public char[] header = new char[]{'A','M','Q','P'};
    // TODO: generate these constants automatically from the xml protocol spec file
    public byte protocolClass = 1;
    public byte protocolInstance = 1;
    public byte protocolMajor = 10;
    public byte protocolMinor = 3;

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

        public MessageDecoderResult decodable(IoSession session, ByteBuffer in)
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

        public MessageDecoderResult decode(IoSession session, ByteBuffer in, ProtocolDecoderOutput out)
            throws Exception
        {
            byte[] header = new byte[4];
            in.get(header);
            ProtocolInitiation pi = new ProtocolInitiation();
            pi.header = new char[]{'A','M','Q','P'};
            byte temp = pi.protocolClass;
            pi.protocolClass = in.get();
            if (pi.protocolClass != temp)
            {
                throw new AMQProtocolVersionException("Protocol class " + temp + " was expected; received " +
                                                      pi.protocolClass);
            }
            temp = pi.protocolInstance;
            pi.protocolInstance = in.get();
            if (pi.protocolInstance != temp)
            {
                throw new AMQProtocolVersionException("Protocol instance " + temp + " was expected; received " +
                                                      pi.protocolInstance);
            }
            temp = pi.protocolMajor;
            pi.protocolMajor = in.get();
            if (pi.protocolMajor != temp)
            {
                throw new AMQProtocolVersionException("Protocol major version " + temp + " was expected; received " +
                                                      pi.protocolMajor);
            }
            temp = pi.protocolMinor;
            pi.protocolMinor = in.get();
            if (pi.protocolMinor != temp)
            {
                throw new AMQProtocolVersionException("Protocol minor version " + temp + " was expected; received " +
                                                      pi.protocolMinor);
            }
            out.write(pi);
            return MessageDecoderResult.OK;
        }

        public void setDisabled(boolean disabled)
        {
            _disabled = disabled;
        }
    }
}
