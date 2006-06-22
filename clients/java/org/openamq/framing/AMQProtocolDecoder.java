package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.CumulativeProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;
import org.openamq.framing.AMQDataBlockDecoder;
import org.openamq.framing.ProtocolInitiation;

public class AMQProtocolDecoder extends CumulativeProtocolDecoder
{
    AMQDataBlockDecoder
        dbd = new AMQDataBlockDecoder();

    protected boolean doDecode(IoSession session, ByteBuffer in, ProtocolDecoderOutput pdo) throws Exception
    {
        int
            pos = in.position();
        boolean
            decodable = dbd.decodable(session, in);

        in.position(pos);
        if (decodable)
            dbd.decode(session, in, pdo);

        return decodable;
    }
}
