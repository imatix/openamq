using System;
using jpmorgan.mina.common;

namespace OpenAMQ.Framing
{
    /// <summary>
    /// An IBody is contained within a top level frame. As such, it is not en/decodable on its own but
    /// is decoded as a step within a the overall en/decoding process.
    /// </summary>
    public interface IBody
    {
        /// <summary>
        /// Gets the type. See RFC 006 for the meaning of "type" in this context.
        /// </summary>
        /// <value>The type.</value>
        byte BodyType
        {
            get;
        }
    
        /// <summary>
        /// Get the size of the body
        /// </summary>
        /// <value>The size in bytes.</value>         
        uint Size
        {
            get;
        }

        /// <summary>
        /// Writes this instance to a buffer.
        /// </summary>
        /// <param name="buffer">The buffer.</param>
        void WritePayload(ByteBuffer buffer);
    
        /// <summary>
        /// Populates this instance from a buffer of data.
        /// </summary>
        /// <param name="buffer">The buffer.</param>
        /// <param name="size">The size.</param>
        /// <exception cref="AMQFrameDecodingException">If the buffer contains data that cannot be decoded</exception>
        void PopulateFromBuffer(ByteBuffer buffer, uint size);
    }
}
