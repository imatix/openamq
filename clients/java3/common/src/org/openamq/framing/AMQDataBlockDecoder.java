package org.openamq.framing;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.ProtocolDecoderOutput;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.mina.protocol.ProtocolViolationException;
import org.apache.mina.protocol.codec.MessageDecoder;
import org.apache.mina.protocol.codec.MessageDecoderResult;

import java.util.HashMap;
import java.util.Map;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQDataBlockDecoder implements MessageDecoder
{
	Logger _logger = Logger.getLogger(AMQDataBlockDecoder.class);

    private final Map _supportedBodies = new HashMap();

    private boolean _disabled = false;

    public AMQDataBlockDecoder()
    {
        _supportedBodies.put(new Byte(AMQMethodBody.TYPE), AMQMethodBodyFactory.getInstance());
        _supportedBodies.put(new Byte(ContentHeaderBody.TYPE), ContentHeaderBodyFactory.getInstance());
        _supportedBodies.put(new Byte(ContentBody.TYPE), ContentBodyFactory.getInstance());
    }

    public MessageDecoderResult decodable(ProtocolSession session, ByteBuffer in)
    {
        if (_disabled)
        {
            return MessageDecoderResult.NOT_OK;
        }
        // final +1 represents the command end which we know we must require even
        // if there is an empty body
        if (in.remaining() < 1)
        {
            return MessageDecoderResult.NEED_DATA;
        }
        final byte type = in.get();

        // we have to check this isn't a protocol initiation frame here - we can't tell later on and we end up
        // waiting for more data
        if ((char)type == 'A')
        {
            return MessageDecoderResult.NOT_OK;
        }
        if (in.remaining() < (1 + 2 + 4))
        {
            return MessageDecoderResult.NEED_DATA;
        }

        // this is just a filler value - no idea why it exists really
        final byte zero = in.get();
        final int channel = in.getUnsignedShort();
        final long bodySize = in.getUnsignedInt();

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

    private boolean isSupportedFrameType(byte frameType)
    {
        final boolean result = _supportedBodies.containsKey(new Byte(frameType));

        if (!result)
        {
        	_logger.warn("AMQDataBlockDecoder does not handle frame type " + frameType);
        }

        return result;
    }
    
    protected Object createAndPopulateFrame(ByteBuffer in)
                    throws AMQFrameDecodingException
    {
        final byte type = in.get();
        final byte zero = in.get();
        final int channel = in.getUnsignedShort();
        final long bodySize = in.getUnsignedInt();

        BodyFactory bodyFactory = (BodyFactory) _supportedBodies.get(new Byte(type));
        AMQFrame frame = new AMQFrame();

        // Note that bodySize here includes the end of frame marker
        frame.populateFromBuffer(in, channel, bodySize - 1, bodyFactory);
        
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

    public void setDisabled(boolean b)
    {
        _disabled = b;
    }
}
