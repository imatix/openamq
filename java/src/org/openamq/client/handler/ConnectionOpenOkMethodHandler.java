package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.protocol.AMQProtocolSession;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.ConnectionOpenOkBody;

public class ConnectionOpenOkMethodHandler implements StateAwareMethodListener
{

    private static final Logger _logger = Logger.getLogger(ConnectionOpenOkMethodHandler.class);

    private static final ConnectionOpenOkMethodHandler _instance = new ConnectionOpenOkMethodHandler();

    public static ConnectionOpenOkMethodHandler getInstance()
    {
        return _instance;
    }

    private ConnectionOpenOkMethodHandler()
    {
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        AMQProtocolSession session = evt.getProtocolSession();
        ConnectionOpenOkBody method = (ConnectionOpenOkBody) evt.getMethod();        
        stateManager.changeState(AMQState.CONNECTION_OPEN);
    }

}
