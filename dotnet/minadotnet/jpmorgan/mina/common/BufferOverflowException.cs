using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.common
{
    public class BufferOverflowException : Exception
    {
        public BufferOverflowException(string message)
            : base(message)
        {
        }
    }
}
