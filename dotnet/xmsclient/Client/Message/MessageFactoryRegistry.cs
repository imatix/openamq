using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;
using IBM.XMS;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public class MessageFactoryRegistry
    {
        private readonly Hashtable _mimeToFactoryMap = new Hashtable();

        public void RegisterFactory(string mimeType, IMessageFactory mf)
        {    
            if (mf == null)
            {
                throw new ArgumentNullException("Message factory");
            }
            if (mimeType == null)
            {
                throw new ArgumentNullException("mf");
            }
            _mimeToFactoryMap[mimeType] = mf;
        }

        public void DeregisterFactory(string mimeType)
        {
            _mimeToFactoryMap.Remove(mimeType);
        }

        /// <summary>
        /// Create a message. This looks up the MIME type from the content header and instantiates the appropriate
        /// concrete message type.
        /// </summary>
        /// <param name="messageNbr">the AMQ message id</param>
        /// <param name="redelivered">true if redelivered</param>
        /// <param name="contentHeader">the content header that was received</param>
        /// <param name="bodies">a list of ContentBody instances</param>
        /// <returns>the message.</returns>
        /// <exception cref="AMQException"/>
        /// <exception cref="XMSException"/>
        public AbstractXMSMessage CreateMessage(ulong messageNbr, bool redelivered,
                                                ContentHeaderBody contentHeader,
                                                IList bodies)
        {
            BasicContentHeaderProperties properties =  (BasicContentHeaderProperties) contentHeader.Properties;
            IMessageFactory mf = (IMessageFactory) _mimeToFactoryMap[properties.ContentType];
            if (mf == null)
            {
                throw new AMQException("Unsupport MIME type of " + properties.ContentType);
            }
            else
            {
                return mf.CreateMessage(messageNbr, redelivered, contentHeader, bodies);
            }
        }

        public AbstractXMSMessage CreateMessage(string mimeType)
        {
            if (mimeType == null)
            {
                throw new ArgumentNullException("Mime type must not be null");
            }
            IMessageFactory mf = (IMessageFactory) _mimeToFactoryMap[mimeType];
            if (mf == null)
            {
                throw new AMQException("Unsupport MIME type of " + mimeType);
            }
            else
            {
                return mf.CreateMessage();
            }
        }

        /// <summary>
        /// Construct a new registry with the default message factories registered
        /// </summary>
        /// <returns>a message factory registry</returns>
        public static MessageFactoryRegistry NewDefaultRegistry()
        {
            MessageFactoryRegistry mf = new MessageFactoryRegistry();
            mf.RegisterFactory("text/plain", new XMSTextMessageFactory());
            mf.RegisterFactory("text/xml", new XMSTextMessageFactory());
            mf.RegisterFactory("application/octet-stream", new XMSBytesMessageFactory());
            // TODO: use bytes message for default message factory
            //if (FieldTable.grm)
            mf.RegisterFactory(null, new XMSBytesMessageFactory());
            return mf;
        }
    }
}
