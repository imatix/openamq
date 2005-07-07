package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.ProtocolDecoderOutput;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.mina.protocol.ProtocolViolationException;
import org.apache.mina.protocol.codec.MessageDecoder;
import org.apache.mina.protocol.codec.MessageDecoderResult;

import java.util.Map;
import java.util.HashMap;

import org.apache.log4j.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQDataBlockDecoder implements MessageDecoder
{
	Logger _logger = Logger.getLogger(AMQDataBlockDecoder.class);

    private final Map _supportedBodies = new HashMap();

    public AMQDataBlockDecoder()
    {
        _supportedBodies.put(new Short(AMQMethodBody.TYPE), AMQMethodBodyFactory.getInstance());        
    }

    public MessageDecoderResult decodable(ProtocolSession session, ByteBuffer in)
    {
        // final +1 represents the command end which we know we must require even
        // if there is an empty body
        if (in.remaining() < (1 + 1 + 2 + 1))
        {
            return MessageDecoderResult.NEED_DATA;
        }
        final byte type = in.get();
        final byte channel = in.get();
        final int bodySize = in.getUnsignedShort();

        // bodySize is always at least one since command end is included in body size
        if (type <= 0 || channel < 0 || bodySize <= 0)
        {
            return MessageDecoderResult.NOT_OK;
        }

        if (in.remaining() < bodySize)
        {
            return MessageDecoderResult.NEED_DATA;
        }
        
        if (isSupportedFrameType(type))
        {
            return MessageDecoderResult.OK;
        }
        /*else if (size < frameSize)
        {
            return MessageDecoderResult.NEED_DATA;
        }*/
        else
        {
            return MessageDecoderResult.NOT_OK;
        }
    }

    private boolean isSupportedFrameType(short frameType)
    {
        final boolean result = _supportedBodies.containsKey(new Short(frameType));

        if (!result)
        {
        	_logger.warn("AMQCommandFrameDecoder does not handle frame type " + frameType);
        }

        return result;
    }
    
    protected Object createAndPopulateFrame(ByteBuffer in)
                    throws AMQFrameDecodingException
    {
        final byte type = in.get();
        final byte channel = in.get();
        final int bodySize = in.getUnsignedShort();

        BodyFactory bodyFactory = (BodyFactory) _supportedBodies.get(new Short(type));
        AMQFrame frame = new AMQFrame();
        
        frame.populateFromBuffer(in, channel, bodySize, bodyFactory);                       
        
        byte marker = in.get();
        assert marker == 0xCE;
        return frame;
    }

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
