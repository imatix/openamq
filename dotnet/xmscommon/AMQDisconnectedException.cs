using System;
using System.Text;

namespace OpenAMQ
{
    public class AMQDisconnectedException : AMQException
    {
        public AMQDisconnectedException(int errorCode, string message)
            : base(errorCode, message)
        {
        }
        
        public AMQDisconnectedException(string message)
            : base(message)
        {            
        }
    }
}
