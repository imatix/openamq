using System;
using jpmorgan.mina.common;
using log4net;
using IBM.XMS;    

namespace OpenAMQ.Framing
{
    public class BasicContentHeaderProperties : IContentHeaderProperties
    {
        private static readonly ILog _log = LogManager.GetLogger(typeof(BasicContentHeaderProperties));

        public string ContentType;

        public string Encoding;

        public FieldTable Headers;

        public byte DeliveryMode;

        public byte Priority;

        public string CorrelationId;

        public uint Expiration;

        public string ReplyTo;

        public string MessageId;

        public ulong Timestamp;

        public string Type;

        public string UserId;

        public string AppId;

        public BasicContentHeaderProperties()
        {
        }

        public uint PropertyListSize
        {
            get
            {
                return (uint)(EncodingUtils.EncodedShortStringLength(ContentType) +
                       EncodingUtils.EncodedShortStringLength(Encoding) +
                       EncodingUtils.EncodedFieldTableLength(Headers) +
                       1 + 1 +
                       EncodingUtils.EncodedShortStringLength(CorrelationId) +
                       EncodingUtils.EncodedShortStringLength(ReplyTo) +
                       EncodingUtils.EncodedShortStringLength(String.Format("D", Expiration)) +
                       EncodingUtils.EncodedShortStringLength(MessageId) +
                       8 +
                       EncodingUtils.EncodedShortStringLength(Type) +
                       EncodingUtils.EncodedShortStringLength(UserId) +
                       EncodingUtils.EncodedShortStringLength(AppId));
            }
        }

        public uint PropertyFlags
        {   
            get
            {
                int value = 0;

                // for now we just blast in all properties
                for (int i = 0; i < 14; i++)
                {
                    value += (1 << (15-i));
                }
                return (uint) value;
            }
        }

        public void WritePropertyListPayload(ByteBuffer buffer)
        {
            EncodingUtils.WriteShortStringBytes(buffer, ContentType);
            EncodingUtils.WriteShortStringBytes(buffer, Encoding);
            EncodingUtils.WriteFieldTableBytes(buffer, Headers);
            buffer.Put(DeliveryMode);
            buffer.Put(Priority);
            EncodingUtils.WriteShortStringBytes(buffer, CorrelationId);
            EncodingUtils.WriteShortStringBytes(buffer, ReplyTo);
            EncodingUtils.WriteShortStringBytes(buffer, String.Format("D", Expiration));
            EncodingUtils.WriteShortStringBytes(buffer, MessageId);            
            buffer.Put(Timestamp);            
            EncodingUtils.WriteShortStringBytes(buffer, Type);
            EncodingUtils.WriteShortStringBytes(buffer, UserId);
            EncodingUtils.WriteShortStringBytes(buffer, AppId);
        }

        public void PopulatePropertiesFromBuffer(ByteBuffer buffer, uint propertyFlags) 
        {
            _log.Debug("Property flags: " + propertyFlags);
            if ((propertyFlags & (1 << 15)) > 0)
                ContentType = EncodingUtils.ReadShortString(buffer);
            if ((propertyFlags & (1 << 14)) > 0)
                Encoding = EncodingUtils.ReadShortString(buffer);
            if ((propertyFlags & (1 << 13)) > 0)
                Headers = EncodingUtils.ReadFieldTable(buffer);
            if ((propertyFlags & (1 << 12)) > 0)
                DeliveryMode = buffer.Get();
            if ((propertyFlags & (1 << 11)) > 0)
                Priority = buffer.Get();
            if ((propertyFlags & (1 << 10)) > 0)
                CorrelationId = EncodingUtils.ReadShortString(buffer);
            if ((propertyFlags & (1 << 9)) > 0)
                ReplyTo = EncodingUtils.ReadShortString(buffer);
            if ((propertyFlags & (1 << 8)) > 0)
                Expiration = UInt32.Parse(EncodingUtils.ReadShortString(buffer));
            if ((propertyFlags & (1 << 7)) > 0)
                MessageId = EncodingUtils.ReadShortString(buffer);
            if ((propertyFlags & (1 << 6)) > 0)                                            
                Timestamp = buffer.GetUnsignedLong();            
            if ((propertyFlags & (1 << 5)) > 0)
                Type = EncodingUtils.ReadShortString(buffer);
            if ((propertyFlags & (1 << 4)) > 0)
                UserId = EncodingUtils.ReadShortString(buffer);
            if ((propertyFlags & (1 << 3)) > 0)
                AppId = EncodingUtils.ReadShortString(buffer);
        }

        public void SetDeliveryMode(IBM.XMS.DeliveryMode deliveryMode)
        {
            if (deliveryMode == IBM.XMS.DeliveryMode.NonPersistent)
            {
                DeliveryMode = 1;
            }
            else
            {
                DeliveryMode = 2;
            }
        }
    }
}
