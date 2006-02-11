using System;
using System.Collections;
using System.Text;
using IBM.XMS;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public interface IMessageFactory
    {
        /// <summary>
        /// Create a message
        /// </summary>
        /// <param name="messageNbr"></param>
        /// <param name="redelivered"></param>
        /// <param name="contentHeader"></param>
        /// <param name="bodies"></param>
        /// <returns></returns>
        /// <exception cref="XMSException">if the message cannot be created</exception>
        AbstractXMSMessage CreateMessage(ulong messageNbr, bool redelivered,
                                         ContentHeaderBody contentHeader,
                                         IList bodies);

        /// <summary>
        /// Creates the message.
        /// </summary>
        /// <returns></returns>
        /// <exception cref="XMSException">if the message cannot be created</exception>
        AbstractXMSMessage CreateMessage();
    }
}
