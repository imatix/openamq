using System;
using System.Collections.Generic;
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
    }
}
