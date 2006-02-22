using System;
using System.Text;

namespace jpmorgan.mina.filter.codec
{
    public class ProtocolEncoderException : ProtocolCodecException
    {
        public ProtocolEncoderException() : base()
        {            
        }
        
        public ProtocolEncoderException(string message) : base(message)
        {            
        }
        
        public ProtocolEncoderException(Exception cause) : base(cause)
        {            
        }
    }
}
