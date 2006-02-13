using System;

namespace jpmorgan.mina.handler.demux
{
    public class UnknownMessageTypeException : Exception
    {
        public UnknownMessageTypeException(string message) : base(message)
        {            
        }
    }
}
