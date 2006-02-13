using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public abstract class AbstractXMSMessageFactory : IMessageFactory
    {
        #region IMessageFactory Members

        public AbstractXMSMessage CreateMessage(ulong messageNbr, bool redelivered, ContentHeaderBody contentHeader, System.Collections.IList bodies)
        {
            AbstractXMSMessage msg = CreateMessageWithBody(messageNbr, contentHeader, bodies);
            msg.JMSRedelivered = redelivered;
            return msg;
        }

        public abstract AbstractXMSMessage CreateMessage();

        #endregion

        /// <summary>
        /// 
        /// </summary>
        /// <param name="messageNbr"></param>
        /// <param name="contentHeader"></param>
        /// <param name="bodies"></param>
        /// <returns></returns>
        /// <exception cref="AMQException"></exception>
        protected abstract AbstractXMSMessage CreateMessageWithBody(ulong messageNbr,
                                                                    ContentHeaderBody contentHeader,
                                                                    IList bodies);               
void @__(){}    }
}
