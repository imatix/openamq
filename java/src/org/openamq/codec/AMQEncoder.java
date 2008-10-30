package org.openamq.codec;

import org.apache.mina.filter.codec.ProtocolEncoder;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;
import org.apache.mina.common.IoSession;
import org.openamq.framing.AMQDataBlockEncoder;

public class AMQEncoder implements ProtocolEncoder
{
    private AMQDataBlockEncoder _dataBlockEncoder = new AMQDataBlockEncoder();

    public void encode(IoSession session, Object message, ProtocolEncoderOutput out) throws Exception
    {
        _dataBlockEncoder.encode(session, message, out);
    }

    public void dispose(IoSession session) throws Exception
    {

    }
}
