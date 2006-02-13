using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.filter.codec
{
    public class ProtocolDecoderException : ProtocolCodecException
    {
        public ProtocolDecoderException() : base()
        {            
        }
        
        public ProtocolDecoderException(string message) : base(message)
        {            
        }
    }
}
