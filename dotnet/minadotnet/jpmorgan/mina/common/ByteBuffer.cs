using System;

namespace jpmorgan.mina.common
{
    public class ByteBuffer
    {
        private byte[] _underlyingData;

        /// <summary>
        /// The position of the next value to be read or written
        /// </summary>
        private int _position;

        /// <summary>
        /// The index of the first element that should not be read or written
        /// </summary>
        private int _limit;

        public ByteBuffer(int size)
        {
            _underlyingData = new byte[size];
        }

        public static ByteBuffer allocate(int size)
        {
            // naive implementation for now
            return new ByteBuffer(size);
        }

        public int Capacity
        {
            get
            {
                return _underlyingData.Length;
            }
        }

        public int Position
        {
            get
            {
                return _position;
            }
            set
            {
                _position = value;
            }
        }

        public int Limit
        {
            get
            {
                return _limit;
            }
            set
            {
                _limit = value;
            }
        }

        public void Clear()
        {
            _position = 0;
            _limit = Capacity;
        }

        public void Flip()
        {
            _limit = _position;
            _position = 0;
        }

        public void Rewind()
        {
            _position = 0;
        }

        public byte[] Buffer
        {
            get
            {
                return _underlyingData;
            }
        }
    }
}
