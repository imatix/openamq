using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class ConnectionRedirectMethodHandler : IStateAwareMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(ConnectionRedirectMethodHandler));

        private const int DEFAULT_REDIRECT_PORT = 5672;

        private static ConnectionRedirectMethodHandler _handler = new ConnectionRedirectMethodHandler();

        public static ConnectionRedirectMethodHandler GetInstance()
        {
            return _handler;
        }

        private ConnectionRedirectMethodHandler()
        {
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
            _logger.Info("ConnectionRedirect frame received");
            ConnectionRedirectBody method = (ConnectionRedirectBody) evt.Method;

            // the host is in the form hostname:port with the port being optional
            int portIndex = method.Host.IndexOf(':');
            String host;
            int port;
            if (portIndex == -1)
            {
                host = method.Host;
                port = DEFAULT_REDIRECT_PORT;
            }
            else
            {
                host = method.Host.Substring(0, portIndex);
                port = Int32.Parse(method.Host.Substring(portIndex + 1));
            }
            evt.ProtocolSession.Failover(host, port);
        }
    }

}
