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
        public byte[] payload;

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
                return (ushort)(payload == null ? 0 : payload.Length);
            }
        }

        public void WritePayload(ByteBuffer buffer)
        {
            if (payload != null)
            {
                buffer.Put(payload);
            }
        }

        public void PopulateFromBuffer(ByteBuffer buffer, uint size)
        {
            if (size > 0)
            {
                payload = new byte[size];
                buffer.Get(payload);
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
