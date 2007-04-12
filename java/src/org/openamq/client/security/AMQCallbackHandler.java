package org.openamq.client.security;

import org.openamq.client.protocol.AMQProtocolSession;

import javax.security.auth.callback.CallbackHandler;

public interface AMQCallbackHandler extends CallbackHandler
{
    void initialise(AMQProtocolSession protocolSession);    
}
