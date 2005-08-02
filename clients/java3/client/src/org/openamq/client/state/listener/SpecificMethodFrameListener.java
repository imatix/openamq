package org.openamq.client.state.listener;

import org.openamq.client.protocol.BlockingMethodFrameListener;
import org.openamq.framing.AMQMethodBody;
import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class SpecificMethodFrameListener extends BlockingMethodFrameListener
{
    private final Class _expectedClass;

    public SpecificMethodFrameListener(int channelId, Class expectedClass)
    {
        super(channelId);
        _expectedClass = expectedClass;
    }

    public boolean processMethod(int channelId, AMQMethodBody frame) throws AMQException
    {
        return _expectedClass.isInstance(frame);
    }
}
