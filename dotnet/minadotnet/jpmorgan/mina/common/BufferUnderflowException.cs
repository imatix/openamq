using System;
using System.Text;

namespace jpmorgan.mina.common
{
    public class BufferUnderflowException : Exception
    {
        public BufferUnderflowException(string message)
            : base(message)
        {
        }
    }
}
