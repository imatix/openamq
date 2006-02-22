using System;
using System.Text;

namespace OpenAMQ
{
    /// <summary>
    /// Thrown when a message has been bounced by the broker, indicating it could not be delivered.
    /// </summary>
    public class AMQUndeliveredException : AMQException
    {
        private object _bounced;
        
        public AMQUndeliveredException(int errorCode, string message, object bounced)
            : base(errorCode, message)
        {
            _bounced = bounced;
        }
    }
}
