using System;
using jpmorgan.mina.common;
using jpmorgan.mina.common.support;
using log4net;

namespace jpmorgan.mina.transport.socket.networkstream.support
{
    public class SocketFilterChain : AbstractFilterChain
    {
        public SocketFilterChain(ISession session) : base(session)
        {            
        }

        protected override void DoWrite(WriteRequest writeRequest)
        {
            SocketSessionImpl s = (SocketSessionImpl) _session;
            byte[] payload = ((ByteBuffer) writeRequest.Message).ToByteArray();
            // start the asynchronous write operation
            s.Socket.BeginSend(payload, 0, payload.Length, 0, new AsyncCallback(SendCompleted), s);
        }

        protected override void DoClose(CloseFuture closeFuture)
        {
            throw new NotImplementedException();
        }
        
        private void SendCompleted(IAsyncResult result)
        {
            SocketSessionImpl s = (SocketSessionImpl) result.AsyncState;
            try
            {
                s.Socket.EndSend(result);
            }
            catch (Exception e)
            {
                ExceptionCaught(e);
            }
        }
    }
}
