package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.ConnectionRedirectBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionRedirectMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(ConnectionRedirectMethodHandler.class);

    private static final String DEFAULT_REDIRECT_PORT = "5672";

    private static ConnectionRedirectMethodHandler _handler = new ConnectionRedirectMethodHandler();

    public static ConnectionRedirectMethodHandler getInstance()
    {
        return _handler;
    }

    private ConnectionRedirectMethodHandler()
    {
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        _logger.info("ConnectionRedirect frame received");
        ConnectionRedirectBody method = (ConnectionRedirectBody) evt.getMethod();
        if (method.port == null)
        {
            // we set the default port
            method.port = DEFAULT_REDIRECT_PORT;
        }
        evt.getProtocolSession().failover(method.hostname, Integer.parseInt(method.port));
    }
}
