package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

public class HeartbeatBodyFactory implements BodyFactory
{
    public AMQBody createBody(ByteBuffer in) throws AMQFrameDecodingException
    {
        return new HeartbeatBody();
    }
}
