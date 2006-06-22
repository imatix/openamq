package org.openamq.framing;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;

import java.util.HashMap;
import java.util.Map;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQDataBlockDecoder
{
	Logger _logger = Logger.getLogger(AMQDataBlockDecoder.class);

    private final Map _supportedBodies = new HashMap();

    public AMQDataBlockDecoder()
    {
        _supportedBodies.put(new Byte(AMQMethodBody.TYPE), AMQMethodBodyFactory.getInstance());
        _supportedBodies.put(new Byte(ContentHeaderBody.TYPE), ContentHeaderBodyFactory.getInstance());
        _supportedBodies.put(new Byte(ContentBody.TYPE), ContentBodyFactory.getInstance());
        _supportedBodies.put(new Byte(HeartbeatBody.TYPE), new HeartbeatBodyFactory());
    }

    public boolean decodable(IoSession session, ByteBuffer in) throws AMQFrameDecodingException
    {
        // final +1 represents the command end which we know we must require even
        // if there is an empty body
        if (in.remaining() < 1)
        {
            return false;
        }
        final byte type = in.get();

        // zero, channel, body size and end byte
        if (in.remaining() < (1 + 2 + 4 + 1))
        {
            return false;
        }

        // this is just a filler value - no idea why it exists really
        final byte zero = in.get();
        final int channel = in.getUnsignedShort();
        final long bodySize = in.getUnsignedInt();

        // bodySize can be zero
        if (type <= 0 || channel < 0 || bodySize < 0)
        {
            throw new AMQFrameDecodingException("Undecodable frame: type = " + type + " channel = " + channel +
                                                " bodySize = " + bodySize);
        }

        if (in.remaining() < (bodySize + 1))
        {
            return false;
        }
        return true;
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
        if (!isSupportedFrameType(type))
        {
            throw new AMQFrameDecodingException("Unsupported frame type: " + type);
        }
        // just a filler
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
        if ((marker & 0xFF) != 0xCE)
        {
            throw new AMQFrameDecodingException("End of frame marker not found. Read " + marker + " size=" + bodySize + " type=" + type);
        }
        return frame;
    }

    public void decode(IoSession session, ByteBuffer in, ProtocolDecoderOutput out)
        throws Exception
    {
        out.write(createAndPopulateFrame(in));
    }
}
