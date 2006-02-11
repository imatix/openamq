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

        public AbstractXMSMessage CreateMessage(long messageNbr, bool redelivered, ContentHeaderBody contentHeader, System.Collections.IList bodies)
        {
            AbstractXMSMessage msg = CreateMessageWithBody(messageNbr, contentHeader, bodies);
            msg.XMSRedelivered = redelivered;
            return msg;
        }        

        #endregion

        /// <summary>
        /// 
        /// </summary>
        /// <param name="messageNbr"></param>
        /// <param name="contentHeader"></param>
        /// <param name="bodies"></param>
        /// <returns></returns>
        /// <exception cref="AMQException"></exception>
        protected abstract AbstractXMSMessage CreateMessageWithBody(long messageNbr,
                                                                    ContentHeaderBody contentHeader,
                                                                    IList bodies);               
    }
}
