package org.openamq.client.protocol;

import org.openamq.client.framing.AMQFrame;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class FrameEvent
{
    private AMQFrame _frame;

    private AMQProtocolSession _protocolSession;

    public FrameEvent(AMQFrame frame, AMQProtocolSession protocolSession)
    {
        _frame = frame;
        _protocolSession = protocolSession;
    }

    public AMQFrame getFrame()
    {
        return _frame;
    }

    public AMQProtocolSession getProtocolSession()
    {
        return _protocolSession;
    }
}
