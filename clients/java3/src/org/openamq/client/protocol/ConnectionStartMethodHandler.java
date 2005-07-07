package org.openamq.client.protocol;

import java.io.UnsupportedEncodingException;
import java.util.StringTokenizer;

import org.apache.log4j.Logger;
import org.apache.mina.protocol.ProtocolSession;
import org.openamq.client.AMQException;
import org.openamq.client.framing.ConnectionStartBody;
import org.openamq.client.framing.ConnectionStartOkBody;
import org.openamq.client.framing.FieldTable;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;

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
            response.put("LOGIN", ps.getUsername());
            response.put("PASSWORD", ps.getPassword());
            
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
            ps.writeFrame(ConnectionStartOkBody.createAMQFrame(evt.getChannelId(), selectedMechanism, encodedResponse,
                                                selectedLocale));
        }
        catch (UnsupportedEncodingException e)
        {
            throw new AMQException(_log, "Unable to decode data: " + e, e);
        }
    }

}
