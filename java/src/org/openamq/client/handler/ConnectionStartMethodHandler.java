package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.Version;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.protocol.AMQProtocolSession;
import org.openamq.client.security.AMQCallbackHandler;
import org.openamq.client.security.CallbackHandlerRegistry;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.ConnectionStartBody;
import org.openamq.framing.ConnectionStartOkBody;
import org.openamq.framing.FieldTable;

import javax.security.sasl.Sasl;
import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;
import java.io.UnsupportedEncodingException;
import java.util.HashSet;
import java.util.StringTokenizer;

public class ConnectionStartMethodHandler implements StateAwareMethodListener
{

    private static final Logger _log = Logger.getLogger(ConnectionStartMethodHandler.class);

    private static final ConnectionStartMethodHandler _instance = new ConnectionStartMethodHandler();

    private static final Version _clientVersion = new Version();

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
            // the mechanism we are going to use
            String mechanism;
            if (body.mechanisms == null)
            {
                throw new AMQException("mechanism not specified in ConnectionStart method frame");
            }
            else
            {
                mechanism = chooseMechanism(body.mechanisms);
            }

            if (mechanism == null)
            {
                throw new AMQException("No supported security mechanism found, passed: " + new String(body.mechanisms));
            }

            final AMQProtocolSession ps = evt.getProtocolSession();
            byte[] saslResponse;
            try
            {
                SaslClient sc = Sasl.createSaslClient(new String[]{mechanism},
                                                      null, "AMQP", "localhost",
                                                      null, createCallbackHandler(mechanism, ps));
                if (sc == null)
                {
                    throw new AMQException("Client SASL configuration error: no SaslClient could be created for mechanism " +
                                           mechanism + ". Please ensure all factories are registered. See DynamicSaslRegistrar for " +
                                           " details of how to register non-standard SASL client providers.");
                }
                ps.setSaslClient(sc);
                saslResponse = (sc.hasInitialResponse() ? sc.evaluateChallenge(new byte[0]) : null);
            }
            catch (SaslException e)
            {
                ps.setSaslClient(null);
                throw new AMQException("Unable to create SASL client: " + e, e);
            }

            if (body.locales == null)
            {
                throw new AMQException("Locales is not defined in Connection Start method");
            }
            final String locales = new String(body.locales, "utf8");
            final StringTokenizer tokenizer = new StringTokenizer(locales, " ");
            String selectedLocale = null;
            if (tokenizer.hasMoreTokens())
            {
                selectedLocale = tokenizer.nextToken();
            }
            else
            {
                throw new AMQException("No locales sent from server, passed: " + locales);
            }

            stateManager.changeState(AMQState.CONNECTION_NOT_TUNED);
            FieldTable clientProperties = new FieldTable();
            clientProperties.put("instance", ps.getClientID());
            clientProperties.put("product", _clientVersion.getClientProduct());
            clientProperties.put("version", _clientVersion.getClientVersion());
            clientProperties.put("platform", getFullSystemInfo());
            clientProperties.put("copyright", _clientVersion.getClientCopyright());
            ps.writeFrame(ConnectionStartOkBody.createAMQFrame(evt.getChannelId(), clientProperties, mechanism,
                                                               saslResponse, selectedLocale));
        }
        catch (UnsupportedEncodingException e)
        {
            throw new AMQException(_log, "Unable to decode data: " + e, e);
        }
    }

    private String getFullSystemInfo()
    {
        StringBuffer fullSystemInfo = new StringBuffer();
        fullSystemInfo.append(System.getProperty("java.runtime.name"));
        fullSystemInfo.append(", " + System.getProperty("java.runtime.version"));
        fullSystemInfo.append(", " + System.getProperty("java.vendor"));
        fullSystemInfo.append(", " + System.getProperty("os.arch"));
        fullSystemInfo.append(", " + System.getProperty("os.name"));
        fullSystemInfo.append(", " + System.getProperty("os.version"));
        fullSystemInfo.append(", " + System.getProperty("sun.os.patch.level"));

        return fullSystemInfo.toString();
    }

    private String chooseMechanism(byte[] availableMechanisms) throws UnsupportedEncodingException
    {
        final String mechanisms = new String(availableMechanisms, "utf8");
        StringTokenizer tokenizer = new StringTokenizer(mechanisms, " ");
        HashSet mechanismSet = new HashSet();
        while (tokenizer.hasMoreTokens())
        {
            mechanismSet.add(tokenizer.nextToken());
        }

        String preferredMechanisms = CallbackHandlerRegistry.getInstance().getMechanisms();
        StringTokenizer prefTokenizer = new StringTokenizer(preferredMechanisms, " ");
        while (prefTokenizer.hasMoreTokens())
        {
            String mech = prefTokenizer.nextToken();
            if (mechanismSet.contains(mech))
            {
                return mech;
            }
        }
        return null;
    }

    private AMQCallbackHandler createCallbackHandler(String mechanism, AMQProtocolSession protocolSession)
            throws AMQException
    {
        Class mechanismClass = CallbackHandlerRegistry.getInstance().getCallbackHandlerClass(mechanism);
        try
        {
            Object instance = mechanismClass.newInstance();
            AMQCallbackHandler cbh = (AMQCallbackHandler) instance;
            cbh.initialise(protocolSession);
            return cbh;
        }
        catch (Exception e)
        {
            throw new AMQException("Unable to create callback handler: " + e, e);
        }
    }
}
