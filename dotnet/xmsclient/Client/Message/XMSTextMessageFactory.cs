using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public class XMSTextMessageFactory : AbstractXMSMessageFactory
    {
        protected override AbstractXMSMessage CreateMessageWithBody(long messageNbr, ContentHeaderBody contentHeader,
                                                                    IList bodies)
        {
               byte[] data;

            // we optimise the non-fragmented case to avoid copying
            if (bodies != null && bodies.Count == 1)
            {
                data = ((ContentBody)bodies[0]).Payload;
            }
            else
            {
                data = new byte[(int)contentHeader.BodySize];
                int currentPosition = 0;
                foreach (ContentBody cb in bodies)
                {                
                    Array.Copy(cb.Payload, 0, data, currentPosition, cb.Payload.Length);
                    currentPosition += cb.Payload.Length;
                }
            }

            return new XMSTextMessage(messageNbr, data, (BasicContentHeaderProperties)contentHeader.Properties);
        }
        
     
        public AbstractXMSMessage CreateMessage()
        {
            return new XMSTextMessage();
        }      
    }
}
