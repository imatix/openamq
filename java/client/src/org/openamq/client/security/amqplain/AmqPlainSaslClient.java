package org.openamq.client.security.amqplain;

import org.openamq.framing.FieldTable;

import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.Callback;

/**
 * Implements the "AMQPlain" authentication protocol that uses FieldTables to send username and pwd.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AmqPlainSaslClient implements SaslClient
{
    /**
     *  The name of this mechanism
     */
    public static final String MECHANISM = "AMQPLAIN";

    private CallbackHandler _cbh;

    public AmqPlainSaslClient(CallbackHandler cbh)
    {
        _cbh = cbh;
    }

    public String getMechanismName()
    {
        return "AMQPLAIN";
    }

    public boolean hasInitialResponse()
    {
        return true;
    }

    public byte[] evaluateChallenge(byte[] challenge) throws SaslException
    {
        // we do not care about the prompt or the default name
        NameCallback nameCallback = new NameCallback("prompt", "defaultName");
        PasswordCallback pwdCallback = new PasswordCallback("prompt", false);
        Callback[] callbacks = new Callback[]{nameCallback, pwdCallback};
        try
        {
            _cbh.handle(callbacks);
        }
        catch (Exception e)
        {
            throw new SaslException("Error handling SASL callbacks: " + e, e);
        }
        FieldTable table = new FieldTable();
        table.put("LOGIN", nameCallback.getName());
        table.put("PASSWORD", pwdCallback.getPassword());
        return table.getDataAsBytes();
    }

    public boolean isComplete()
    {
        return true;
    }

    public byte[] unwrap(byte[] incoming, int offset, int len) throws SaslException
    {
        throw new SaslException("Not supported");
    }

    public byte[] wrap(byte[] outgoing, int offset, int len) throws SaslException
    {
        throw new SaslException("Not supported");
    }

    public Object getNegotiatedProperty(String propName)
    {
        return null;
    }

    public void dispose() throws SaslException
    {
        _cbh = null;
    }
}
