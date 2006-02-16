using System;
using System.Collections;
using System.Text;
using jpmorgan.mina.common;
using jpmorgan.mina.common.support;
using log4net;
using IBM.XMS;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public abstract class AbstractXMSMessage : AMQMessage, IMessage
    {
        private static readonly Hashtable _destinationCache = Hashtable.Synchronized(new Hashtable());

        public const char BOOLEAN_PROPERTY_PREFIX = 'B';
        public const char BYTE_PROPERTY_PREFIX = 'b';
        public const char SHORT_PROPERTY_PREFIX = 's';
        public const char INT_PROPERTY_PREFIX = 'i';
        public const char LONG_PROPERTY_PREFIX = 'l';
        public const char FLOAT_PROPERTY_PREFIX = 'f';
        public const char DOUBLE_PROPERTY_PREFIX = 'd';
        public const char STRING_PROPERTY_PREFIX = 'S';

        protected ulong _messageNbr;

        protected bool _redelivered;

        protected AbstractXMSMessage() : base(new BasicContentHeaderProperties())
        {           
        }

        protected AbstractXMSMessage(ulong messageNbr, BasicContentHeaderProperties contentHeader)
            : this(contentHeader)
        {            
            _messageNbr = messageNbr;
        }

        protected AbstractXMSMessage(BasicContentHeaderProperties contentHeader) 
            : base(contentHeader)
        {            
        }

        public string JMSMessageID
        {
            get
            {
                if (XmsContentHeaderProperties.MessageId == null)
                {
                    XmsContentHeaderProperties.MessageId = "ID:" + _messageNbr;
                }
                return XmsContentHeaderProperties.MessageId;
            }
            set
            {
                XmsContentHeaderProperties.MessageId = value;
            }
        }        

        public long JMSTimestamp
        {
            get
            {
                // TODO: look at ulong/long choice
                return (long) XmsContentHeaderProperties.Timestamp;
            }
            set
            {
                XmsContentHeaderProperties.Timestamp = (ulong) value;
            }
        }        

        public byte[] JMSCorrelationIDAsBytes
        {
            get
            {            
                return Encoding.Default.GetBytes(XmsContentHeaderProperties.CorrelationId);
            }
            set
            {
                XmsContentHeaderProperties.CorrelationId = Encoding.Default.GetString(value);
            }
        }
        
        public string JMSCorrelationID
        {
            get
            {
                return XmsContentHeaderProperties.CorrelationId;
            }
            set
            {
                XmsContentHeaderProperties.ContentType = value;
            }
        }

        public IDestination JMSReplyTo
        {
            get
            {
                string replyToEncoding = XmsContentHeaderProperties.ReplyTo;
                if (replyToEncoding == null)
                {
                    return null;
                }
                else
                {
                    IDestination dest = (IDestination) _destinationCache[replyToEncoding];
                    if (dest == null)
                    {
                        char destType = replyToEncoding[0];
                        if (destType == 'Q')
                        {
                            dest = new AMQQueue(replyToEncoding.Substring(1));
                        }
                        else if (destType == 'T')
                        {
                            dest = new AMQTopic(replyToEncoding.Substring(1));
                        }
                        else
                        {
                            throw new XMSException("Illegal value in JMS_ReplyTo property: " + replyToEncoding);
                        }
                        _destinationCache[replyToEncoding] = dest;
                    }
                    return dest;
                }
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("Null destination not allowed");
                }
                if (!(value is AMQDestination))
                {
                    throw new ArgumentException("ReplyTo destination must be an AMQ destination - passed argument was type " +
                                                value.GetType());
                }
                AMQDestination amqd = (AMQDestination) value;

                string encodedDestination = amqd.EncodedName;
                _destinationCache[encodedDestination] = value;
                XmsContentHeaderProperties.ReplyTo = encodedDestination;
            }
        }

        public IDestination JMSDestination
        {
            get
            {
                // TODO: implement this once we have sorted out how to figure out the exchange class
                throw new Exception("not implemented");
            }
            set
            {
                throw new Exception("not implemented");
            }
        }        

        public DeliveryMode JMSDeliveryMode
        {
            get
            {
                byte b = XmsContentHeaderProperties.DeliveryMode;
                switch (b)
                {
                    case 1:
                        return DeliveryMode.NonPersistent;
                    case 2:
                        return DeliveryMode.Persistent;
                    default:
                        throw new XMSException("Illegal value for delivery mode in content header properties");
                }                
            }
            set
            {
                XmsContentHeaderProperties.DeliveryMode = (byte)(value==DeliveryMode.NonPersistent?1:2);
            }
        }        

        public bool JMSRedelivered
        {
            get
            {
                return _redelivered;
            }
            set
            {
                _redelivered = value;
            }
        }        

        public string JMSType
        {
            get
            {
                return MimeType;
            }            
            set
            {
                //MimeType = value;
            }
        }
        
        public long JMSExpiration
        {
            get
            {
                return XmsContentHeaderProperties.Expiration;
            }
            set
            {
                XmsContentHeaderProperties.Expiration = (uint) value;
            }
        }

        public int JMSPriority
        {
            get
            {
                return XmsContentHeaderProperties.Priority;
            }
            set
            {
                XmsContentHeaderProperties.Priority = (byte) value;
            }
        }        

        public void ClearProperties()
        {
            if (XmsContentHeaderProperties.Headers != null)
            {
                XmsContentHeaderProperties.Headers.Clear();
            }
        }

        public bool PropertyExists(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return false;
            }
            else
            {
                // TODO: fix this
                return XmsContentHeaderProperties.Headers.Contains(STRING_PROPERTY_PREFIX  + propertyName);
            }
        }

        public bool GetBooleanProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return false;
            }
            else
            {
                object b = XmsContentHeaderProperties.Headers[BOOLEAN_PROPERTY_PREFIX + propertyName];

                if (b == null)
                {
                    return false;
                }
                else
                {
                    return (bool)b;
                }
            }
        }

        public byte GetByteProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return 0;
            }
            else
            {
                object b = XmsContentHeaderProperties.Headers[BYTE_PROPERTY_PREFIX + propertyName];
                if (b == null)
                {
                    return 0;
                }
                else
                {
                    return (byte) b;
                }
            }
        }

        public short GetShortProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return 0;
            }
            else
            {
                object s = XmsContentHeaderProperties.Headers[SHORT_PROPERTY_PREFIX + propertyName];
                if (s == null)
                {
                    return 0;
                }
                else
                {
                    return (short)s;
                }
            }
        }

        public int GetIntProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return 0;
            }
            else
            {
                object i = XmsContentHeaderProperties.Headers[INT_PROPERTY_PREFIX + propertyName];
                if (i == null)
                {
                    return 0;
                }
                else
                {
                    return (int) i;
                }
            }
        }

        public long GetLongProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return 0;
            }
            else
            {
                object l = XmsContentHeaderProperties.Headers[LONG_PROPERTY_PREFIX + propertyName];
                if (l == null)
                {
                    // temp - the spec says do this but this throws a NumberFormatException
                    //return Long.valueOf(null).longValue();
                    return 0;
                }
                else
                {
                    return (long) l;
                }
            }
        }

        public float GetFloatProperty(String propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return 0;
            }
            else
            {
                object f = XmsContentHeaderProperties.Headers[FLOAT_PROPERTY_PREFIX + propertyName];
                if (f == null)
                {
                    return 0;
                }
                else
                {
                    return (float) f;
                }
            }
        }

        public double GetDoubleProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return 0;
            }
            else
            {
                object d = XmsContentHeaderProperties.Headers[DOUBLE_PROPERTY_PREFIX + propertyName];
                if (d == null)
                {
                    return 0;
                }
                else
                {
                    return (double) d;
                }
            }
        }

        public string GetStringProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            if (XmsContentHeaderProperties.Headers == null)
            {
                return null;
            }
            else
            {
                return (string) XmsContentHeaderProperties.Headers[STRING_PROPERTY_PREFIX + propertyName];
            }
        }

        public object GetObjectProperty(string propertyName)
        {
            CheckPropertyName(propertyName);
            throw new XMSException("Not implemented");
        }

        public IEnumerator PropertyNames
        {
            get
            {
                throw new XMSException("Not implemented");
            }
            set
            {
                throw new XMSException("Not implemented");
            }            
        }

        public void SetBooleanProperty(string propertyName, bool b)
        {
            CheckPropertyName(propertyName);
            XmsContentHeaderProperties.Headers[BOOLEAN_PROPERTY_PREFIX + propertyName] = b;
        }

        public void SetByteProperty(string propertyName, byte b)
        {
            CheckPropertyName(propertyName);
            XmsContentHeaderProperties.Headers[BYTE_PROPERTY_PREFIX + propertyName] = b;
        }

        public void SetShortProperty(string propertyName, short i)
        {
            CheckPropertyName(propertyName);
            XmsContentHeaderProperties.Headers[SHORT_PROPERTY_PREFIX + propertyName] = i;
        }

        public void SetIntProperty(string propertyName, int i)
        {
            CheckPropertyName(propertyName);
            XmsContentHeaderProperties.Headers[INT_PROPERTY_PREFIX + propertyName] = i;
        }

        public void SetLongProperty(string propertyName, long l)
        {
            CheckPropertyName(propertyName);
            XmsContentHeaderProperties.Headers[LONG_PROPERTY_PREFIX + propertyName] = l;
        }

        public void SetFloatProperty(string propertyName, float f)
        {
            CheckPropertyName(propertyName);
            XmsContentHeaderProperties.Headers[FLOAT_PROPERTY_PREFIX + propertyName] = f;
        }

        public void SetDoubleProperty(string propertyName, double v)
        {
            CheckPropertyName(propertyName);
            XmsContentHeaderProperties.Headers[DOUBLE_PROPERTY_PREFIX + propertyName] = v;
        }

        public void SetStringProperty(string propertyName, string value)
        {
            CheckPropertyName(propertyName);
            CreatePropertyMapIfRequired();
            propertyName = STRING_PROPERTY_PREFIX + propertyName;
            XmsContentHeaderProperties.Headers[propertyName] = value;
        }

        private void CreatePropertyMapIfRequired()
        {
            if (XmsContentHeaderProperties.Headers == null)
            {
                XmsContentHeaderProperties.Headers = new FieldTable();
            }
        }

        public void SetObjectProperty(string propertyName, object value)
        {
            throw new XMSException("Not implemented");
        }

        public void Acknowledge()
        {
            // the JMS 1.1 spec says in section 3.6 that calls to acknowledge are ignored when client acknowledge
            // is not specified. In our case, we only set the session field where client acknowledge mode is specified.
            if (_session != null)
            {
                _session.SendAcknowledgement(_messageNbr);
            }
        }

        public abstract void ClearBody();

        /// <summary>
        /// Get a String representation of the body of the message. Used in the
        /// toString() method which outputs this before message properties.
        /// </summary>
        /// <exception cref="XMSException"></exception>
        public abstract string ToBodyString();

        /// <summary>
        /// Return the raw byte array that is used to populate the frame when sending
        /// the message.
        /// </summary>
        /// <value>a byte array of message data</value>                
        public abstract byte[] Data
        {
            get;
            set;
        }

        public abstract string MimeType
        {
            get;           
        }

        public override string ToString()
        {
            try
            {
                StringBuilder buf = new StringBuilder("Body:\n");
                buf.Append(ToBodyString());
                buf.Append("\nXMS timestamp: ").Append(JMSTimestamp);
                buf.Append("\nXMS expiration: ").Append(JMSExpiration);
                buf.Append("\nXMS priority: ").Append(JMSPriority);
                buf.Append("\nXMS delivery mode: ").Append(JMSDeliveryMode);
                buf.Append("\nXMS reply to: ").Append(JMSReplyTo);
                buf.Append("\nAMQ message number: ").Append(_messageNbr);
                buf.Append("\nProperties:");
                if (XmsContentHeaderProperties.Headers == null)
                {
                    buf.Append("<NONE>");
                }
                else
                {
                    foreach (LinkedHashtable.LinkedDictionaryEntry entry in XmsContentHeaderProperties.Headers)                    
                    {
                        string propertyName = (string) entry.key;
                        if (propertyName == null)
                        {
                            buf.Append("\nInternal error: Property with NULL key defined");
                        }
                        else
                        {
                            buf.Append('\n').Append(propertyName.Substring(1));

                            char typeIdentifier = propertyName[0];
                            switch (typeIdentifier)
                            {
                                case BOOLEAN_PROPERTY_PREFIX:
                                    buf.Append("<boolean> ");
                                    break;
                                case BYTE_PROPERTY_PREFIX:
                                    buf.Append("<byte> ");
                                    break;
                                case SHORT_PROPERTY_PREFIX:
                                    buf.Append("<short> ");
                                    break;
                                case INT_PROPERTY_PREFIX:
                                    buf.Append("<int> ");
                                    break;
                                case LONG_PROPERTY_PREFIX:
                                    buf.Append("<long> ");
                                    break;
                                case FLOAT_PROPERTY_PREFIX:
                                    buf.Append("<float> ");
                                    break;
                                case DOUBLE_PROPERTY_PREFIX:
                                    buf.Append("<double> ");
                                    break;
                                case STRING_PROPERTY_PREFIX:
                                    buf.Append("<string> ");
                                    break;
                                 default:
                                    buf.Append("<unknown type (identifier " +
                                               typeIdentifier + ") ");
                                    break;
                            }
                            buf.Append(entry.value);
                        }
                    }
                }
                return buf.ToString();
            }
            catch (Exception e)
            {
                return e.ToString();
            }
        }

        public FieldTable UnderlyingMessagePropertiesMap
        {
            get
            {
                return XmsContentHeaderProperties.Headers;
            }
            set
            {
                XmsContentHeaderProperties.Headers = value;
            }
        }
        
        private void CheckPropertyName(string propertyName)
        {
            if (propertyName == null)
            {
                throw new ArgumentException("Property name must not be null");
            }
            else if ("".Equals(propertyName))
            {
                throw new ArgumentException("Property name must not be the empty string");
            }

            if (XmsContentHeaderProperties.Headers == null)
            {
                XmsContentHeaderProperties.Headers = new FieldTable();
            }
        }

        public FieldTable PopulateHeadersFromMessageProperties()
        {
            if (XmsContentHeaderProperties.Headers == null)
            {
                return null;
            }
            else
            {
                //
                // We need to convert every property into a String representation
                // Note that type information is preserved in the property name
                //
                FieldTable table = new FieldTable();
                foreach (DictionaryEntry entry in  XmsContentHeaderProperties.Headers)
                {                    
                    string propertyName = (string) entry.Key;
                    if (propertyName == null)
                    {
                        continue;
                    }
                    else
                    {
                        table[propertyName] = entry.Value.ToString();
                    }
                }
                return table;
            }
        }

        /// <summary>
        /// Get the AMQ message number assigned to this message
        /// </summary>
        /// <returns>the message number</returns>
        public ulong MessageNbr
        {
            get
            {
                return _messageNbr;
            }
            set
            {
                _messageNbr = value;
            }
        }        

        public BasicContentHeaderProperties XmsContentHeaderProperties
        {
            get
            {
                return (BasicContentHeaderProperties) _contentHeaderProperties;
            }
        }

        public short GetSignedByteProperty(string property_name)
        {
            throw new NotImplementedException();
        }

        public byte[] GetBytesProperty(string property_name)
        {
            throw new NotImplementedException();
        }

        public char GetCharProperty(string property_name)
        {
            throw new NotImplementedException();
        }

        public void SetSignedByteProperty(string property_name, short value)
        {
            throw new NotImplementedException();
        }

        public void SetBytesProperty(string property_name, byte[] value)
        {
            throw new NotImplementedException();
        }

        public void SetCharProperty(string property_name, char value)
        {
            throw new NotImplementedException();
        }
    }
}
