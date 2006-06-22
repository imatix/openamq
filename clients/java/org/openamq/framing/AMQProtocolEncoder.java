package org.openamq.framing;

import org.apache.mina.filter.codec.ProtocolEncoder;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;
import org.apache.mina.common.IoSession;
import org.openamq.framing.AMQDataBlockEncoder;

public class AMQProtocolEncoder implements ProtocolEncoder
{
    AMQDataBlockEncoder
        dbe = new AMQDataBlockEncoder();

    public void encode(IoSession session, Object message, ProtocolEncoderOutput peo) throws Exception
    {
        dbe.encode(session, message, peo);
    }

    public void dispose(IoSession session) throws Exception
    {
    }
}
