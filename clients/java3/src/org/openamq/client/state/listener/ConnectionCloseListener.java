/**
 * Date Created: 21-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client.state.listener;

import org.openamq.client.protocol.BlockingCommandFrameListener;
import org.openamq.client.framing.AMQCommandFrame;
import org.openamq.client.framing.Channel;
import org.openamq.client.framing.Connection;
import org.openamq.client.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionCloseListener extends BlockingCommandFrameListener
{
    public boolean processCommandFrame(AMQCommandFrame frame) throws AMQException
    {
        if (frame.getType() == Connection.Close.FRAME_TYPE)
        {
            Connection.Close reply = (Connection.Close) frame;

            if (reply.replyCode == 200)
            {
                return true;
            }
            else
            {
                throw new AMQException(reply.replyCode, "Abnormal connection close: " + reply.replyText);
            }
        }
        else
        {
            return false;
        }
    }
}
