using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Message;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class BasicDeliverMethodHandler : IStateAwareMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(BasicDeliverMethodHandler));

        private static readonly BasicDeliverMethodHandler _instance = new BasicDeliverMethodHandler();

        public static BasicDeliverMethodHandler getInstance()
        {
            return _instance;
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
            UnprocessedMessage msg = new UnprocessedMessage();
            msg.DeliverBody = (BasicDeliverBody) evt.Method;
            msg.ChannelId = evt.ChannelId;
            _logger.Debug("New JmsDeliver method received");
            evt.ProtocolSession.UnprocessedMessageReceived(msg);
        }
    }
}
