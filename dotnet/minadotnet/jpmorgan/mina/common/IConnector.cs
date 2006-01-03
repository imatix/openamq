using System;
using System.Net;

namespace jpmorgan.mina.common
{
    /// Connects to endpoint, communicates with the server, and fires events to
    /// {@link IoHandler}s.
    /// <p>
    /// Please refer to
    /// <a href="../../../../../xref-examples/org/apache/mina/examples/netcat/Main.html">NetCat</a>
    /// example. 
    /// <p>
    /// You should connect to the desired socket address to start communication,
    /// and then events for incoming connections will be sent to the specified
    /// default {@link IoHandler}.
    /// <p>
    /// Threads connect to endpoint start automatically when
    /// {@link #connect(SocketAddress, IoHandler)} is invoked, and stop when all
    /// connection attempts are finished.
    public interface IConnector
    {
        ///
        /// Connects to the specified <code>address</code>.  If communication starts
        /// successfully, events are fired to the specified
        /// <code>handler</code>.
        /// 
        /// @return {@link ConnectFuture} that will tell the result of the connection attempt        
        ConnectFuture connect(IPEndPoint address, IHandler handler);

        ///
        /// Connects to the specified <code>address</code>.  If communication starts
        /// successfully, events are fired to the specified
        /// <code>handler</code>.
        ///
        /// @param filterChainBuilder
        ///            an {@link IoFilterChainBuilder} that will modify the
        ///            {@link IoFilterChain} of a newly created {@link IoSession}
        /// @return {@link ConnectFuture} that will tell the result of the connection attempt         
        ConnectFuture connect(IPEndPoint address, IHandler handler,
                              IFilterChainBuilder filterChainBuilder);

        ///
        /// Connects to the specified <code>address</code>.  If communication starts
        /// successfully, events are fired to the specified
        /// <code>handler</code>.
        /// 
        /// @param localAddress the local address the channel is bound to
        /// @return {@link ConnectFuture} that will tell the result of the connection attempt         
        ConnectFuture connect(IPEndPoint address, IPEndPoint localAddress,
                              IHandler handler);

        ///
        /// Connects to the specified <code>address</code>.  If communication starts
        /// successfully, events are fired to the specified
        /// <code>handler</code>.
        /// 
        /// @param filterChainBuilder
        ///            an {@link IoFilterChainBuilder} that will modify the
        ///            {@link IoFilterChain} of a newly created {@link IoSession}
        /// @return {@link ConnectFuture} that will tell the result of the connection attempt         
        ConnectFuture connect(IPEndPoint address, IPEndPoint localAddress,
                              IHandler handler, IFilterChainBuilder filterChainBuilder);

        ///
        /// Returns the connect timeout in seconds.         
        int ConnectionTimeout
        {
            get;
        }

        ///
        /// Returns the connect timeout in milliseconds.
        long ConnectionTimeoutMillis
        {
            get;
            set;
        }        
    }
}
