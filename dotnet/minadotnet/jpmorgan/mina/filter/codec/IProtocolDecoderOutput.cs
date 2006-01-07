using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.filter.codec
{
    public interface IProtocolDecoderOutput
    {
        /// <summary>
        /// Callback for {@link ProtocolDecoder} to generate decoded messages.
        /// {@link ProtocolDecoder} must call {@link #write(Object)} for each
        /// decoded messages.
        /// </summary>
        /// <param name="message">the decoded message</param>
        void Write(object message);
    }
}
