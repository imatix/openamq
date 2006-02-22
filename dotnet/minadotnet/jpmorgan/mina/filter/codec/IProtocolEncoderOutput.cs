using System;
using System.Text;
using jpmorgan.mina.common;

namespace jpmorgan.mina.filter.codec
{
    public interface IProtocolEncoderOutput
    {
        /**
         * Callback for {@link ProtocolEncoder} to generate encoded
         * {@link ByteBuffer}s. {@link ProtocolEncoder} must call
         * {@link #write(ByteBuffer)} for each decoded messages.
         * 
         * @param buf the buffer which contains encoded data
         */
        void Write(ByteBuffer buf);

        /**
         * Merges all buffers you wrote via {@link #write(ByteBuffer)} into
         * one {@link ByteBuffer} and replaces the old fragmented ones with it.
         * This method is useful when you want to control the way MINA generates
         * network packets.
         */
        void MergeAll();

        /**
         * Flushes all buffers you wrote via {@link #write(ByteBuffer)} to
         * the session.  This operation is asynchronous; please wait for
         * the returned {@link WriteFuture} if you want to wait for
         * the buffers flushed.
         */
        WriteFuture Flush();
    }
}
