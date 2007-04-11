package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class HeartbeatBodyFactory implements BodyFactory
{
    public AMQBody createBody(ByteBuffer in) throws AMQFrameDecodingException
    {
        return new HeartbeatBody();
    }
}
