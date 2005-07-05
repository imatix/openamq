package org.openamq.client.transport;

import org.apache.mina.protocol.ProtocolCodecFactory;
import org.apache.mina.protocol.ProtocolHandler;
import org.apache.mina.protocol.ProtocolProvider;
import org.apache.mina.protocol.codec.DemuxingProtocolCodecFactory;
import org.openamq.client.framing.AMQFrameEncoder;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.AMQCommandFrameDecoder;
import org.openamq.client.framing.Handle;
import org.openamq.client.AMQConnection;
import org.openamq.client.protocol.AMQProtocolHandler;

/**
 * Creates the codec factory and registers the codecs that are used to encode
 * (and decode) data sent to and from the server. All the code
 */
public class AMQProtocolProvider implements ProtocolProvider
{
    private DemuxingProtocolCodecFactory _factory;

    private AMQProtocolHandler _handler;

    public AMQProtocolProvider(AMQConnection connection)
    {
        _factory = new DemuxingProtocolCodecFactory();
        _factory.register(AMQFrameEncoder.class);
        _factory.register(Connection.Initiation.Decoder.class);
        _factory.register(AMQCommandFrameDecoder.class);
        _factory.register(Handle.Notify.Decoder.class);
        _handler = new AMQProtocolHandler(connection);
    }

    public ProtocolCodecFactory getCodecFactory()
    {
        return _factory;
    }

    public ProtocolHandler getHandler()
    {
        return _handler;
    }
}
