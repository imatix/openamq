using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class ConnectionSecureMethodHandler : IStateAwareMethodListener
    {
        private static readonly ConnectionSecureMethodHandler _instance = new ConnectionSecureMethodHandler();

        public static ConnectionSecureMethodHandler getInstance()
        {
            return _instance;
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
            AMQFrame response = ConnectionSecureOkBody.CreateAMQFrame(evt.ChannelId, null);
            evt.ProtocolSession.WriteFrame(response);
        }
    }
}
