using System;
using System.Text;
using jpmorgan.mina.common;

namespace jpmorgan.mina.filter.codec
{
    public abstract class ProtocolDecoderAdapter : IProtocolDecoder
    {
        public abstract void Decode(ISession session, ByteBuffer input, IProtocolDecoderOutput output);        

        public virtual void Dispose(ISession session)
        {            
        }
    }
}
