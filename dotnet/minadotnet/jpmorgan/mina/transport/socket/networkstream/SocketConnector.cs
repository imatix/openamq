using System;
using System.Net;
using System.Net.Sockets;
using jpmorgan.mina.common;
using jpmorgan.mina.transport.socket.networkstream.support;

namespace jpmorgan.mina.transport.socket.networkstream
{
    public class SocketConnector : IConnector, ISocketSessionManager
    {
        private int _processors;

        private class AsyncResultState
        {
            public SocketSession Session;
            public ConnectFuture Future;
            public IFilterChainBuilder FilterChainBuilder;
        }
        #region ISocketSessionManager Members

        public int Processors
        {
            get
            {
                return _processors;
            }
            set
            {
                _processors = value;
            }
        }

        #endregion

        #region IConnector Members

        /// <summary>
        ///  Connect
        /// </summary>
        /// <param name="host"></param>
        /// <param name="port"></param>
        /// <param name="handler"></param>
        /// <returns></returns>
        /// <exception cref="SocketException">when the host cannot be found in DNS</exception>                
        public ConnectFuture Connect(string host, int port, IHandler handler)
        {
            IPAddress[] addresses = Dns.GetHostAddresses(host);
            if (addresses != null && addresses.Length > 0)
            {
                IPEndPoint endpoint = new IPEndPoint(addresses[0], port);
                return Connect(endpoint, handler);
            }
            return null; // should never occur since GetHostAddresses throws when it cannot find the host
        }
        
        public ConnectFuture Connect(IPEndPoint address, IHandler handler)
        {
            return Connect(address, handler, new NOOPIoFilterChainBuilder());
        }

        public ConnectFuture Connect(IPEndPoint address, IHandler handler, IFilterChainBuilder filterChainBuilder)
        {
            return Connect(address, null, handler, filterChainBuilder);
        }

        public ConnectFuture Connect(IPEndPoint address, IPEndPoint localAddress, IHandler handler)
        {
            return Connect(address, localAddress, handler, new NOOPIoFilterChainBuilder());
        }

        public ConnectFuture Connect(IPEndPoint address, IPEndPoint localAddress, IHandler handler, IFilterChainBuilder filterChainBuilder)
        {
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);            
            ConnectFuture cf = new ConnectFuture();
            SocketSession session = newSession(sock, handler);
            AsyncResultState state = new AsyncResultState();
            state.Future = cf;
            state.Session = session;
            state.FilterChainBuilder = filterChainBuilder;
            sock.BeginConnect(address, new AsyncCallback(ConnectCallback), state);
            return cf;
        }

        private void ConnectCallback(IAsyncResult result)
        {
            AsyncResultState state = (AsyncResultState) result.AsyncState;
            SocketSession session = state.Session;
            state.FilterChainBuilder.BuildFilterChain(session.FilterChain);
            // fire the event so listeners can react accordingly and set up any decoders etc.
            // the events are taken directly from the Java version and don't map terribly well to the .NET model
            // but we will keep them for now
            session.FilterChain.SessionCreated();
            // start the asynchronous listener process
            SocketIoProcessor.StartReceive(session);
            session.FilterChain.SessionOpened();
            // this marks the io future as complete
            state.Future.Session = session;            
        }

        public int ConnectionTimeout
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public long ConnectionTimeoutMillis
        {
            get
            {
                throw new Exception("The method or operation is not implemented.");
            }
            set
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        #endregion

        private SocketSession newSession(Socket socket, IHandler handler)
        {
            return new SocketSessionImpl(socket, handler);
        }
    }
}
