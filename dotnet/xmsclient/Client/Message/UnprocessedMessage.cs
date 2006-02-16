using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public class UnprocessedMessage
    {
        private ulong _bytesReceived = 0;

        public BasicDeliverBody DeliverBody;
        public BasicReturnBody BounceBody;
        public ushort ChannelId;
        public ContentHeaderBody ContentHeader;

        /// <summary>
        /// List of ContentBody instances. Due to fragmentation you don't know how big this will be in general
        /// </summary>
        /// TODO: write and use linked list class
        public IList Bodies = new ArrayList();

        public void ReceiveBody(ContentBody body)
        {
            Bodies.Add(body);
            if (body.Payload != null)
            {
                _bytesReceived += (uint)body.Payload.Length;
            }
        }

        public bool IsAllBodyDataReceived()
        {
            return _bytesReceived == ContentHeader.BodySize;
        }

    }
}
