/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client.state.listener;

import org.openamq.client.AMQException;
import org.openamq.client.protocol.BlockingCommandFrameListener;
import org.openamq.client.framing.AMQCommandFrame;
import org.openamq.client.framing.AMQMethodBody;
import org.openamq.client.framing.Handle;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class HandleCloseListener extends BlockingCommandFrameListener
{
    private int _handleId;

    public HandleCloseListener(int handleId)
    {
        _handleId = handleId;
    }

    public boolean processCommandFrame(AMQCommandFrame frame)
    {
        if (frame.getType() == Handle.Close.FRAME_TYPE)
        {
            Handle.Close reply = (Handle.Close) frame;
            return reply.handleId == _handleId;
        }
        else
        {
            return false;
        }
    }

    public boolean processMethod(short channelId, AMQMethodBody frame) throws AMQException
    {
        throw new AMQException("HandleClose frame listener invoked!!");
    }
}
