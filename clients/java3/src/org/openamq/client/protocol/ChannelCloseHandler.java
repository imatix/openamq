/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client.protocol;

import org.apache.log4j.Logger;
import org.openamq.client.AMQException;
import org.openamq.client.framing.Channel;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareFrameListener;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ChannelCloseHandler implements StateAwareFrameListener
{
    private static final Logger _logger = Logger.getLogger(ChannelCloseHandler.class);

    private static ChannelCloseHandler _handler = new ChannelCloseHandler();

    public static ChannelCloseHandler getInstance()
    {
        return _handler;
    }

    public void frameReceived(AMQStateManager stateManager, FrameEvent evt) throws AMQException
    {
         _logger.debug("ChannelClose frame received");
        Channel.Close frame = (Channel.Close) evt.getFrame();

        int errorCode = frame.replyCode;
        String reason = frame.replyText;
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Channel close reply code: " + errorCode + ", reason: " + reason);
        }
        // returns true if this frame requires the client to respond to the server,
        // i.e. if the server initiated the close not the client itself.
        if (evt.getProtocolSession().channelClosed(frame.channelId, errorCode,
                                                   reason))
        {
            frame.replyCode = 200;
            frame.replyText = "Client says au revoir";
            evt.getProtocolSession().writeFrame(frame);
        }
        if (errorCode != 200)
        {
            _logger.debug("Channel close received with errorCode " + errorCode + ", throwing exception");
            throw new AMQException(errorCode, "Error: " + reason);
        }
    }
}
