using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class ContentBody : IBody
    {
        public static readonly byte TYPE = 3;

        /// <summary>
        /// 
        /// </summary>
        /// TODO: consider whether this should be a pointer into the ByteBuffer to avoid copying */
        public byte[] Payload;

        #region IBody Members

        public byte BodyType
        {
            get
            {
                return TYPE;
            }
        }

        public uint Size
        {
            get
            {
                return (ushort)(Payload == null ? 0 : Payload.Length);
            }
        }

        public void WritePayload(ByteBuffer buffer)
        {
            if (Payload != null)
            {
                buffer.Put(Payload);
            }
        }

        public void PopulateFromBuffer(ByteBuffer buffer, uint size)
        {
            if (size > 0)
            {
                Payload = new byte[size];
                buffer.Get(Payload);
            }
        }

        #endregion

        public static AMQFrame CreateAMQFrame(ushort channelId, ContentBody body)
        {
            AMQFrame frame = new AMQFrame();
            frame.Channel = channelId;
            frame.BodyFrame = body;
            return frame;
        }
    }
}
