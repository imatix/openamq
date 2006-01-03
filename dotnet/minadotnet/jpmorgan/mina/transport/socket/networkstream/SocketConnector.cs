using System;
using System.Net;
using System.Net.Sockets;
using jpmorgan.mina.common;
using jpmorgan.mina.transport.socket.networkstream.support;

namespace jpmorgan.mina.transport.socket.networkstream
{
    internal class SocketConnector : IConnector, ISocketSessionManager
    {
        private int _processors;

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

        public ConnectFuture connect(IPEndPoint address, IHandler handler)
        {
            return connect(address, handler, new NOOPIoFilterChainBuilder());
        }

        public ConnectFuture connect(IPEndPoint address, IHandler handler, IFilterChainBuilder filterChainBuilder)
        {
            return connect(address, null, handler, filterChainBuilder);
        }

        public ConnectFuture connect(IPEndPoint address, IPEndPoint localAddress, IHandler handler)
        {
            return connect(address, localAddress, handler, new NOOPIoFilterChainBuilder());
        }

        public ConnectFuture connect(IPEndPoint address, IPEndPoint localAddress, IHandler handler, IFilterChainBuilder filterChainBuilder)
        {
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            SocketSession session = newSession(sock, handler, filterChainBuilder);
            sock.BeginConnect(address, new AsyncCallback(ConnectCallback), session);
            return null; // RG what to do here? Does ConnectFuture make any sense?
        }

        private void ConnectCallback(IAsyncResult result)
        {
            SocketSession session = (SocketSession) result.AsyncState;
            SocketIoProcessor.StartReceive(session);            
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

        private SocketSession newSession(Socket socket, IHandler handler, IFilterChainBuilder builder)
        {
            return new SocketSessionImpl(socket, handler);
        }
    }
}
