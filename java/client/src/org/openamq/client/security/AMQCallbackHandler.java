package org.openamq.client.security;

import org.openamq.client.protocol.AMQProtocolSession;

import javax.security.auth.callback.CallbackHandler;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface AMQCallbackHandler extends CallbackHandler
{
    void initialise(AMQProtocolSession protocolSession);    
}
