using System.Collections;
using jpmorgan.mina.common;
using jpmorgan.mina.common.support;
using jpmorgan.mina.filter.codec;
using jpmorgan.mina.filter.codec.demux;
using log4net;

namespace OpenAMQ.Framing
{
    public class AMQDataBlockEncoder : IMessageEncoder
    {
        private static ILog _logger = LogManager.GetLogger(typeof(AMQDataBlockEncoder));

        private Hashtable _messageTypes;
        
        public AMQDataBlockEncoder()
        {
            _messageTypes = new Hashtable();
            _messageTypes[typeof (IEncodableAMQDataBlock)] = 1;
        }


        public Hashtable MessageTypes
        {
            get
            {
                return _messageTypes;
            }
        }

        public void Encode(ISession session, object message, IProtocolEncoderOutput output)
        {
            IDataBlock frame = (IDataBlock) message;
            int frameSize = (int)frame.Size; // TODO: sort out signed/unsigned
            ByteBuffer buffer = ByteBuffer.Allocate(frameSize);
            frame.WritePayload(buffer);
            
            if (_logger.IsDebugEnabled)
            {
                _logger.Debug("Encoded frame byte-buffer is '" + ByteBufferHexDumper.GetHexDump(buffer) + "'");
            }
            buffer.Flip();
            output.Write(buffer);
        }
    }
}
