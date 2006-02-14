using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.filter.codec
{
    public class ProtocolDecoderException : ProtocolCodecException
    {
        private string _hexdump;
        
        public ProtocolDecoderException() : base()
        {            
        }
        
        public ProtocolDecoderException(string message) : base(message)
        {            
        }
        
        public ProtocolDecoderException(Exception cause) : base(cause)
        {            
        }
        
        public string HexDump
        {
            get
            {
                return _hexdump;
            }
            set
            {
                _hexdump = value;
            }
        }
    }
}
