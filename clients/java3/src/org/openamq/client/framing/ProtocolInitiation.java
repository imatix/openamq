package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;

public class ProtocolInitiation extends AMQDataBlock implements EncodableAMQDataBlock
{
    public char[] header = new char[]{'D','E','M','O'};
    public byte protocolClass = 1;
    public byte protocolInstance = 1;
    public byte protocolMajor = 1;
    public byte protocolMinor = 0;

    public long getSize()
    {
        return 4 + 1 + 1 + 1 + 1;
    }

    public void writePayload(ByteBuffer buffer)
    {
        for (int i = 0; i < header.length; i++)
        {
            buffer.put((byte)header[i]);
        }
        buffer.put(protocolClass);
        buffer.put(protocolInstance);
        buffer.put(protocolMajor);
        buffer.put(protocolMinor);
    }
    
    
}
