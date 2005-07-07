package org.openamq.client.protocol;

import org.apache.mina.common.Session;
import org.apache.log4j.Logger;
import org.openamq.client.AMQException;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.ConnectionCloseBody;
import org.openamq.client.framing.ConnectionCloseOkBody;
import org.openamq.client.framing.FieldTable;
import org.openamq.client.state.*;

import java.util.StringTokenizer;
import java.util.Iterator;
import java.util.Map;

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
        if (errorCode == 200)
        {
            stateManager.changeState(AMQState.CONNECTION_CLOSED);
        }
        else
        {
            throw new AMQException(errorCode, "Error: " + reason);
        }


        // this actually closes the connection
        evt.getProtocolSession().closeProtocolSession();
    }
}
