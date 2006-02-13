using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using jpmorgan.mina.common;
using jpmorgan.mina.transport.socket.networkstream;
using log4net;
using OpenAMQ.XMS.Client.Protocol;

namespace OpenAMQ.XMS.Client.Transport
{
    internal class TransportConnection
    {
        private static ILog _logger = LogManager.GetLogger(typeof(TransportConnection));        

        private AMQProtocolHandler _protocolHandler;
        
        internal TransportConnection(AMQProtocolHandler protocolHandler)
        {
            _protocolHandler = protocolHandler;
        }
        
        internal void Connect(string host, int port)
        {
            SocketConnector connector = new SocketConnector();
            
            _logger.Info("Attempting connection to " + host + " port " + port);

            ConnectFuture future = connector.Connect(host, port, _protocolHandler);
            
        }                
    }
}
