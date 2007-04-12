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
import org.openamq.framing.AMQFrame;

public class ConnectionTuneMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(ConnectionTuneMethodHandler.class);

    private static final ConnectionTuneMethodHandler _instance = new ConnectionTuneMethodHandler();

    public static ConnectionTuneMethodHandler getInstance()
    {
        return _instance;
    }

    protected ConnectionTuneMethodHandler()
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
        params.setChannelMax(frame.channelMax);
        params.setHeartbeat(Integer.getInteger("amqj.heartbeat.delay", frame.heartbeat));
        session.setConnectionTuneParameters(params);

        stateManager.changeState(AMQState.CONNECTION_NOT_OPENED);
        session.writeFrame(createTuneOkFrame(evt.getChannelId(), params));
        session.writeFrame(createConnectionOpenFrame(evt.getChannelId(), session.getAMQConnection().getVirtualPath(), null, true));
    }

    protected AMQFrame createConnectionOpenFrame(int channel, String path, String capabilities, boolean insist)
    {
        return ConnectionOpenBody.createAMQFrame(channel, path, capabilities, insist);
    }

    protected AMQFrame createTuneOkFrame(int channel, ConnectionTuneParameters params)
    {
        return ConnectionTuneOkBody.createAMQFrame(channel, params.getChannelMax(), params.getFrameMax(), params.getHeartbeat());
    }
}
