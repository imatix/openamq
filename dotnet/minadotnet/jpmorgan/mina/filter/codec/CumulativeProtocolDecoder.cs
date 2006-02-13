using System;
using System.Collections.Generic;
using System.Text;
using jpmorgan.mina.common;

namespace jpmorgan.mina.filter.codec
{
    public abstract class CumulativeProtocolDecoder : ProtocolDecoderAdapter
    {
        private static string BUFFER = typeof(CumulativeProtocolDecoder).Name + ".Buffer";
        
        private readonly int _initialCapacity;
        
        /**
         * Creates a new instance with the 16 bytes initial capacity of
         * cumulative buffer.  Please note that the capacity increases
         * automatically.
         */
        protected CumulativeProtocolDecoder() : this(16)
        {            
        }
        
        /**
         * Creates a new instance with the specified initial capacity of
         * cumulative buffer.  Please note that the capacity increases
         * automatically.
         */
        protected CumulativeProtocolDecoder( int initialCapacity )
        {
            if (initialCapacity < 0)
            {
                throw new ArgumentException("initialCapacity: " + initialCapacity);
            }
            
            _initialCapacity = initialCapacity;
        }
        
        /**
         * Cumulates content of <tt>in</tt> into internal buffer and forwards
         * decoding request to {@link #doDecode(IoSession, ByteBuffer, ProtocolDecoderOutput)}.
         * <tt>doDecode()</tt> is invoked repeatedly until it returns <tt>false</tt>
         * and the cumulative buffer is compacted after decoding ends.
         * 
         * @throws IllegalStateException if your <tt>doDecode()</tt> returned
         *                               <tt>true</tt> not consuming the cumulative buffer.
         */
        public override void Decode(ISession session, ByteBuffer input,
                                    IProtocolDecoderOutput output)
        {
            ByteBuffer buf = (ByteBuffer) session.GetAttribute(BUFFER);
            if (buf == null)
            {
                buf = ByteBuffer.Allocate(_initialCapacity);
                buf.AutoExpand = true;
                session.SetAttribute(BUFFER, buf);
            }
            
            buf.Put(input);
            buf.Flip();

            try
            {
                for (;;)
                {
                    int oldPos = buf.Position;
                    bool decoded = DoDecode(session, buf, output);
                    if (decoded)
                    {
                        if (buf.Position == oldPos)
                        {
                            throw new Exception(
                                    "doDecode() can't return true when buffer is not consumed." );
                        }
                        
                        if (!buf.HasRemaining)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            finally
            {
                buf.Compact();
            }
        }
        
        /**
         * Implement this method to consume the specified cumulative buffer and
         * decode its content into message(s). 
         *  
         * @param in the cumulative buffer
         * @return <tt>true</tt> if and only if there's more to decode in the buffer
         *         and you want to have <tt>doDecode</tt> method invoked again.
         *         Return <tt>false</tt> if remaining data is not enough to decode,
         *         then this method will be invoked again when more data is cumulated.
         * @throws Exception if cannot decode <tt>in</tt>.
         */
        protected abstract bool DoDecode(ISession session, ByteBuffer input,
                                         IProtocolDecoderOutput output);

        /**
         * Releases the cumulative buffer used by the specified <tt>session</tt>.
         * Please don't forget to call <tt>super.dispose( session )</tt> when
         * you override this method.
         */
        public override void Dispose(ISession session )
        {
            ByteBuffer buf = (ByteBuffer) session.GetAttribute(BUFFER);
            if (buf != null)
            {
                buf.Release();
                session.RemoveAttribute(BUFFER);
            }
        }
    }
}
