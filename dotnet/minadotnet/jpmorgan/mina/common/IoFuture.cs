using System;
using System.Threading;

namespace jpmorgan.mina.common
{
    ///
    /// Represents the result of an ashynchronous I/O operation.
    ///  
    public class IoFuture
    {
        protected readonly ManualResetEvent _resetEvent;
        protected object _result;
        protected Callback _callback;
        protected bool _ready;

        ///
        /// Something interested in being notified when the result
        /// of an {@link IoFuture} becomes available.
        ///
        public delegate void Callback(IoFuture future, object result);        

        ///
        /// Creates a new instance.
        ///
        public IoFuture()
        {
            _resetEvent = new ManualResetEvent(false);
        }

        ///
        /// Creates a new instance which uses the specified object as a lock.
        ///
        public IoFuture(Callback callback) : this()
        {
            if (callback == null)
            {
                throw new ArgumentNullException("callback");
            }
            _callback = callback;
        }
        
        ///
        /// Returns the lock object this future acquires.
        ///
        public ManualResetEvent Event
        {
            get            
            {
                return _resetEvent;
            }
        }

        ///
        /// Wait for the asynchronous operation to end.
        ///
        public void Join()
        {
            _resetEvent.WaitOne();
        }

        /**
         * Wait for the asynchronous operation to end with the specified timeout.
         * 
         * @return <tt>true</tt> if the operation is finished.
         */
        public bool Join(int timeoutInMillis)
        {
            return _resetEvent.WaitOne(timeoutInMillis, false);
        }

        ///
        /// Returns if the asynchronous operation is finished.
        ///
        public bool Ready
        {
            get
            {                
                return _ready;             
            }
        }        

        ///
        /// Returns the result of the asynchronous operation.
        ///
        protected object Value
        {
            get
            {                
                return _result;        
            }

            set
            {                
                _result = value;
                _ready = true;
                _resetEvent.Set();
                InvokeCallback();
            }
        }
              
        private void InvokeCallback()
        {
            if (_callback != null)
            {
                _callback(this, _result);
            }            
        }        
    }
}