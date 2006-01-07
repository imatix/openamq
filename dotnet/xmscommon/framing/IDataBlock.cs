using System;
using jpmorgan.mina.common;

namespace OpenAMQ.Framing
{
    /// <summary>
    /// A data block represents something that has a size in bytes and the ability to write itself to a byte
    /// buffer (similar to a byte array). It represents "top level" frames in the protocol specification.
    /// </summary>
    public interface IDataBlock : IEncodableAMQDataBlock
    {
        /// <summary>
        /// Get the size of buffer needed to store the byte representation of this
        /// frame.
        /// </summary>
        /// <returns>size in bytes</returns>        
        uint Size
        {
            get;
        }

        /// <summary>
        /// Writes the datablock to the specified buffer.
        /// </summary>
        /// <param name="buffer">The buffer to write to. Must be the correct size.</param>
        void WritePayload(ByteBuffer buffer);
    }
}
