package org.openamq.client.handler;

import org.openamq.AMQException;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.ConnectionSecureOkBody;
import org.openamq.framing.ConnectionSecureBody;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.protocol.AMQMethodEvent;

import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;

public class ConnectionSecureMethodHandler implements StateAwareMethodListener
{
    private static final ConnectionSecureMethodHandler _instance = new ConnectionSecureMethodHandler();

    public static ConnectionSecureMethodHandler getInstance()
    {
        return _instance;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        SaslClient client = evt.getProtocolSession().getSaslClient();
        if (client == null)
        {
            throw new AMQException("No SASL client set up - cannot proceed with authentication");
        }

        ConnectionSecureBody body = (ConnectionSecureBody) evt.getMethod();

        try
        {
            // Evaluate server challenge
            byte[] response = client.evaluateChallenge(body.challenge);
            AMQFrame responseFrame = ConnectionSecureOkBody.createAMQFrame(evt.getChannelId(), response);
            evt.getProtocolSession().writeFrame(responseFrame);
        }
        catch (SaslException e)
        {
            throw new AMQException("Error processing SASL challenge: " + e, e);
        }


    }
}
