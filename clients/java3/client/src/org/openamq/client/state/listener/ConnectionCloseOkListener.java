/**
 * Date Created: 21-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client.state.listener;

import org.openamq.AMQException;
import org.openamq.client.protocol.BlockingMethodFrameListener;
import org.openamq.framing.AMQMethodBody;
import org.openamq.framing.ConnectionCloseOkBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionCloseOkListener extends BlockingMethodFrameListener
{

    public ConnectionCloseOkListener()
    {
        super(0);
    }
    
    public boolean processMethod(int channelId, AMQMethodBody frame) throws AMQException
    {
        return (frame instanceof ConnectionCloseOkBody);
    }
}
