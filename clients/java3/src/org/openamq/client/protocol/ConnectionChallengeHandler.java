package org.openamq.client.protocol;

import org.openamq.client.AMQException;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.FieldTable;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareFrameListener;

import java.util.StringTokenizer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionChallengeHandler implements StateAwareFrameListener
{
    private static final ConnectionChallengeHandler _instance = new ConnectionChallengeHandler();
    
    public static ConnectionChallengeHandler getInstance()
    {
        return _instance;
    }

    private ConnectionChallengeHandler()
    {
    }

    public void frameReceived(AMQStateManager stateManager, FrameEvent evt) throws AMQException
    {
        final Connection.Challenge frame = (Connection.Challenge) evt.getFrame();
        final AMQProtocolSession protocolSession = evt.getProtocolSession();
        if (protocolSession.getSecurityMechanism() == null)
        {
            // TODO: check protocol version server supports

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
                throw new AMQException("No supported security mechanism found");
            }
            else
            {
                Connection.Response response = new Connection.Response();
                response.mechanism = selectedMechanism;
                response.responses = new FieldTable();
                response.responses.put("LOGIN", protocolSession.getUsername());
                response.responses.put("PASSWORD", protocolSession.getPassword());
                protocolSession.writeFrame(response);
            }
        }
        else
        {
            throw new AMQException("PLAIN security mechanism not expecting server challenge.");
        }
    }
}
