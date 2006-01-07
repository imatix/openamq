using System;
using jpmorgan.mina.common;
using log4net;
using IBM.XMS;

namespace OpenAMQ.XMS.Client
{
    public class AMQConnection : IConnection
    {
        #region IConnection Members

        public string ClientID
        {
            get
            {
                throw new Exception("The method or operation is not implemented.");
            }
            set
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        public void Close()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public IBM.XMS.ISession CreateSession(bool transacted, AcknowledgeMode acknowledgeMode)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public ExceptionListener ExceptionListener
        {
            get
            {
                throw new Exception("The method or operation is not implemented.");
            }
            set
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        public IConnectionMetaData MetaData
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public void Start()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void Stop()
        {
            throw new Exception("The method or operation is not implemented.");
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
    }
}
