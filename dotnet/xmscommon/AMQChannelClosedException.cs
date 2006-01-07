using System;
using System.Collections.Generic;
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
