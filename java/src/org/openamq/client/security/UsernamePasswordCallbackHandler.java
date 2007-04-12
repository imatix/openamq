package org.openamq.client.security;

import org.openamq.client.protocol.AMQProtocolSession;

import javax.security.auth.callback.*;
import java.io.IOException;

public class UsernamePasswordCallbackHandler implements AMQCallbackHandler
{
    private AMQProtocolSession _protocolSession;

    public void initialise(AMQProtocolSession protocolSession)
    {
        _protocolSession = protocolSession;
    }

    public void handle(Callback[] callbacks) throws IOException, UnsupportedCallbackException
    {
        for (int i = 0; i < callbacks.length; i++)
        {
            Callback cb = callbacks[i];
            if (cb instanceof NameCallback)
            {
                ((NameCallback)cb).setName(_protocolSession.getUsername());
            }
            else if (cb instanceof PasswordCallback)
            {
                ((PasswordCallback)cb).setPassword(_protocolSession.getPassword().toCharArray());
            }
            else
            {
                throw new UnsupportedCallbackException(cb);
            }
        }
    }
}
