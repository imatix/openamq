using System;
using System.Collections.Generic;
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
