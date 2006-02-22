using System;
using System.Text;

namespace jpmorgan.mina.filter.codec
{
    public class ProtocolCodecException : Exception
    {
        public ProtocolCodecException() : base()
        {            
        }
        
        public ProtocolCodecException(string message) : base(message)
        {            
        }
        
        public ProtocolCodecException(Exception cause) : base("Codec Exception", cause)
        {            
        }
    }
}
