using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class ContentHeaderBodyFactory : IBodyFactory
    {
        private static readonly ILog _log = LogManager.GetLogger(typeof(ContentHeaderBodyFactory));

        private static readonly ContentHeaderBodyFactory _instance = new ContentHeaderBodyFactory();

        public static ContentHeaderBodyFactory GetInstance()
        {
            return _instance;
        }

        private ContentHeaderBodyFactory()
        {
            _log.Debug("Creating content header body factory");
        }

        #region IBodyFactory Members

        public IBody CreateBody(ByteBuffer inbuf)
        {
            // all content headers are the same - it is only the properties that differ.
            // the content header body further delegates construction of properties
            return new ContentHeaderBody();
        }

        #endregion
    }
}
