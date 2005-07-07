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
import org.openamq.client.framing.AMQMethodBody;
import org.openamq.client.framing.Channel;
import org.openamq.client.framing.ChannelCloseOkBody;
import org.openamq.client.AMQException;

/**
 * Used to wait until a successful Channel Close response has been received from the server.
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ChannelCloseOkListener extends BlockingCommandFrameListener
{
    private int _channelId;

    public ChannelCloseOkListener(int channelId)
    {
        _channelId = channelId;
    }

    public boolean processMethod(short channelId, AMQMethodBody method) throws AMQException
    {
        if (channelId == _channelId)
        {
            return (method instanceof ChannelCloseOkBody);                                                
        }        
        return false;              
    }
}
