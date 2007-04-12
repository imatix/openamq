package org.openamq.framing;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.demux.MessageEncoder;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;

import java.util.HashSet;
import java.util.Set;

public class AMQDataBlockEncoder implements MessageEncoder
{
	Logger _logger = Logger.getLogger(AMQDataBlockEncoder.class);

    private Set _messageTypes;

    public AMQDataBlockEncoder()
    {
        _messageTypes = new HashSet();
        _messageTypes.add(EncodableAMQDataBlock.class);
    }

    public void encode(IoSession session, Object message, ProtocolEncoderOutput out) throws Exception
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
