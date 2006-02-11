using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class ConnectionTuneMethodHandler : IStateAwareMethodListener
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(ConnectionTuneMethodHandler));

        private static readonly ConnectionTuneMethodHandler _instance = new ConnectionTuneMethodHandler();

        public static ConnectionTuneMethodHandler getInstance()
        {
            return _instance;
        }

        private ConnectionTuneMethodHandler()
        {
        }

        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
            _logger.Debug("ConnectionTune frame received");
            ConnectionTuneBody frame = (ConnectionTuneBody) evt.Method;
            AMQProtocolSession session = evt.ProtocolSession;

            ConnectionTuneParameters parameters = session.ConnectionTuneParameters;
            if (parameters == null)
            {
                parameters = new ConnectionTuneParameters();
            }

            parameters.FrameMax = frame.FrameMax;
            parameters.FrameMax = 65535;
            //params.setChannelMax(frame.channelMax);
            parameters.Heartbeat = frame.Heartbeat;
            session.ConnectionTuneParameters = parameters;

            stateManager.ChangeState(AMQState.CONNECTION_NOT_OPENED);
    //        session.writeFrame(ConnectionTuneOkBody.createAMQFrame(evt.getChannelId(), frame.channelMax, frame.frameMax,
    //                                                               frame.heartbeat));
            session.WriteFrame(ConnectionTuneOkBody.CreateAMQFrame(evt.ChannelId, frame.ChannelMax, 65535,
                                                                   frame.Heartbeat));
            session.WriteFrame(ConnectionOpenBody.CreateAMQFrame(evt.ChannelId,
                                                                 session.AMQConnection.VirtualPath, null,
                                                                 true));
        }
    }
}
