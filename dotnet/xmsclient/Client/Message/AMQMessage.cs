using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{

    public class AMQMessage
    {
        protected IContentHeaderProperties _contentHeaderProperties;

        /// <summary>
        /// If the acknowledge mode is CLIENT_ACKNOWLEDGE the session is required
        /// </summary>
        protected AMQSession _session;

        public AMQMessage(IContentHeaderProperties properties)
        {
            _contentHeaderProperties = properties;
        }

        public AMQSession Session
        {
            get
            {
                return _session;
            }

            set
            {
                _session = value;
            }
        }
    }
}
