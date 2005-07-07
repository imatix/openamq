package org.openamq.client.protocol;

import javax.jms.JMSException;

import org.apache.log4j.Logger;
import org.openamq.client.AMQException;
import org.openamq.client.framing.ConnectionOpenOkBody;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;

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
        try
        {
            session.setClientID(method.clientId);
        }
        catch (JMSException e)
        {
            // never occurs
        }
        stateManager.changeState(AMQState.CONNECTION_OPEN);
    }

}
