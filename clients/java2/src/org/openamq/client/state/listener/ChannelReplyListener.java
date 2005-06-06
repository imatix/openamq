package org.openamq.client.state.listener;

import org.openamq.client.framing.AMQFrame;
import org.openamq.client.framing.Channel;
import org.openamq.client.state.BlockingFrameListener;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ChannelReplyListener implements BlockingFrameListener
{
    private volatile boolean _done = false;

    private int _channelId;

    /**
     * Used to wait until a channel reply has been received with the specified id
     * @param channelId
     */
    public ChannelReplyListener(int channelId)
    {
        _channelId = channelId;
    }

    public void frameReceived(AMQFrame frame)
    {
        Channel.Reply channelReply = (Channel.Reply) frame;
        _done = (_channelId == channelReply.channelId);
    }

    public Class getInterestingFrame()
    {
        return Channel.Reply.class;
    }

    public boolean readyToContinue()
    {
        return _done;
    }
}
