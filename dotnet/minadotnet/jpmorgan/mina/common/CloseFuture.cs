using System;

namespace jpmorgan.mina.common
{
    public class CloseFuture : IoFuture
    {
        ///
        /// Creates a new instance.
        ///
        public CloseFuture()
        {
        }

        public CloseFuture(Callback callback)
            : base(callback)
        {
        }
    }
}
