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

    private static final int DEFAULT_REDIRECT_PORT = 5672;

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

        // the host is in the form hostname:port with the port being optional
        int portIndex = method.host.indexOf(':');
        String host;
        int port;
        if (portIndex == -1)
        {
            host = method.host;
            port = DEFAULT_REDIRECT_PORT;
        }
        else
        {
            host = method.host.substring(0, portIndex);
            port = Integer.parseInt(method.host.substring(portIndex + 1));
        }
        evt.getProtocolSession().failover(host, port);
    }
}
