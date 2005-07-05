/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client.state.listener;

import org.openamq.client.protocol.BlockingCommandFrameListener;
import org.openamq.client.framing.AMQCommandFrame;
import org.openamq.client.framing.Channel;
import org.openamq.client.AMQException;

/**
 * Used to wait until a successful Channel Close response has been received from the server.
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ChannelCloseListener extends BlockingCommandFrameListener
{
    private int _channelId;

    public ChannelCloseListener(int channelId)
    {
        _channelId = channelId;
    }

    public boolean processCommandFrame(AMQCommandFrame frame) throws AMQException
    {
         if (frame.getType() == Channel.Close.FRAME_TYPE)
        {
            Channel.Close reply = (Channel.Close) frame;
            if (reply.channelId == _channelId)
            {
                if (reply.replyCode == 200)
                {
                    return true;
                }
                else
                {
                    throw new AMQException(reply.replyCode, "Abnormal channel close: " + reply.replyText);
                }
            }
            else
            {
                return false;
            }            
        }
        else
        {
            return false;
        }
    }
}
