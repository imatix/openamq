package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;
import org.openamq.AMQException;

public class ProtocolInitiation extends AMQDataBlock implements EncodableAMQDataBlock
{
    public char[] header = new char[]{'A','M','Q','P'};
    // TODO: generate these constants automatically from the xml protocol spec file
    public byte protocolClass = 1;
    public byte protocolInstance = 1;
    public byte protocolMajor = 7;
    public byte protocolMinor = 0;

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

    public boolean equals(Object o)
    {
        if (!(o instanceof ProtocolInitiation))
        {
            return false;
        }

        ProtocolInitiation pi = (ProtocolInitiation) o;
        if (pi.header == null)
        {
            return false;
        }

        if (header.length != pi.header.length)
        {
            return false;
        }

        for (int i = 0; i < header.length; i++)
        {
            if (header[i] != pi.header[i])
            {
                return false;
            }
        }

        return (protocolClass == pi.protocolClass &&
                protocolInstance == pi.protocolInstance &&
                protocolMajor == pi.protocolMajor &&
                protocolMinor == pi.protocolMinor);
    }

    public static class Decoder //implements MessageDecoder
    {
        /**
         *
         * @param session
         * @param in
         * @return true if we have enough data to decode the PI frame fully, false if more
         * data is required
         */
        public boolean decodable(IoSession session, ByteBuffer in)
        {
            return (in.remaining() >= 8);
        }

        public void decode(IoSession session, ByteBuffer in, ProtocolDecoderOutput out)
            throws Exception
        {
            byte[] theHeader = new byte[4];
            in.get(theHeader);
            ProtocolInitiation pi = new ProtocolInitiation();
            pi.header = new char[]{(char) theHeader[0],(char) theHeader[1],(char) theHeader[2], (char) theHeader[3]};
            String stringHeader = new String(pi.header);
            if (!"AMQP".equals(stringHeader))
            {
                throw new AMQProtocolHeaderException("Invalid protocol header - read " + stringHeader);
            }
            byte temp = pi.protocolClass;
            pi.protocolClass = in.get();
            if (pi.protocolClass != temp)
            {
                throw new AMQProtocolClassException("Protocol class " + temp + " was expected; received " +
                                                      pi.protocolClass);
            }
            temp = pi.protocolInstance;
            pi.protocolInstance = in.get();
            if (pi.protocolInstance != temp)
            {
                throw new AMQProtocolInstanceException("Protocol instance " + temp + " was expected; received " +
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
        }
    }
}
