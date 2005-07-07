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
import org.openamq.client.framing.AMQMethodBody;
import org.openamq.client.framing.Channel;
import org.openamq.client.framing.ChannelCloseOkBody;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.ConnectionCloseOkBody;
import org.openamq.client.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionCloseOkListener extends BlockingCommandFrameListener
{
    public boolean processMethod(short channelId, AMQMethodBody frame) throws AMQException
    {
        return (frame instanceof ConnectionCloseOkBody);
    }    
}
