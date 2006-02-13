using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.filter.codec.demux
{
    public interface IMessageDecoderFactory
    {
        /**
         * Creates a new message encoder.
         */
        IMessageDecoder NewDecoder();
    }
}
