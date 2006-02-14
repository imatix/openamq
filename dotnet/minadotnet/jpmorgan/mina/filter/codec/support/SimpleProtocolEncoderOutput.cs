using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;

namespace jpmorgan.mina.filter.codec.support
{
    public abstract class SimpleProtocolEncoderOutput : IProtocolEncoderOutput
    {
        private readonly Queue _bufferQueue = new Queue();
        
        public SimpleProtocolEncoderOutput()
        {            
        }
        
        public Queue BufferQueue
        {
            get
            {
                return _bufferQueue;
            }
        }
        
        public void Write(ByteBuffer buf)
        {
            _bufferQueue.Enqueue(buf);
        }
        
        public void MergeAll()
        {
            int sum = 0;
            int size = _bufferQueue.Count;
            
            if (size < 2)
            {
                return;
            }
            
            foreach (object o in _bufferQueue)
            {
                ByteBuffer buf = (ByteBuffer) o;
                sum += buf.Remaining;
            }
            
            // allocate a new BB that will contain all fragments
            ByteBuffer newBuf = ByteBuffer.Allocate(sum);
            
            // and merge all
            for (;;)
            {
                ByteBuffer buf = (ByteBuffer) _bufferQueue.Dequeue();
                if (buf == null)
                {
                    break;
                }
                newBuf.Put(buf);
                buf.Release();
            }
            
            newBuf.Flip();
            _bufferQueue.Enqueue(newBuf);
        }
        
        public WriteFuture Flush()
        {
            WriteFuture future = null;
            if (_bufferQueue.Count == 0)
            {
                ByteBuffer buf = ByteBuffer.Allocate(0);
                buf.Flip();
                // write zero bytes
                future = DoFlush(buf); 
            }
            else
            {
                for (;;)
                {
                    ByteBuffer buf = (ByteBuffer) _bufferQueue.Dequeue();
                    if (buf == null)
                    {
                        break;
                    }
                    future = DoFlush(buf);
                }
            }
            
            if (future == null)
            {
                throw new Exception("DoFlush must return a future - currently null");
            }
            return future;
        }

        protected abstract WriteFuture DoFlush(ByteBuffer buf);
    }
}
