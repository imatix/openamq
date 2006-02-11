using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Message;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class BasicReturnMethodHandler : IStateAwareMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(BasicReturnMethodHandler));

        private static readonly BasicReturnMethodHandler _instance = new BasicReturnMethodHandler();

        public static BasicReturnMethodHandler GetInstance()
        {
            return _instance;
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt) 
        {
            _logger.Debug("New JmsBounce method received");
            UnprocessedMessage msg = new UnprocessedMessage();
            msg.DeliverBody = null;
            msg.BounceBody = (BasicReturnBody) evt.Method;
            msg.ChannelId = evt.ChannelId;

            evt.ProtocolSession.UnprocessedMessageReceived(msg);
        }
    }
}
