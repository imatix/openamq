using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class CompositeAMQDataBlock : IDataBlock, IEncodableAMQDataBlock
    {
        private IDataBlock[] _blocks;

        public CompositeAMQDataBlock(IDataBlock[] blocks)
        {
            _blocks = blocks;
        }

        public IDataBlock[] Blocks
        {
            get
            {
                return _blocks;
            }
        }

        public uint Size
        {
            get
            {
                uint frameSize = 0;
                foreach (IDataBlock block in _blocks)
                {
                    frameSize += block.Size;
                }
                return frameSize;
            }
        }

        public void WritePayload(ByteBuffer buffer)
        {
            foreach (IDataBlock block in _blocks)
            {
                block.WritePayload(buffer);
            }
        }
    }
}
