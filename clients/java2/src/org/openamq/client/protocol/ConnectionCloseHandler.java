package org.openamq.client.protocol;

import org.apache.mina.common.Session;
import org.openamq.client.AMQException;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.FieldTable;
import org.openamq.client.state.*;

import java.util.StringTokenizer;
import java.util.Iterator;
import java.util.Map;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionCloseHandler implements StateTransitionHandler
{
    public StateAndFrame messageReceived(AMQState state, Object message, Session session) throws AMQException
    {
        Connection.Close frame = (Connection.Close) message;
        
        frame.replyCode = 200;
        frame.replyText = "Client says au revoir";

        // TODO: actually close connection here

        return new StateAndFrame(AMQState.CONNECTION_CLOSED, frame);
    }
}
