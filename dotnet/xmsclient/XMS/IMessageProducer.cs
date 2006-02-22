using System;
using System.Text;
using IBM.XMS;

namespace OpenAMQ.XMS
{
    public interface IMessageProducer : IBM.XMS.IMessageProducer
    {
        /// <summary>
        /// Set the default MIME type for messages produced by this producer. This reduces the overhead of each message.
        /// </summary>
        /// <param>mimeType</param>         
        string MimeType
        {
            set;
        }

        /// <summary>
        /// Set the default encoding for messages produced by this producer. This reduces the overhead of each message.
        /// </summary>        
        string Encoding
        {
            set;
        }
        
        void Send(IDestination destination, IMessage message, DeliveryMode deliveryMode,
                  int priority, long timeToLive, bool immediate);

        void Send(IDestination destination, IMessage message, DeliveryMode deliveryMode,
                  int priority, long timeToLive, bool mandatory, bool immediate);                
    }
}
