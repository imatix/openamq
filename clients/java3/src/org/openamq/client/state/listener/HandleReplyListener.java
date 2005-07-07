package org.openamq.client.state.listener;

import org.openamq.client.AMQException;
import org.openamq.client.framing.AMQDataBlock;
import org.openamq.client.framing.AMQMethodBody;
import org.openamq.client.framing.Handle;
import org.openamq.client.framing.AMQCommandFrame;
import org.openamq.client.framing.Channel;
import org.openamq.client.protocol.BlockingCommandFrameListener;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class HandleReplyListener extends BlockingCommandFrameListener
{
    private int _handleId;

    /**
     * Used to wait until a channel reply has been received with the specified id
     * @param handleId
     */
    public HandleReplyListener(int handleId)
    {
        _handleId = handleId;
    }

    public boolean processCommandFrame(AMQCommandFrame frame)
    {
        if (frame.getType() == Handle.Reply.FRAME_TYPE)
        {
            Handle.Reply reply = (Handle.Reply) frame;
            return reply.handleId == _handleId;
        }
        else
        {
            return false;
        }
    }

    public boolean processMethod(short channelId, AMQMethodBody frame) throws AMQException
    {
        throw new AMQException("HandleReply frame received!!");
    }
}
