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
            ByteBuffer buf = (ByteBuffer) writeRequest.Message;
            byte[] payload = buf.ToByteArray();
            // start the asynchronous write operation
            int pos = buf.Position;
            s.Socket.BeginSend(payload, pos, buf.Limit - pos, 0, new AsyncCallback(SendCompleted), s);
        }

        protected override void DoClose(CloseFuture closeFuture)
        {
            SocketSessionImpl s = (SocketSessionImpl)_session;
            //s.Socket.Disconnect(true);
            closeFuture.Event.Set();
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
