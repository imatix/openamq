package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.protocol.ProtocolEncoderOutput;
import org.apache.mina.protocol.ProtocolSession;
import org.apache.mina.protocol.ProtocolViolationException;
import org.apache.mina.protocol.codec.MessageEncoder;

import java.util.Set;
import java.util.HashSet;

import org.apache.log4j.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQDataBlockEncoder implements MessageEncoder
{
	Logger _logger = Logger.getLogger(AMQDataBlockEncoder.class);
	
    protected ByteBuffer _buffer;

    private Set _messageTypes;

    public AMQDataBlockEncoder()
    {
        _messageTypes = new HashSet();
        //
        // Connection frames
        //
        _messageTypes.add(EncodableAMQDataBlock.class);
    }

    public void encode(ProtocolSession session, Object message, ProtocolEncoderOutput out) throws ProtocolViolationException
    {
        final AMQDataBlock frame = (AMQDataBlock) message;
        int frameSize = (int)frame.getSize();
        _buffer = ByteBuffer.allocate(frameSize + 4096);
        _buffer.setAutoExpand(true);
        frame.writePayload(_buffer);
       
        if (_logger.isDebugEnabled())
        {
        	_logger.debug("Encoded frame byte-buffer is '" + EncodingUtils.convertToHexString(_buffer) + "'");
        }
        
        _buffer.flip();
        out.write(_buffer);
    }

    public Set getMessageTypes()
    {
        return _messageTypes;
    }
}
