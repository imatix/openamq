using System;
using jpmorgan.mina.common;

namespace OpenAMQ.Framing
{
    /// <summary>
    /// There will be an implementation of this interface for each content type. All content types have associated
    /// header properties and this provides a way to encode and decode them.
    /// </summary>    
    public interface IContentHeaderProperties
    {
        /// <summary>
        /// Writes the property list to the buffer, in a suitably encoded form.
        /// </summary>
        /// <param name="buffer">The buffer to write to</param>
        void WritePropertyListPayload(ByteBuffer buffer);

        /// <summary>
        /// Populates the properties from buffer.
        /// </summary>
        /// <param name="buffer">The buffer to read from.</param>
        /// <param name="propertyFlags">The property flags.</param>
        /// <exception cref="AMQFrameDecodingException">Thrown when the buffer does not contain valid data</exception>
        void PopulatePropertiesFromBuffer(ByteBuffer buffer, uint propertyFlags);

        /// <summary>
        /// Gets the size of the encoded property list in bytes.
        /// </summary>
        /// <value>The size of the property list in bytes</value>        
        uint PropertyListSize
        {
            get;
        }

        /// <summary>
        /// Gets the property flags. Property flags indicate which properties are set in the list. The
        /// position and meaning of each flag is defined in the protocol specification for the particular
        /// content type with which these properties are associated.
        /// </summary>
        /// <value>the flags as an unsigned integer</value>
        uint PropertyFlags
        {
            get;
        }
    }
}
