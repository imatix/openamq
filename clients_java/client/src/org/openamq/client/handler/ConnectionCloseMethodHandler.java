package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.AMQConnectionClosedException;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.ConnectionCloseBody;
import org.openamq.framing.ConnectionCloseOkBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionCloseMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(ConnectionCloseMethodHandler.class);

    private static ConnectionCloseMethodHandler _handler = new ConnectionCloseMethodHandler();

    public static ConnectionCloseMethodHandler getInstance()
    {
        return _handler;
    }

    private ConnectionCloseMethodHandler()
    {
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        _logger.debug("ConnectionClose frame received");
        ConnectionCloseBody method = (ConnectionCloseBody) evt.getMethod();

        int errorCode = method.replyCode;
        String reason = method.replyText;

        // TODO: check whether channel id of zero is appropriate
        evt.getProtocolSession().writeFrame(ConnectionCloseOkBody.createAMQFrame((short)0));
        stateManager.changeState(AMQState.CONNECTION_CLOSED);
        if (errorCode != 200)
        {
            _logger.debug("Connection close received with error code " + errorCode);
            throw new AMQConnectionClosedException(errorCode, "Error: " + reason);
        }

        // this actually closes the connection in the case where it is not an error.
        evt.getProtocolSession().closeProtocolSession();
    }
}
