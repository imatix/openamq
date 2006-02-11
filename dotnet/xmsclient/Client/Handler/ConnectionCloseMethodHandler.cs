using System;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class ConnectionCloseMethodHandler : IStateAwareMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(ConnectionCloseMethodHandler));

        private static ConnectionCloseMethodHandler _handler = new ConnectionCloseMethodHandler();

        public static ConnectionCloseMethodHandler getInstance()
        {
            return _handler;
        }

        private ConnectionCloseMethodHandler()
        {
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
            _logger.Debug("ConnectionClose frame received");
            ConnectionCloseBody method = (ConnectionCloseBody) evt.Method;

            int errorCode = method.ReplyCode;
            String reason = method.ReplyText;

            // TODO: check whether channel id of zero is appropriate
            evt.ProtocolSession.WriteFrame(ConnectionCloseOkBody.CreateAMQFrame(0));
            stateManager.ChangeState(AMQState.CONNECTION_CLOSED);
            if (errorCode != 200)
            {
                _logger.Debug("Connection close received with error code " + errorCode);
                throw new AMQConnectionClosedException(errorCode, "Error: " + reason);
            }

            // this actually closes the connection in the case where it is not an error.
            evt.ProtocolSession.CloseProtocolSession();
        }
    }
}
