using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class ConnectionOpenOkMethodHandler : IStateAwareMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(ConnectionOpenOkMethodHandler));

        private static readonly ConnectionOpenOkMethodHandler _instance = new ConnectionOpenOkMethodHandler();

        public static ConnectionOpenOkMethodHandler GetInstance()
        {
            return _instance;
        }

        private ConnectionOpenOkMethodHandler()
        {
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
            AMQProtocolSession session = evt.ProtocolSession;
            ConnectionOpenOkBody method = (ConnectionOpenOkBody) evt.Method;
            stateManager.ChangeState(AMQState.CONNECTION_OPEN);
        }

    }

}
