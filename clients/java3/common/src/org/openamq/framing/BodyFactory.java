package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

public interface BodyFactory
{
    AMQBody createBody(ByteBuffer in) throws AMQFrameDecodingException;
}
