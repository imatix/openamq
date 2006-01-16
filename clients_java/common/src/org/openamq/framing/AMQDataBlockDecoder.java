package org.openamq.framing;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.demux.MessageDecoder;
import org.apache.mina.filter.codec.demux.MessageDecoderResult;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;

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

    public MessageDecoderResult decodable(IoSession session, ByteBuffer in)
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
        // waiting for more data. This could be improved if MINA supported some kind of state awareness when decoding
        if ((char)type == 'A')
        {
            return MessageDecoderResult.NOT_OK;
        }
        // zero, channel, body size and end byte
        if (in.remaining() < (1 + 2 + 4 + 1))
        {
            return MessageDecoderResult.NEED_DATA;
        }

        // this is just a filler value - no idea why it exists really
        final byte zero = in.get();
        final int channel = in.getUnsignedShort();
        final long bodySize = in.getUnsignedInt();

        // bodySize can be zero
        if (type <= 0 || channel < 0 || bodySize < 0)
        {
            return MessageDecoderResult.NOT_OK;
        }

        if (in.remaining() < (bodySize + 1))
        {
            return MessageDecoderResult.NEED_DATA;
        }

        if (isSupportedFrameType(type))
        {
            if (_logger.isDebugEnabled())
            {
                // we have read 8 bytes so far, so output 8 + bodysize + 1 (for end byte) to get complete data block size
                // this logging statement is useful when looking at exactly what size of data is coming in/out
                // the broker
                _logger.debug("Able to decode data block of size " + (bodySize + 9));
            }
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
        if (bodyFactory == null)
        {
            throw new AMQFrameDecodingException("Unsupported body type: " + type);
        }
        AMQFrame frame = new AMQFrame();

        frame.populateFromBuffer(in, channel, bodySize, bodyFactory);

        byte marker = in.get();
        assert marker == 0xCE;
        return frame;
    }

    public MessageDecoderResult decode(IoSession session, ByteBuffer in, ProtocolDecoderOutput out)
        throws Exception
    {

        out.write(createAndPopulateFrame(in));

        return MessageDecoderResult.OK;
    }

    public void setDisabled(boolean b)
    {
        _disabled = b;
    }
}
