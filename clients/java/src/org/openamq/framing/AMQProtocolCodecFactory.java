package org.openamq.framing;

import org.apache.mina.filter.codec.ProtocolCodecFactory;
import org.apache.mina.filter.codec.ProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolEncoder;

public class AMQProtocolCodecFactory implements ProtocolCodecFactory {
    AMQProtocolEncoder
        encoder;
    AMQProtocolDecoder
        decoder;

    public AMQProtocolCodecFactory()
    {
        encoder = new AMQProtocolEncoder();
        decoder = new AMQProtocolDecoder();
    }

    public ProtocolEncoder getEncoder()
    {
        return encoder;
    }

    public ProtocolDecoder getDecoder()
    {
        return decoder;
    }
}
