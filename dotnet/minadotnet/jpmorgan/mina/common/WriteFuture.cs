using System;

namespace jpmorgan.mina.common
{
    public class WriteFuture : IoFuture
    {
        ///
        /// Returns a new {@link WriteFuture} which is already marked as 'written'.         
        public static WriteFuture newWrittenFuture()
        {
            WriteFuture unwrittenFuture = new WriteFuture();
            unwrittenFuture.Value = true;
            return unwrittenFuture;
        }

        ///
        /// Returns a new {@link WriteFuture} which is already marked as 'not written'.
        ///
        public static WriteFuture newNotWrittenFuture()
        {
            WriteFuture unwrittenFuture = new WriteFuture();
            unwrittenFuture.Value = false;
            return unwrittenFuture;
        }
        
        ///
        /// Creates a new instance.         
        public WriteFuture()
        {
        }
        
        ///
        /// Creates a new instance which uses the specified object as a callback
        ///
        public WriteFuture(Callback callback) : base(callback)
        {            
        }               
    }    
}
