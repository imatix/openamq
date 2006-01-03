using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace jpmorgan.mina.common
{
    ///
    /// Accepts incoming connection, communicates with clients, and fires events to
    /// {@link IoHandler}s.
    ///
    /// Please refer to
    /// <a href="../../../../../xref-examples/org/apache/mina/examples/echoserver/Main.html">EchoServer</a>
    /// example. 
    /// <p>
    /// You should bind to the desired socket address to accept incoming
    /// connections, and then events for incoming connections will be sent to
    /// the specified default {@link IoHandler}.
    /// <p>
    /// Threads accept incoming connections start automatically when
    /// {@link #bind(SocketAddress, IoHandler)} is invoked, and stop when all
    /// addresses are unbound.    
    public interface IAcceptor
    {
        ///
        /// Binds to the specified <code>address</code> and handles incoming
        /// connections with the specified <code>handler</code>.
        /// 
        /// @throws IOException if failed to bind            
        void Bind(IPEndPoint address, IHandler handler);

        ///
        /// Binds to the specified <code>address</code> and handles incoming
        /// connections with the specified <code>handler</code>.
        ///
        /// @param filterChainBuilder
        ///     an {@link IoFilterChainBuilder} that will modify the
        ///     {@link IoFilterChain} of a newly created {@link IoSession}
        /// @throws IOException if failed to bind    
        void Bind(IPEndPoint address, IHandler handler, IFilterChainBuilder filterChainBuilder);

        ///
        /// Unbinds from the specified <code>address</code> and disconnects all clients
        /// connected there.    
        void Unbind(IPEndPoint address);
    
        ///
        /// (Optional) Returns an {@link IoSession} that is bound to the specified
        /// <tt>localAddress</tt> and <tt>remoteAddress</tt> which reuses
        /// the <tt>localAddress</tt> that is already bound by {@link IoAcceptor}
        /// via {@link #bind(SocketAddress, IoHandler)}.
        /// <p>
        /// This operation is optional.  Please throw {@link UnsupportedOperationException}
        /// if the transport type doesn't support this operation.  This operation is
        /// usually implemented for connectionless transport types.
        /// 
        /// @throws UnsupportedOperationException if this operation is not supported
        /// @throws IllegalArgumentException if the specified <tt>localAddress</tt> is
        /// not bound yet. (see {@link #bind(SocketAddress, IoHandler)})
        ISession NewSession(IPEndPoint remoteAddress, IPEndPoint localAddress);
    
        // TODO: DIRMINA-93
        ///
        /// Returns <tt>true</tt> if and only if all clients are disconnected
        /// when this acceptor unbinds the related local address.
        ///
        //boolean isDisconnectClientsOnUnbind();
    
        ///
        /// Sets whether all clients are disconnected from this acceptor unbinds the
        /// related local address.  The default value is <tt>true</tt>.
        ///
        //void setDisconnectClientsOnUnbind( boolean disconnectClientsOnUnbind );
    }
}
