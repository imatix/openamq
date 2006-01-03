using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using jpmorgan.mina.common;

namespace jpmorgan.mina.transport.socket.networkstream
{
    internal class SocketAcceptor : ISocketSessionManager, IAcceptor
    {

        #region ISocketSessionManager Members

        public int Processors
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

        #region IoAcceptor Members

        public void Bind(IPEndPoint address, IHandler handler)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void Bind(IPEndPoint address, IHandler handler, IFilterChainBuilder filterChainBuilder)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void Unbind(IPEndPoint address)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public ISession NewSession(IPEndPoint remoteAddress, IPEndPoint localAddress)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
}
