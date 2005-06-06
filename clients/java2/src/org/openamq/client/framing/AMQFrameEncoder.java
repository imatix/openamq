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
public class AMQFrameEncoder implements MessageEncoder
{
	Logger _logger = Logger.getLogger(AMQFrameEncoder.class);
	
    protected ByteBuffer _buffer;

    private Set _messageTypes;

    public AMQFrameEncoder()
    {
        _messageTypes = new HashSet();
        //
        // Connection frames
        //
        _messageTypes.add(Connection.Initiation.class);
        _messageTypes.add(Connection.Challenge.class);
        _messageTypes.add(Connection.Response.class);
        _messageTypes.add(Connection.Tune.class);
        _messageTypes.add(Connection.Close.class);
        //
        // Channel frames
        //
        _messageTypes.add(Channel.Ack.class);
        _messageTypes.add(Channel.Close.class);
        _messageTypes.add(Channel.Commit.class);
        _messageTypes.add(Channel.Open.class);
        _messageTypes.add(Channel.Reply.class);
        _messageTypes.add(Channel.Rollback.class);
        //
        // Handle frames
        //
        _messageTypes.add(Handle.Browse.class);
        _messageTypes.add(Handle.Cancel.class);
        _messageTypes.add(Handle.Consume.class);
        _messageTypes.add(Handle.Created.class);
        _messageTypes.add(Handle.Flow.class);
        _messageTypes.add(Handle.Notify.class);
        _messageTypes.add(Handle.Open.class);
        _messageTypes.add(Handle.Query.class);
        _messageTypes.add(Handle.Send.class);
        _messageTypes.add(Handle.Unget.class);
    }

    public void encode(ProtocolSession session, Object message, ProtocolEncoderOutput out) throws ProtocolViolationException
    {
        final AMQFrame frame = (AMQFrame) message;
        int frameSize = (int)frame.getSize();
        _buffer = ByteBuffer.allocate(frameSize);
        frame.writePayload(_buffer);
       
        if (_logger.isInfoEnabled())
        {
        	_logger.info("Encoded frame byte-buffer is '" + EncodingUtils.convertToHexString(_buffer) + "'");
        }
        
        _buffer.flip();
        out.write(_buffer);
    }

    public Set getMessageTypes()
    {
        return _messageTypes;
    }
}
