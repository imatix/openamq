package org.openamq.client.transport;

import org.apache.mina.protocol.ProtocolCodecFactory;
import org.apache.mina.protocol.ProtocolHandler;
import org.apache.mina.protocol.ProtocolProvider;
import org.apache.mina.protocol.codec.DemuxingProtocolCodecFactory;
import org.openamq.framing.AMQDataBlockEncoder;
import org.openamq.framing.AMQDataBlockDecoder;
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
        _factory.register(AMQDataBlockEncoder.class);

        AMQDataBlockDecoder decoder = new AMQDataBlockDecoder();
        _factory.register(decoder);
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
