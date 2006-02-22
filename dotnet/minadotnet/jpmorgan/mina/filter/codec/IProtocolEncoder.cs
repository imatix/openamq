using System;
using System.Text;
using jpmorgan.mina.common;

namespace jpmorgan.mina.filter.codec
{
    public interface IProtocolEncoder
    {
        /**
         * Encodes higher-level message objects into binary or protocol-specific data.
         * MINA invokes {@link #encode(IoSession, Object, ProtocolEncoderOutput)}
         * method with message which is popped from the session write queue, and then
         * the encoder implementation puts encoded {@link ByteBuffer}s into
         * {@link ProtocolEncoderOutput}.
         * 
         * @throws Exception if the message violated protocol specification
         */
        void Encode(ISession session, Object message,
                    IProtocolEncoderOutput output);

        
        /**
         * Releases all resources related with this encoder.
         * 
         * @throws Exception if failed to dispose all resources
         */
        void Dispose(ISession session);
    }
}
