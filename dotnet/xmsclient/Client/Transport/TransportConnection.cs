using System;
using System.Net.Sockets;
using System.Threading;
using log4net;
using OpenAMQ.XMS.Client.Protocol;

namespace OpenAMQ.XMS.Client.Transport
{
    internal class TransportConnection
    {
        private static ILog _logger = LogManager.GetLogger(typeof(TransportConnection));

        private ManualResetEvent _resetEvent = new ManualResetEvent(false);

        private AMQProtocolHandler _protocolHandler;
        
        internal TransportConnection(AMQProtocolHandler protocolHandler)
        {
            _protocolHandler = protocolHandler;
        }
        
        internal void Connect(string host, int port)
        {
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _logger.Info("Attempting connection to " + host + " port " + port);

            sock.BeginConnect(host, port, new AsyncCallback(ConnectCallback), sock);        
            // wait for connection to complete
            _resetEvent.WaitOne();        
        }
        
        private void ConnectCallback(IAsyncResult ar)
        {
            Socket sock = (Socket) ar.AsyncState;
            sock.EndConnect(ar);
            _logger.Info("Connection completed to " + sock.RemoteEndPoint.ToString());
            _resetEvent.Set();
        }
    }
}
