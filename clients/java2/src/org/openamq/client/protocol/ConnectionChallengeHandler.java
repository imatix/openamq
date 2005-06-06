package org.openamq.client.protocol;

import org.apache.mina.common.Session;
import org.openamq.client.AMQException;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.FieldTable;
import org.openamq.client.state.*;

import java.util.StringTokenizer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionChallengeHandler implements StateTransitionHandler
{
    public StateAndFrame messageReceived(AMQState state, Object message, Session session) throws AMQException
    {
        Connection.Challenge frame = (Connection.Challenge) message;

        if (session.getAttribute(ProtocolSessionAttributes.SECURITY_MECHANISM) == null)
        {
            // TODO: check protocl version server supports

            StringTokenizer tokenizer = new StringTokenizer(frame.mechanisms, " ");
            String selectedMechanism = null;
            while (tokenizer.hasMoreTokens())
            {
                String mechanism = tokenizer.nextToken();
                if (mechanism.equals("PLAIN"))
                {
                    selectedMechanism = mechanism;
                    break;
                }
            }

            if (selectedMechanism == null)
            {
                // TODO: decide how to handle things like this - exception or another state?
                throw new AMQException("No supported security mechanism found");
            }
            else
            {
                Connection.Response response = new Connection.Response();
                response.mechanism = selectedMechanism;
                response.responses = new FieldTable();
                response.responses.put("LOGIN", session.getAttribute(ProtocolSessionAttributes.USERNAME));
                response.responses.put("PASSWORD", session.getAttribute(ProtocolSessionAttributes.PASSWORD));
                return new StateAndFrame(state, response);
            }
        }
        else
        {
            throw new AMQException("PLAIN security mechanism not expecting server challenge.");
        }
    }
}
