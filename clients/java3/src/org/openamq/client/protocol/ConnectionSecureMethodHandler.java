package org.openamq.client.protocol;

import org.openamq.client.AMQException;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;

public class ConnectionSecureMethodHandler implements StateAwareMethodListener
{
    private static final ConnectionSecureMethodHandler _instance = new ConnectionSecureMethodHandler();
    
    public static ConnectionSecureMethodHandler getInstance()
    {
        return _instance;
    }
    
    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        // TODO: support ConnectionSecure
        throw new AMQException("ConnectionSecure currently unsupported by this client");
    }
}
