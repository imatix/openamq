using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class ChannelCloseMethodHandler : IStateAwareMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(ChannelCloseMethodHandler));

        private static ChannelCloseMethodHandler _handler = new ChannelCloseMethodHandler();

        public static ChannelCloseMethodHandler GetInstance()
        {
            return _handler;
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
             _logger.Debug("ChannelClose method received");
            ChannelCloseBody method = (ChannelCloseBody) evt.Method;

            int errorCode = method.ReplyCode;
            string reason = method.ReplyText;
            if (_logger.IsDebugEnabled)
            {
                _logger.Debug("Channel close reply code: " + errorCode + ", reason: " + reason);
            }

            AMQFrame frame = ChannelCloseOkBody.CreateAMQFrame(evt.ChannelId);
            evt.ProtocolSession.WriteFrame(frame);
            //if (errorCode != AMQConstant.REPLY_SUCCESS.getCode())
            // HACK
            if (errorCode != 200)
            {
                _logger.Debug("Channel close received with errorCode " + errorCode + ", throwing exception");
                evt.ProtocolSession.AMQConnection.ExceptionReceived(new AMQChannelClosedException(errorCode, "Error: " + reason));
            }
            evt.ProtocolSession.ChannelClosed(evt.ChannelId, errorCode, reason);
        }
    }
}
