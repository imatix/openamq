using System;
using System.Text;
using jpmorgan.mina.common;

namespace jpmorgan.mina.filter.codec
{
    public interface IProtocolDecoder
    {
        /**
         * Decodes binary or protocol-specific content into higher-level message objects.
         * MINA invokes {@link #decode(IoSession, ByteBuffer, ProtocolDecoderOutput)}
         * method with read data, and then the decoder implementation puts decoded
         * messages into {@link ProtocolDecoderOutput}.
         * 
         * @throws Exception if the read data violated protocol specification
         */
        void Decode(ISession session, ByteBuffer input,
                    IProtocolDecoderOutput output);
        
        /**
         * Releases all resources related with this decoder.
         * 
         * @throws Exception if failed to dispose all resources
         */
        void Dispose(ISession session);
    }
}
