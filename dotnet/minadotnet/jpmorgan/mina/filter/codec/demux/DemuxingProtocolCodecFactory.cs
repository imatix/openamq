using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;

namespace jpmorgan.mina.filter.codec.demux
{
    public class DemuxingProtocolCodecFactory : IProtocolCodecFactory
    {
        private ArrayList _decoderFactories = new ArrayList();
        private ArrayList _encoderFactories = new ArrayList();
        
        public void Register(Type encoderOrDecoderClass)
        {
            if (encoderOrDecoderClass == null)
            {
                throw new ArgumentNullException("encoderOrDecoderClass");
            }

            bool registered = false;
            if (typeof(IMessageEncoder).IsAssignableFrom(encoderOrDecoderClass))
            {
                Register(new DefaultConstructorMessageEncoderFactory(encoderOrDecoderClass));
                registered = true;
            }
            
            if (typeof(IMessageDecoder).IsAssignableFrom(encoderOrDecoderClass))
            {
                Register(new DefaultConstructorMessageDecoderFactory(encoderOrDecoderClass));
                registered = true;
            }
            
            if (!registered)
            {
                throw new ArgumentException("Unregisterable type: " + encoderOrDecoderClass);
            }
        }
        
        public void Register(IMessageEncoder encoder)
        {
            Register(new SingletonMessageEncoderFactory(encoder));
        }
        
        public void Register(IMessageEncoderFactory factory)
        {
            if (factory == null)
            {
                throw new ArgumentNullException("factory");
            }

            _encoderFactories.Add(factory);
        }
        
        public void Register(IMessageDecoder decoder)
        {
            Register(new SingletonMessageDecoderFactory(decoder));
        }
        
        public void Register(IMessageDecoderFactory factory)
        {
            if (factory == null)
            {
                throw new ArgumentNullException("factory");
            }
            _decoderFactories.Add(factory);
        }
    }
}
