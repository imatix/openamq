package org.openamq.client.protocol;

import org.openamq.client.AMQException;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.ConnectionOpenBody;
import org.openamq.client.framing.ConnectionStartOkBody;
import org.openamq.client.framing.ConnectionTuneBody;
import org.openamq.client.framing.ConnectionTuneOkBody;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.apache.log4j.Logger;

import java.util.Iterator;
import java.util.Map;

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
            session.setConnectionTuneParameters(params);
        }
                    
        params.setFrameMax(frame.frameMax);
        params.setChannelMax(frame.channelMax);
        params.setHearbeat(frame.heartbeat);                           

        session.writeFrame(ConnectionTuneOkBody.createAMQFrame(evt.getChannelId(), frame.frameMax, frame.channelMax, frame.heartbeat));
        session.writeFrame(ConnectionOpenBody.createAMQFrame(evt.getChannelId(), session.getClientID()));
    }
}
