using System;
using System.Text;
using jpmorgan.mina.filter.codec.demux;

namespace minadotnet.jpmorgan.mina.filter.codec.demux
{
    public interface IMessageEncoderFactory
    {
        /**
         * Creates a new message encoder.
         */
        IMessageEncoder NewEncoder();
    }
}
