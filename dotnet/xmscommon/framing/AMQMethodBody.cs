using System;
using System.Text;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public abstract class AMQMethodBody : IBody
    {
        public const int TYPE = 1;

        public uint _synchTag;

        public uint SynchTag
        {
            get
            {
                return _synchTag;
            }
            set
            {
                _synchTag = value;
            }
        }

        protected abstract uint BodySize
        {
            get;
        }

        protected abstract ushort Clazz
        {
            get;
        }

        protected abstract ushort Method
        {
            get;
        }

        protected abstract void WriteMethodPayload(ByteBuffer buffer);

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
                return (uint) (2 + 2 + BodySize);
            }
        }

        public void WritePayload(ByteBuffer buffer)
        {
            buffer.Put(Clazz);
            buffer.Put(Method);
            WriteMethodPayload(buffer);
        }

        /// <summary>
        /// Populates the method body by decoding the specified buffer
        /// </summary>
        /// <param name="buffer">The buffer to decode.</param>
        /// <exception cref="AMQFrameDecodingException">If the buffer cannot be decoded</exception>
        protected abstract void PopulateMethodBodyFromBuffer(ByteBuffer buffer);

        /// <summary>
        /// Populates this instance from a buffer of data.
        /// </summary>
        /// <param name="buffer">The buffer.</param>
        /// <param name="size">The size.</param>
        /// <exception cref="AMQFrameDecodingException">If the buffer contains data that cannot be decoded</exception>
        public void PopulateFromBuffer(ByteBuffer buffer, uint size)
        {
            PopulateMethodBodyFromBuffer(buffer);
        }

        public override string ToString()
        {
            StringBuilder buf = new StringBuilder(GetType().ToString());
            buf.Append("\n\tClass: ").Append(Clazz);
            buf.Append("\n\tMethod: ").Append(Method);
            return buf.ToString();
        }
    }
}
