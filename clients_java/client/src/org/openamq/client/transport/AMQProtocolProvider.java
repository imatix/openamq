package org.openamq.client.transport;

import org.apache.mina.filter.codec.ProtocolCodecFactory;
import org.apache.mina.filter.codec.demux.DemuxingProtocolCodecFactory;
import org.apache.mina.handler.demux.DemuxingIoHandler;
import org.openamq.framing.AMQDataBlockDecoder;
import org.openamq.framing.AMQDataBlockEncoder;
import org.openamq.framing.ProtocolInitiation;

/**
 * Creates the codec factory and registers the codecs that are used to encode
 * (and decode) data sent to and from the server. All the code
 */
public class AMQProtocolProvider extends DemuxingIoHandler
{
    private DemuxingProtocolCodecFactory _factory;

    public AMQProtocolProvider()
    {
        _factory = new DemuxingProtocolCodecFactory();
        _factory.register(AMQDataBlockEncoder.class);

        AMQDataBlockDecoder decoder = new AMQDataBlockDecoder();
        _factory.register(decoder);
        _factory.register(new ProtocolInitiation.Decoder());
    }

    public ProtocolCodecFactory getCodecFactory()
    {
        return _factory;
    }
}
