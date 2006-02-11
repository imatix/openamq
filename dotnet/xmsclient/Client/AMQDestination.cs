using System;
using IBM.XMS;

namespace OpenAMQ.XMS.Client 
{
    public abstract class AMQDestination : IDestination
    {
        public readonly static string TOPIC_EXCHANGE_NAME = "amq.topic";

        public readonly static string TOPIC_EXCHANGE_CLASS = "topic";

        public readonly static string QUEUE_EXCHANGE_NAME = "amq.direct";

        public readonly static string QUEUE_EXCHANGE_CLASS = "direct";

        public readonly static string HEADERS_EXCHANGE_NAME = "amq.match";

        public readonly static string HEADERS_EXCHANGE_CLASS = "headers";

        protected readonly string _exchangeName;

        protected readonly string _exchangeClass;

        protected readonly string _destinationName;

        protected readonly bool _isExclusive;

        protected readonly bool _isAutoDelete;

        protected string _queueName;

        protected AMQDestination(string exchangeName, string exchangeClass, string destinationName, string queueName)
            : this(exchangeName, exchangeClass, destinationName, false, false, queueName)
        {            
        }

        protected AMQDestination(string exchangeName, string exchangeClass, string destinationName)
            : this(exchangeName, exchangeClass, destinationName, false, false, null)
        {            
        }

        protected AMQDestination(String exchangeName, String exchangeClass, String destinationName, bool isExclusive,
                                 bool isAutoDelete, String queueName)
        {
            if (destinationName == null)
            {
                throw new ArgumentNullException("destinationName");
            }
            if (exchangeName == null)
            {
                throw new ArgumentNullException("exchangeName");
            }
            if (exchangeClass == null)
            {
                throw new ArgumentNullException("exchangeClass");
            }
            _exchangeName = exchangeName;
            _exchangeClass = exchangeClass;
            _destinationName = destinationName;
            _isExclusive = isExclusive;
            _isAutoDelete = isAutoDelete;
            _queueName = queueName;
        }

        #region IDestination Members

        public string Name
        {
            get
            {
                return _destinationName;
            }
        }

        public DestinationType TypeId
        {
            get
            {
                if (TOPIC_EXCHANGE_NAME.Equals(_exchangeName))
                {
                    return DestinationType.Topic;
                }
                else
                {
                    return DestinationType.Queue;
                }
            }
        }
        #endregion

        #region IPropertyContext Members

        public bool GetBooleanProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public byte GetByteProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public byte[] GetBytesProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public char GetCharProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public double GetDoubleProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public float GetFloatProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public int GetIntProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public long GetLongProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public object GetObjectProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public short GetShortProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public short GetSignedByteProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public string GetStringProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public bool PropertyExists(string propertyName)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetBooleanProperty(string property_name, bool value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetByteProperty(string property_name, byte value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetBytesProperty(string property_name, byte[] value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetCharProperty(string property_name, char value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetDoubleProperty(string property_name, double value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetFloatProperty(string property_name, float value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetIntProperty(string property_name, int value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetLongProperty(string property_name, long value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetObjectProperty(string property_name, object value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetShortProperty(string property_name, short value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetSignedByteProperty(string property_name, short value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetStringProperty(string property_name, string value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        public abstract string RoutingKey
        {
            get;
        }

        public abstract string EncodedName
        {
            get;
        }

        public bool Exclusive
        {
            get
            {
                return _isExclusive;
            }
        }

        public bool AutoDelete
        {
            get
            {
                return _isAutoDelete;
            }
        }

        public string QueueName
        {
            get
            {
                return _queueName;
            }
            set
            {
                _queueName = value;
            }
        }


        public string ExchangeName
        {
            get
            {
                return _exchangeName;
            }
        }

        public string ExchangeClass
        {
            get
            {
                return _exchangeClass;
            }
        }

        public bool IsExclusive
        {
            get
            {
                return _isExclusive;
            }
        }

        public bool IsAutoDelete
        {
            get
            {
                return _isAutoDelete;
            }
        }

        public override string ToString()
        {
            return "Destination: " + _destinationName + ", " +
                   "Queue Name: " + _queueName + ", Exchange: " + _exchangeName +
                   ", Exchange class: " + _exchangeClass + ", Exclusive: " + _isExclusive +
                   ", AutoDelete: " + _isAutoDelete + ", Routing  Key: " + RoutingKey;
        }

        public override bool Equals(object o)
        {
            if (this == o)
            {
                return true;
            }
            if (o == null || GetType() != o.GetType())
            {
                return false;
            }

            AMQDestination that = (AMQDestination) o;

            if (!_destinationName.Equals(that._destinationName))
            {
                return false;
            }
            if (!_exchangeClass.Equals(that._exchangeClass))
            {
                return false;
            }
            if (!_exchangeName.Equals(that._exchangeName))
            {
                return false;
            }
            if ((_queueName == null && that._queueName != null) ||
                (_queueName != null && !_queueName.Equals(that._queueName)))
            {
                return false;
            }
            if (_isExclusive != that._isExclusive)
            {
                return false;
            }
            if (_isAutoDelete != that._isAutoDelete)
            {
                return false;
            }
            return true;
        }

        public override int GetHashCode()
        {
            int result;
            result = _exchangeName.GetHashCode();
            result = 29 * result + _exchangeClass.GetHashCode();
            result = 29 * result + _destinationName.GetHashCode();
            if (_queueName != null)
            {
                result = 29 * result + _queueName.GetHashCode();
            }
            result = result * (_isExclusive ? 13 : 7);
            result = result * (_isAutoDelete ? 13 : 7);
            return result;
        }
    }
}
