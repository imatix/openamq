using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.common
{
    public class WriteRequest
    {
        private static readonly WriteFuture UNUSED_FUTURE = new WriteFuture();

        private readonly object message;
        private readonly WriteFuture future;

        /**
         * Creates a new instance without {@link WriteFuture}.  You'll get
         * an instance of {@link WriteFuture} even if you called this constructor
         * because {@link #getFuture()} will return a bogus future.
         */
        public WriteRequest(object message) : this(message, null)
        {            
        }

        /**
         * Creates a new instance with {@link WriteFuture}.
         */
        public WriteRequest(object message, WriteFuture future)
        {
            if (message == null)
            {
                throw new ArgumentNullException("message");
            }

            if (future == null)
            {
                future = UNUSED_FUTURE;
            }

            this.message = message;
            this.future = future;
        }

        /**
         * Returns {@link WriteFuture} that is associated with this write request.
         */
        public WriteFuture Future
        {
            get
            {
                return future;
            }
        }

        /**
         * Returns a message object to be written.
         */
        public object Message
        {
            get
            {
                return message;
            }
        }

        public override string ToString()
        {
            return message.ToString();
        }
    }
}
