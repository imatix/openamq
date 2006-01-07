using System;
using log4net;

namespace OpenAMQ.Framing
{
    /// <summary>
    /// Thrown when a frame cannot be decoded. This generally indicates a mismatch between the broker and the
    /// client.
    /// </summary>
    public class AMQFrameDecodingException : AMQException
    {
        public AMQFrameDecodingException(string message)
            : base(message)
        {
        }

        public AMQFrameDecodingException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        public AMQFrameDecodingException(ILog logger, string message)
            : base(logger, message)
        {
        }

        public AMQFrameDecodingException(ILog logger, string message, Exception innerException)
            : base(logger, message, innerException)
        {
        }            
    }
}
