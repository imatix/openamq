using System;
using System.Collections.Generic;
using System.Text;

using jpmorgan.mina.common;
using jpmorgan.mina.filter.codec;

namespace jpmorgan.mina.filter.codec.demux
{
    public interface IMessageDecoder
    {
        /// <summary>
        /// Checks the specified buffer is decodable by this decoder.
        /// </summary>
        /// <param name="session">The session.</param>
        /// <param name="buffer">The buffer to read data from.</param>
        /// <returns>
        /// OK if this decoder can decode the specified buffer.
        /// NOT_OK if this decoder cannot decode the specified buffer.
        /// if more data is required to determine if the
        /// specified buffer is decodable ({@link #OK}) or not decodable
        /// {@link #NOT_OK}.</returns>             
        MessageDecoderResult Decodable(ISession session, ByteBuffer buffer);
    
        /// <summary>
        /// Decodes binary or protocol-specific content into higher-level message objects.
        /// MINA invokes {@link #decode(IoSession, ByteBuffer, ProtocolDecoderOutput)}
        /// method with read data, and then the decoder implementation puts decoded
        /// messages into {@link ProtocolDecoderOutput}.
        /// </summary>
        /// <returns>
        /// {@link #OK} if you finished decoding messages successfully.
        /// {@link #NEED_DATA} if you need more data to finish decoding current message.
        /// {@link #NOT_OK} if you cannot decode current message due to protocol specification violation.
        /// </returns>
        /// <exception cref="Exeption">if the read data violated protocol specification </exception>     
        MessageDecoderResult Decode(ISession session, ByteBuffer buffer, IProtocolDecoderOutput output);
    }
}
