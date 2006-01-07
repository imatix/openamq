using System;
using jpmorgan.mina.common;

namespace OpenAMQ.Framing
{
    /// <summary>
    /// Any class that is capable of turning a stream of bytes into an AMQ structure must implement this interface.
    /// </summary>
    public interface IBodyFactory
    {
        /// <summary>
        /// Creates the body.
        /// </summary>
        /// <param name="inbuf">The ByteBuffer containing data from the network</param>
        /// <returns></returns>
        /// <exception>AMQFrameDecodingException</exception>
        IBody CreateBody(ByteBuffer inbuf);
    }
}
