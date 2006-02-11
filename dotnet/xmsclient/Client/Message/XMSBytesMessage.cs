using System;
using System.IO;
using System.Text;
using jpmorgan.mina.common;
using log4net;
using IBM.XMS;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Message
{
    public class XMSBytesMessage : AbstractXMSMessage, IBytesMessage
    {
        private const string MIME_TYPE = "application/octet-stream";

        /// <summary>
        /// The backingstore for the data
        /// </summary>
        private MemoryStream _dataStream;

        private int _bodyLength;                

        private BinaryReader _reader;

        private BinaryWriter _writer;

        XMSBytesMessage() : this(null)
        {            
        }

        /// <summary>
        /// Construct a bytes message with existing data.
        /// </summary>
        /// <param name="data">if data is not null, the message is immediately in read only mode. if data is null, it is in
        /// write-only mode</param>        
        XMSBytesMessage(byte[] data) : base()
        {
            // superclass constructor has instantiated a content header at this point
            XmsContentHeaderProperties.ContentType = MIME_TYPE;
            if (data == null)
            {
                _dataStream = new MemoryStream();
                _writer = new BinaryWriter(_dataStream);
            }
            else
            {
                _dataStream = new MemoryStream(data);
                _bodyLength = data.Length;
                _reader = new BinaryReader(_dataStream);
            }
        }

        XMSBytesMessage(long messageNbr, byte[] data, ContentHeaderBody contentHeader)
            // TODO: this casting is ugly. Need to review whole ContentHeaderBody idea
            : base(messageNbr, (BasicContentHeaderProperties) contentHeader.Properties)
        {                        
            XmsContentHeaderProperties.ContentType = MIME_TYPE;
            _dataStream = new MemoryStream(data);
            _bodyLength = data.Length;
            _reader = new BinaryReader(_dataStream);
        }

        public override void ClearBody()
        {
            if (_reader != null)
            {
                _reader.Close();
                _reader = null;
            }
            _dataStream = new MemoryStream();
            _bodyLength = 0;
            
            _writer = new BinaryWriter(_dataStream);
        }

        public override string ToBodyString()
        {
            CheckReadable();
            try
            {
                return GetText();
            }
            catch (IOException e)
            {
                throw new XMSException(e.ToString());
            }
        }
        
        private string GetText()
        {                  
            if (_dataStream != null)
            {
                // we cannot just read the underlying buffer since it may be larger than the amount of 
                // "filled" data. Length is not the same as Capacity.
                byte[] data = new byte[_dataStream.Length];
                _dataStream.Read(data, 0, _dataStream.Length);
                return Encoding.UTF8.GetString(data);
            }
            else
            {
                return null;
            }
        }

        public byte[] GetData()
        {
            
                if (_dataStream == null)
                {                    
                    return null;
                }
                else
                {
                    byte[] data = new byte[_dataStream.Length];
                    _dataStream.Read(data, 0, (int)_dataStream.Length);
                    return data;
                }                
        }

        public override string MimeType
        {
            get
            {
                return MIME_TYPE;
            }
        }

        public long BodyLength
        {
            get
            {
                CheckReadable();
                return _bodyLength;
            }
        }

        /// <summary>
        ///  
        /// </summary>
        /// <exception cref="MessageNotReadableException">if the message is in write mode</exception>
        private void CheckReadable() 
        {
            if (_reader == null)
            {
                throw new MessageNotReadableException("You need to call reset() to make the message readable");
            }
        }

        private void CheckWritable()
        {
            if (_reader != null)
            {
                throw new MessageNotWriteableException("You need to call clearBody() to make the message writable");
            }
        }

        public bool ReadBoolean()
        {
            CheckReadable();
            try
            {
                return _reader.ReadBoolean();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public byte ReadByte()
        {
            CheckReadable();
            try
            {
                return _reader.ReadByte();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public short ReadSignedByte()
        {
            CheckReadable();
            try
            {
                return _reader.ReadSByte();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public short ReadShort()
        {
            CheckReadable();
            try
            {
                return _reader.ReadInt16();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public int ReadUnsignedShort()
        {
            CheckReadable();
            try
            {
                return _reader.ReadUInt16();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public char ReadChar()
        {
            CheckReadable();
            try
            {
                return _reader.ReadChar();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public int ReadInt()
        {
            CheckReadable();
            try
            {
                return _reader.ReadInt32();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public long ReadLong()
        {
            CheckReadable();
            try
            {
                return _reader.ReadInt64();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public float ReadFloat()
        {
            CheckReadable();
            try
            {
                return _reader.ReadSingle();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public double ReadDouble()
        {
            CheckReadable();
            try
            {
                return _reader.ReadDouble();
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public string ReadUTF()
        {
            CheckReadable();
            try
            {
                byte[] data = _reader.ReadBytes((int)_dataStream.Length);
                return Encoding.UTF8.GetString(data);                
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public int ReadBytes(byte[] bytes)
        {
            if (bytes == null)
            {
                throw new ArgumentNullException("bytes");
            }
            CheckReadable();
            try
            {                
                return _reader.Read(bytes, 0, bytes.Length);
            }
            catch (IOException e)
            {
                XMSException ex =  new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public int ReadBytes(byte[] bytes, int count)
        {
            CheckReadable();
            if (bytes == null)
            {
                throw new ArgumentNullException("bytes");
            }
            if (count < 0)
            {
                throw new ArgumentOutOfRangeException("count must be >= 0");
            }
            if (count > bytes.Length)
            {
                count = bytes.Length;
            }

            try
            {
                return _reader.Read(bytes, 0, count);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;                
            }
        }

        public void WriteBoolean(bool b)
        {
            CheckWritable();
            try
            {
                _writer.Write(b);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void WriteByte(byte b)
        {
            CheckWritable();
            try
            {
                _writer.Write(b);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void WriteShort(short i)
        {
            CheckWritable();
            try
            {
                _writer.Write(i);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void WriteChar(char c)
        {
            CheckWritable();
            try
            {
                _writer.Write(c);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void Write(int i)
        {
            CheckWritable();
            try
            {
                _writer.Write(i);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void Write(long l)
        {
            CheckWritable();
            try
            {
                _writer.Write(l);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void Write(float v)
        {
            CheckWritable();
            try
            {
                _writer.Write(v);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void Write(double v)
        {
            CheckWritable();
            try
            {
                _writer.Write(v);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void WriteUTF(string value)
        {
            CheckWritable();
            try
            {
                byte[] encodedData = Encoding.UTF8.GetBytes(value);
                _writer.Write(encodedData);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void WriteBytes(byte[] bytes)
        {
            CheckWritable();
            try
            {
                _writer.Write(bytes);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void WriteBytes(byte[] bytes, int offset, int length)
        {
            CheckWritable();
            try
            {
                _writer.Write(bytes, offset, length);
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }

        public void WriteObject(object value)
        {
            CheckWritable();
            throw new XMSException("Not implemented");
        }

        public void Reset()
        {
            CheckWritable();
            try
            {
                _writer.Close();
                _writer = null;
                _reader = new BinaryReader(_dataStream);
                _bodyLength = (int) _dataStream.Length;
            }
            catch (IOException e)
            {
                XMSException ex = new XMSException(e.ToString());
                ex.LinkedException = e;
                throw ex;
            }
        }
    }
}
