using System;
using System.Text;
using jpmorgan.mina.common;
using log4net;

namespace jpmorgan.mina.common
{
    public class ByteBufferProxy : ByteBuffer
    {
        protected ByteBuffer _buf;
        
        protected ByteBufferProxy(ByteBuffer buf)
        {
            if (buf == null)
            {
                throw new ArgumentNullException("buf");
            }
            _buf = buf;
        }


        public override void Acquire()
        {
            _buf.Acquire();
        }

        public override void Release()
        {
            _buf.Release();
        }

        public override int Capacity
        {
            get { return _buf.Capacity;  }
        }

        public override bool IsAutoExpand
        {
            get { return _buf.IsAutoExpand;  }
            set { _buf.IsAutoExpand = value; }
        }

        public override void Expand(int expectedRemaining)
        {
            _buf.Expand(expectedRemaining);                
        }

        public override void Expand(int pos, int expectedRemaining)
        {
            _buf.Expand(pos, expectedRemaining);
        }

        public override bool Pooled
        {
            get { return _buf.Pooled; }
            set { _buf.Pooled = value; }
        }

        public override int Position
        {
            get { return _buf.Position; }
            set { _buf.Position = value; }
        }

        public override int Limit
        {
            get { return _buf.Limit; }
            set { _buf.Limit = value; }
        }

        public override void Clear()
        {
            _buf.Clear();
        }

        public override void Flip()
        {
            _buf.Flip(); 
        }

        public override void Rewind()
        {
            _buf.Rewind();
        }

        public override int Remaining
        {
            get { return _buf.Remaining; }
        }

        public override byte Get()
        {
            return _buf.Get();
        }

        public override byte Get(int index)
        {
            return _buf.Get(index);
        }

        public override void Get(byte[] destination)
        {
            _buf.Get(destination);
        }

        public override ushort GetUnsignedShort()
        {
            return _buf.GetUnsignedShort();
        }

        public override uint GetUnsignedInt()
        {
            return _buf.GetUnsignedInt();
        }

        public override ulong GetUnsignedLong()
        {
            return _buf.GetUnsignedLong();
        }

        public override string GetString(uint length, Encoding encoder)
        {
            return _buf.GetString(length, encoder);
        }

        public override void Put(byte data)
        {
            _buf.Put(data);
        }

        public override void Put(byte[] data)
        {
            _buf.Put(data);
        }

        public override void Put(ushort data)
        {
            _buf.Put(data);
        }

        public override void Put(uint data)
        {
            _buf.Put(data);
        }

        public override void Put(ulong data)
        {
            _buf.Put(data);
        }

        public override void Put(ByteBuffer buf)
        {
            _buf.Put(buf);
        }

        public override void Compact()
        {
            _buf.Compact();
        }

        public override byte[] ToByteArray()
        {
            return _buf.ToByteArray();
        }
    }
}
