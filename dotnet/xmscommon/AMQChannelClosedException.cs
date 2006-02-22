using System;
using System.Text;

namespace OpenAMQ
{
    public class AMQChannelClosedException : AMQException
    {
        public AMQChannelClosedException(int errorCode, string message)
            : base(errorCode, message)
        {
        }
    }
}
