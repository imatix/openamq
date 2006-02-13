using System;
using System.Collections.Generic;
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
    }
}
