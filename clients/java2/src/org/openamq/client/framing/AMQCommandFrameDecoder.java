package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.mina.protocol.codec.MessageDecoderResult;

import java.util.Map;
import java.util.HashSet;
import java.util.Set;
import java.util.HashMap;

import org.apache.log4j.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQCommandFrameDecoder extends AMQFrameDecoder
{
	Logger _logger = Logger.getLogger(AMQCommandFrameDecoder.class);

    /**
     * Marker indicating the frame size is an unsigned long. In the absence
     * of this marker the frame size is an unsigned integer.
     */
    private static final int FRAME_LONG = 0xFFFF;

    private final Map _supportedFrames = new HashMap();

    public AMQCommandFrameDecoder()
    {
        //
        // Connection frames
        //
        _supportedFrames.put(new Short(Connection.Challenge.FRAME_TYPE), Connection.Challenge.class);
        _supportedFrames.put(new Short(Connection.Tune.FRAME_TYPE), Connection.Tune.class);
        _supportedFrames.put(new Short(Connection.Response.FRAME_TYPE), Connection.Response.class);
        _supportedFrames.put(new Short(Connection.Close.FRAME_TYPE), Connection.Close.class);

        //
        // Channel frames
        //
        _supportedFrames.put(new Short(Channel.Ack.FRAME_TYPE), Channel.Ack.class);
        _supportedFrames.put(new Short(Channel.Close.FRAME_TYPE), Channel.Close.class);
        _supportedFrames.put(new Short(Channel.Commit.FRAME_TYPE), Channel.Commit.class);
        _supportedFrames.put(new Short(Channel.Open.FRAME_TYPE), Channel.Open.class);
        _supportedFrames.put(new Short(Channel.Reply.FRAME_TYPE), Channel.Reply.class);
        _supportedFrames.put(new Short(Channel.Rollback.FRAME_TYPE), Channel.Rollback.class);

        //
        // Handle frames
        //
        _supportedFrames.put(new Short(Handle.Browse.FRAME_TYPE), Handle.Browse.class);
        _supportedFrames.put(new Short(Handle.Cancel.FRAME_TYPE), Handle.Cancel.class);
        _supportedFrames.put(new Short(Handle.Consume.FRAME_TYPE), Handle.Consume.class);
        _supportedFrames.put(new Short(Handle.Created.FRAME_TYPE), Handle.Created.class);
        _supportedFrames.put(new Short(Handle.Flow.FRAME_TYPE), Handle.Flow.class);
        _supportedFrames.put(new Short(Handle.Open.FRAME_TYPE), Handle.Open.class);
        _supportedFrames.put(new Short(Handle.Query.FRAME_TYPE), Handle.Query.class);
        _supportedFrames.put(new Short(Handle.Reply.FRAME_TYPE), Handle.Reply.class);
        _supportedFrames.put(new Short(Handle.Send.FRAME_TYPE), Handle.Send.class);
        _supportedFrames.put(new Short(Handle.Unget.FRAME_TYPE), Handle.Unget.class);

    }

    public MessageDecoderResult decodable(ProtocolSession session, ByteBuffer in)
    {
        final long frameSize = readFrameSize(in);

        if (frameSize < 0)
        {
            return MessageDecoderResult.NOT_OK;
        }

        if (frameSize == 0 || in.remaining() < frameSize)
        {
            return MessageDecoderResult.NEED_DATA;
        }

        short frameType = in.getUnsigned();

        if (isSupportedFrameType(frameType))
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
        final boolean result = _supportedFrames.containsKey(new Short(frameType));

        if (!result && _logger.isDebugEnabled())
        {
        	_logger.debug("AMQCommandFrameDecoder does not handle frame type " + frameType);
        }

        return(result);
    }

    /**
     * Read the frame size from the buffer.
     * @param in the buffer
     * @return the size indicated by the buffer, < 0 on error and 0 on needs more data.
     */
    protected long readFrameSize(ByteBuffer in)
    {
    	int remaining;

    	if ((remaining = in.remaining()) < EncodingUtils.SIZEOF_UNSIGNED_SHORT)
    	{
    		// Logging as point of interest (only) - for the time being.
    		// We now know that this does/can happen and this check (and return) sorts it out.
    		// _logger.info("Short-length (" + EncodingUtils.SIZEOF_UNSIGNED_SHORT + ") not received yet, only got " + remaining + " - waiting for more...");

    		return(0);
    	}

        long frameSize = in.getUnsignedShort();

        if (frameSize == FRAME_LONG)
        {
        	if ((remaining = in.remaining()) < EncodingUtils.SIZEOF_UNSIGNED_INT)
        	{
        		// Logging as point of interest (only) - for the time being.
        		_logger.info("Int-length (" + EncodingUtils.SIZEOF_UNSIGNED_INT + ") not received yet, only got " + remaining + " - waiting for more...");

        		return(0);
        	}

            frameSize = in.getUnsignedInt();
        }

        assert frameSize > 0;

        return frameSize;
    }

    protected Object createAndPopulateFrame(ByteBuffer in)
                    throws AMQFrameDecodingException
    {
        long frameSize = readFrameSize(in);

        short frameType = in.getUnsigned();
        Class frameClass = (Class) _supportedFrames.get(new Short(frameType));
        AMQCommandFrame frame;
        try
        {
            frame = (AMQCommandFrame) frameClass.newInstance();
            frame.populateFromBuffer(in);
        }
        catch (IllegalAccessException e)
        {
            throw new AMQFrameDecodingException("Unable to instantiate class of type " + frameClass);
        }
        catch (InstantiationException e)
        {
                throw new AMQFrameDecodingException("Unable to instantiate class of type " + frameClass);
        }
        byte marker = in.get();
        assert marker == 0xCE;
        return frame;
    }

}
