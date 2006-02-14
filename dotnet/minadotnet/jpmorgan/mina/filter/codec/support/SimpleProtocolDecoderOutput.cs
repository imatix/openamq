using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;

namespace jpmorgan.mina.filter.codec.support
{
    public class SimpleProtocolDecoderOutput : IProtocolDecoderOutput
    {
        private readonly Queue _messageQueue = new Queue();
        
        public SimpleProtocolDecoderOutput()
        {            
        }
        
        public Queue MessageQueue
        {
            get
            {
                return _messageQueue;
            }
        }
        
        public void Write(object message)
        {
            _messageQueue.Enqueue(message);
        }            
    }
}
