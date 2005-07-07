package org.openamq.client.state.listener;

import org.openamq.client.framing.AMQFrame;
import org.openamq.client.framing.Channel;
import org.openamq.client.framing.AMQCommandFrame;
import org.openamq.client.protocol.BlockingCommandFrameListener;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ChannelReplyListener extends BlockingCommandFrameListener
{
    private int _channelId;

    /**
     * Used to wait until a channel reply has been received with the specified id
     * @param channelId
     */
    public ChannelReplyListener(int channelId)
    {
        _channelId = channelId;
    }

    public boolean processCommandFrame(AMQCommandFrame frame)
    {
        if (frame.getType() == Channel.Reply.FRAME_TYPE)
        {
            Channel.Reply reply = (Channel.Reply) frame;
            return reply.channelId == _channelId;
        }
        else
        {
            return false;
        }
    }
}

