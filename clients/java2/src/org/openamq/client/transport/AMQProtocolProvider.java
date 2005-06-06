package org.openamq.client.transport;

import org.apache.mina.protocol.ProtocolCodecFactory;
import org.apache.mina.protocol.ProtocolHandler;
import org.apache.mina.protocol.ProtocolProvider;
import org.apache.mina.protocol.codec.DemuxingProtocolCodecFactory;
import org.openamq.client.framing.AMQFrameEncoder;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.AMQCommandFrameDecoder;
import org.openamq.client.state.StateAwareProtocolHandler;
import org.openamq.client.AMQConnection;

/**
 */
public class AMQProtocolProvider implements ProtocolProvider
{
    private DemuxingProtocolCodecFactory _factory;

    private StateAwareProtocolHandler _handler;

    public AMQProtocolProvider(String username, String password,
                               String clientName, String virtualPath,
                               AMQConnection connection)
    {
        _factory = new DemuxingProtocolCodecFactory();
        _factory.register(AMQFrameEncoder.class);
        _factory.register(Connection.Initiation.Decoder.class);
        _factory.register(AMQCommandFrameDecoder.class);
        _handler = new StateAwareProtocolHandler(username, password, clientName,
                                                 virtualPath, connection);
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
