using System;
using System.Text;
using jpmorgan.mina.common;
using log4net;

using IBM.XMS;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public class XMSTextMessage : AbstractXMSMessage, ITextMessage
    {
        private const string MIME_TYPE = "text/plain";

        private byte[] _data;

        private string _decodedValue;

        public XMSTextMessage() : this(null, null)
        {        
        }

        public XMSTextMessage(byte[] data, String encoding) : base()
        {
            // the superclass has instantied a content header at this point
            XmsContentHeaderProperties.ContentType= MIME_TYPE;
            _data = data;
            XmsContentHeaderProperties.Encoding = encoding;
        }

        public XMSTextMessage(ulong messageNbr, byte[] data, BasicContentHeaderProperties contentHeader)
            : base(messageNbr, contentHeader)
        {            
            contentHeader.ContentType = MIME_TYPE;
            _data = data;
        }

        public XMSTextMessage(byte[] data) : this(data, null)
        {            
        }

        public XMSTextMessage(string text)
        {
            Text = text;
        }

        public override void ClearBody()
        {
            _data = null;
            _decodedValue = null;
        }

        public override string ToBodyString()
        {
            return Text;
        }

        public override byte[] Data
        {
            get
            {
                return _data;
            }
            set
            {
                _data = value;
            }
        }

        public override string MimeType
        {
            get
            {
                return MIME_TYPE;
            }
        }        

        public string Text
        {
            get
            {
                if (_data == null && _decodedValue == null)
                {
                    return null;
                }
                else if (_decodedValue != null)
                {
                    return _decodedValue;
                }
                else
                {                    
                    if (XmsContentHeaderProperties.Encoding != null)
                    {
                        // throw ArgumentException if the encoding is not supported
                        _decodedValue = Encoding.GetEncoding(XmsContentHeaderProperties.Encoding).GetString(_data);
                    }
                    else
                    {
                        _decodedValue = Encoding.Default.GetString(_data);
                    }
                    return _decodedValue;                    
                }
            }

            set
            {            
                if (XmsContentHeaderProperties.Encoding == null)
                {
                    _data = Encoding.Default.GetBytes(value);
                }
                else
                {
                    // throw ArgumentException if the encoding is not supported
                    _data = Encoding.GetEncoding(XmsContentHeaderProperties.Encoding).GetBytes(value);
                }
                _decodedValue = value;
            }
        }
    }
}
