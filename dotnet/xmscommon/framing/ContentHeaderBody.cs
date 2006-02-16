using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class ContentHeaderBody : IBody
    {
        public static readonly byte TYPE = 2;

        public ushort ClassId;

        public ushort Weight;
        
        public ulong BodySize;

        /** must never be null */
        public IContentHeaderProperties Properties;

        public ContentHeaderBody()
        {
        }

        public ContentHeaderBody(IContentHeaderProperties props, ushort classId)
        {
            Properties = props;
            ClassId = classId;
        }

        public ContentHeaderBody(ushort classId, ushort weight, IContentHeaderProperties props, uint bodySize)
            : this(props, classId)
        {            
            Weight = weight;
            BodySize = bodySize;
        }

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
                return (2 + 2 + 8 + 2 + Properties.PropertyListSize);
            }
        }

        public void WritePayload(ByteBuffer buffer)
        {
            buffer.Put(ClassId);
            buffer.Put(Weight);
            buffer.Put(BodySize);
            buffer.Put(Properties.PropertyFlags);
            Properties.WritePropertyListPayload(buffer);
        }

        public void PopulateFromBuffer(ByteBuffer buffer, uint size)
        {     
            ClassId = buffer.GetUnsignedShort();
            Weight = buffer.GetUnsignedShort();
            BodySize = buffer.GetUnsignedLong();
            ushort propertyFlags = buffer.GetUnsignedShort();
            ContentHeaderPropertiesFactory factory = ContentHeaderPropertiesFactory.GetInstance();
            Properties = factory.CreateContentHeaderProperties(ClassId, propertyFlags, buffer);    
        }

        #endregion

        public static AMQFrame CreateAMQFrame(ushort channelId, ushort classId, ushort weight, BasicContentHeaderProperties properties,
                                              uint bodySize)
        {
            AMQFrame frame = new AMQFrame();
            frame.Channel = channelId;
            frame.BodyFrame = new ContentHeaderBody(classId, weight, properties, bodySize);
            return frame;
        }

        public static AMQFrame CreateAMQFrame(ushort channelId, ContentHeaderBody body)
        {
            AMQFrame frame = new AMQFrame();
            frame.Channel = channelId;
            frame.BodyFrame = body;
            return frame;
        }
    }
}
