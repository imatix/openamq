using System;
using jpmorgan.mina.common;
using jpmorgan.mina.filter.codec;
using jpmorgan.mina.filter.codec.demux;
using jpmorgan.mina.handler.demux;
using log4net;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Transport
{
    public class AMQProtocolProvider : DemuxingIoHandler
    {
        private DemuxingProtocolCodecFactory _factory;
        
        public AMQProtocolProvider()
        {
            _factory = new DemuxingProtocolCodecFactory();
            _factory.Register(typeof (AMQDataBlockEncoder));
            _factory.Register(new ProtocolInitiation.Decoder());
        }
        
        public IProtocolCodecFactory CodecFactory
        {
            get
            {
                return _factory;
            }
        }
    }
}
