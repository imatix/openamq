using System;
using jpmorgan.mina.common;
using log4net;

namespace jpmorgan.mina.filter.codec
{
    public interface IProtocolCodecFactory
    {
        IProtocolEncoder Encoder
        {
            get;
        }

        IProtocolDecoder Decoder
        {
            get;
        }
    }
}
