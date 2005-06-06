package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.ProtocolDecoderOutput;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.mina.protocol.ProtocolViolationException;
import org.apache.mina.protocol.codec.MessageDecoder;
import org.apache.mina.protocol.codec.MessageDecoderResult;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AMQFrameDecoder implements MessageDecoder
{
    /**
     * Subclasses must override this method to read all necessary data from the
     * buffer and create a frame instance.
     * @param in the buffer
     * @return a new frame instance
     */
    protected abstract Object createAndPopulateFrame(ByteBuffer in)
            throws AMQFrameDecodingException;

    public MessageDecoderResult decode(ProtocolSession session, ByteBuffer in, ProtocolDecoderOutput out)
            throws ProtocolViolationException
    {
        try
        {
            out.write(createAndPopulateFrame(in));
        }
        catch (AMQFrameDecodingException e)
        {
            throw new ProtocolViolationException(e);
        }
        return MessageDecoderResult.OK;
    }
}
