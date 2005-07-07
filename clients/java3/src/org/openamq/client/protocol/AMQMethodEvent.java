package org.openamq.client.protocol;

import org.openamq.client.framing.AMQMethodBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQMethodEvent
{
    private AMQMethodBody _method;

    private short _channelId;
    
    private AMQProtocolSession _protocolSession;

    public AMQMethodEvent(short channelId, AMQMethodBody method, AMQProtocolSession protocolSession)
    {
        _channelId = channelId;
        _method = method;
        _protocolSession = protocolSession;
    }

    public AMQMethodBody getMethod()
    {
        return _method;
    }

    public short getChannelId()
    {
        return _channelId;
    }
    
    public AMQProtocolSession getProtocolSession()
    {
        return _protocolSession;
    }
    
    public String toString()
    {
        StringBuffer buf = new StringBuffer("Method event: ");
        buf.append("\nChannel id: ").append(_channelId);
        buf.append("\nMethod: ").append(_method);
        return buf.toString();
    }
}
