using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class ContentBodyFactory : IBodyFactory
    {
        private static readonly ILog _log = LogManager.GetLogger(typeof(ContentBodyFactory));

        private static readonly ContentBodyFactory _instance = new ContentBodyFactory();

        public static ContentBodyFactory GetInstance()
        {
            return _instance;
        }

        private ContentBodyFactory()
        {
            _log.Debug("Creating content body factory");
        }

        /// <summary>
        /// Creates the body.
        /// </summary>
        /// <param name="inbuf">The ByteBuffer containing data from the network</param>
        /// <returns></returns>
        /// <exception>AMQFrameDecodingException</exception>
        public IBody CreateBody(ByteBuffer inbuf)
        {
            return new ContentBody();
        }
    }
}
