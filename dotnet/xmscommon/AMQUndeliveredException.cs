using System;
using System.Collections.Generic;
using System.Text;

namespace OpenAMQ
{
    /// <summary>
    /// Thrown when a message has been bounced by the broker, indicating it could not be delivered.
    /// </summary>
    public class AMQUndeliveredException : AMQException
    {
        public AMQUndeliveredException(int errorCode, string message)
            : base(errorCode, message)
        {
        }
    }
}
