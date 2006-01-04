package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.protocol.AMQProtocolSession;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.ConnectionOpenBody;
import org.openamq.framing.ConnectionTuneBody;
import org.openamq.framing.ConnectionTuneOkBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionTuneMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(ConnectionTuneMethodHandler.class);

    private static final ConnectionTuneMethodHandler _instance = new ConnectionTuneMethodHandler();

    public static ConnectionTuneMethodHandler getInstance()
    {
        return _instance;
    }

    private ConnectionTuneMethodHandler()
    {
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        _logger.debug("ConnectionTune frame received");
        ConnectionTuneBody frame = (ConnectionTuneBody) evt.getMethod();
        AMQProtocolSession session = evt.getProtocolSession();

        ConnectionTuneParameters params = session.getConnectionTuneParameters();
        if (params == null)
        {
            params = new ConnectionTuneParameters();
        }

        params.setFrameMax(frame.frameMax);
        params.setFrameMax(65535);
        //params.setChannelMax(frame.channelMax);
        params.setHearbeat(frame.heartbeat);
        session.setConnectionTuneParameters(params);

        stateManager.changeState(AMQState.CONNECTION_NOT_OPENED);
//        session.writeFrame(ConnectionTuneOkBody.createAMQFrame(evt.getChannelId(), frame.channelMax, frame.frameMax,
//                                                               frame.heartbeat));
        session.writeFrame(ConnectionTuneOkBody.createAMQFrame(evt.getChannelId(), frame.channelMax, 65535,
                                                               frame.heartbeat));
        session.writeFrame(ConnectionOpenBody.createAMQFrame(evt.getChannelId(),
                                                             null, session.getAMQConnection().getVirtualPath(), null,
                                                             true));
    }
}
