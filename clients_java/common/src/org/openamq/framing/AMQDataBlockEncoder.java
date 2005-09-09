package org.openamq.framing;

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

    private Set _messageTypes;

    public AMQDataBlockEncoder()
    {
        _messageTypes = new HashSet();
        _messageTypes.add(EncodableAMQDataBlock.class);
    }

    public void encode(ProtocolSession session, Object message, ProtocolEncoderOutput out) throws ProtocolViolationException
    {
        final AMQDataBlock frame = (AMQDataBlock) message;
        int frameSize = (int)frame.getSize();
        final ByteBuffer buffer = ByteBuffer.allocate(frameSize);
        //buffer.setAutoExpand(true);
        frame.writePayload(buffer);
       
        if (_logger.isDebugEnabled())
        {
        	_logger.debug("Encoded frame byte-buffer is '" + EncodingUtils.convertToHexString(buffer) + "'");
        }
        
        buffer.flip();
        out.write(buffer);
    }

    public Set getMessageTypes()
    {
        return _messageTypes;
    }
}
