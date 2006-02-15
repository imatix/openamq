using System;
using System.Net.Sockets;
using jpmorgan.mina.common;
using log4net;

namespace jpmorgan.mina.transport.socket.networkstream.support
{
    internal class SocketIoProcessor
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(SocketIoProcessor));
        
        public static void ReceiveCompleted(IAsyncResult result)
        {
            SocketSession session = (SocketSession)result.AsyncState;
            int bytesRead = session.Socket.EndReceive(result);
            session.Buffer.Limit = bytesRead;
            if (bytesRead > 0)
            {
                session.FilterChain.MessageReceived(session.Buffer);
            }
            // register to receive data again and the process repeats
            StartReceive(session);
        }

        public static void SendCompleted(IAsyncResult result)
        {
            SocketSession session = (SocketSession)result.AsyncState;
        }

        public static void StartReceive(SocketSession session)
        {
            int bufSize = session.ReceiveBufferSize;
            session.Buffer = ByteBuffer.Allocate(bufSize);
            byte[] buffer = session.Buffer.ToByteArray();
            try
            {
                session.Socket.BeginReceive(buffer, 0, buffer.Length, SocketFlags.None,
                                            new AsyncCallback(ReceiveCompleted), session);
            }
            catch (SocketException e)
            {
                _logger.Error("Socket error " + e, e);
                // TODO: what to do now?
            }
        }
    }
}
