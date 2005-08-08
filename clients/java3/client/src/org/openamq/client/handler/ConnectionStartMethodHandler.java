package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.protocol.AMQProtocolSession;
import org.openamq.framing.ConnectionStartBody;
import org.openamq.framing.ConnectionStartOkBody;
import org.openamq.framing.FieldTable;

import java.io.UnsupportedEncodingException;
import java.util.StringTokenizer;

public class ConnectionStartMethodHandler implements StateAwareMethodListener
{

    private static final Logger _log = Logger.getLogger(ConnectionStartMethodHandler.class);
    
    private static final ConnectionStartMethodHandler _instance = new ConnectionStartMethodHandler();
    
    public static ConnectionStartMethodHandler getInstance()
    {
        return _instance;
    }

    private ConnectionStartMethodHandler()
    {       
    }
    
    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        ConnectionStartBody body = (ConnectionStartBody) evt.getMethod();
        
        try
        {
            if (body.mechanisms == null)
            {
                throw new AMQException("mechanism not specified in ConnectionStart method frame");
            }
            final String mechanisms = new String(body.mechanisms, "utf8");
            StringTokenizer tokenizer = new StringTokenizer(mechanisms, " ");
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
                throw new AMQException("No supported security mechanism found, passed: " + mechanisms);
            }
            
            FieldTable response = new FieldTable();
            final AMQProtocolSession ps = evt.getProtocolSession();
            response.put("login", ps.getUsername());
            response.put("password", ps.getPassword());

            if (body.locales == null)
            {
                throw new AMQException("Locales is not defined in Connection Start method");
            }
            final String locales = new String(body.locales, "utf8");
            tokenizer = new StringTokenizer(locales, " ");
            String selectedLocale = null;
            if (tokenizer.hasMoreTokens())
            {
                selectedLocale = tokenizer.nextToken();           
            }
            else
            {
                throw new AMQException("No locales sent from server, passed: " + locales);
            }        
                
            byte[] encodedResponse = response.getDataAsBytes();
            stateManager.changeState(AMQState.CONNECTION_NOT_TUNED);
            ps.writeFrame(ConnectionStartOkBody.createAMQFrame(evt.getChannelId(), selectedMechanism, encodedResponse,
                                                selectedLocale));
        }
        catch (UnsupportedEncodingException e)
        {
            throw new AMQException(_log, "Unable to decode data: " + e, e);
        }
    }

}
