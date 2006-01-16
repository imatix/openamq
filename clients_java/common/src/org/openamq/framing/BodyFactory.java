package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * Any class that is capable of turning a stream of bytes into an AMQ structure must implement this interface.
 */
public interface BodyFactory
{
    AMQBody createBody(ByteBuffer in) throws AMQFrameDecodingException;
}
