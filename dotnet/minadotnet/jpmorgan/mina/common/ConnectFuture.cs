using System;

namespace jpmorgan.mina.common
{
    /// An {@link IoFuture} for asynchronous connect requests.
    ///
    /// <h3>Example</h3>
    /// <pre>
    /// IConnector connector = ...;
    /// ConnectFuture future = connector.connect(...);
    /// future.join(); // Wait until the connection attempt is finished.
    /// IoSession session = future.getSession();
    /// session.write(...);
    /// </pre>
    public class ConnectFuture : IoFuture
    {
        ///
        /// Returns a new {@link ConnectFuture} which is already marked as 'failed to connect'.
        ///
        public static ConnectFuture newFailedFuture(Exception exception)
        {
            ConnectFuture failedFuture = new ConnectFuture();
            failedFuture.Exception = exception;
            return failedFuture;
        }
        
        public ConnectFuture()
        {
        }
        
        ///
        /// Creates a new instance which uses the specified object as a callback.
        ///
        public ConnectFuture(Callback callback) : base(callback)
        {            
        }

        ///
        /// Returns {@link IoSession} which is the result of connect operation.
        ///
        /// @return <tt>null</tt> if the connect operation is not finished yet
        /// @throws IOException if connection attempt failed by an exception         
        public ISession Session
        {
            get
            {
                object v = Value;
                if (v is Exception)
                {
                    //ExceptionUtil.throwException( ( Throwable ) v );
                    //throw new InternalError(); // this cannot be executed
                    throw (Exception) v;
                }
                else
                {
                    return (ISession) v;
                }
            }

            set
            {
                Value = value;
            }
        }

        ///
        /// Returns <tt>true</tt> if the connect operation is finished successfully.
        ///
        public bool Connected
        {
            get
            {
                return Value is ISession;
            }
        }
                
        ///
        /// This method is invoked by MINA internally.  Please do not call this method
        /// directly.
        /// 
        public Exception Exception
        {
            set
            {
                Value = value;
            }
        }
    }
}
