using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class AMQMethodBodyFactory : IBodyFactory
    {
        private static readonly AMQMethodBodyFactory _instance = new AMQMethodBodyFactory();

        public static AMQMethodBodyFactory GetInstance()
        {
            return _instance;
        }

        #region IBodyFactory Members

        /// <summary>
        /// Creates the body.
        /// </summary>
        /// <param name="inbuf">The ByteBuffer containing data from the network</param>
        /// <returns></returns>
        /// <exception>AMQFrameDecodingException</exception>
        public IBody CreateBody(ByteBuffer inbuf)
        {
            return MethodBodyDecoderRegistry.Get(inbuf.GetUnsignedShort(), inbuf.GetUnsignedShort());
        }

        #endregion
    }
}
