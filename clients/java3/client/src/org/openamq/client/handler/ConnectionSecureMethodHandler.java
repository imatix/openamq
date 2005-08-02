package org.openamq.client.handler;

import org.openamq.AMQException;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.ConnectionSecureOkBody;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.protocol.AMQMethodEvent;

public class ConnectionSecureMethodHandler implements StateAwareMethodListener
{
    private static final ConnectionSecureMethodHandler _instance = new ConnectionSecureMethodHandler();

    public static ConnectionSecureMethodHandler getInstance()
    {
        return _instance;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        AMQFrame response = ConnectionSecureOkBody.createAMQFrame(evt.getChannelId(), null);
        evt.getProtocolSession().writeFrame(response);
    }
}
