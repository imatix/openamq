using System;
using System.Text;

namespace OpenAMQ
{
    public class AMQConnectionClosedException : AMQException
    {
        public AMQConnectionClosedException(int errorCode, string message)
            : base(errorCode, message)
        {
        }
    }
}
