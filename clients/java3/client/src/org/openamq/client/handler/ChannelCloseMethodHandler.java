/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.ChannelCloseBody;
import org.openamq.framing.ChannelCloseOkBody;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.protocol.AMQMethodEvent;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ChannelCloseMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(ChannelCloseMethodHandler.class);

    private static ChannelCloseMethodHandler _handler = new ChannelCloseMethodHandler();

    public static ChannelCloseMethodHandler getInstance()
    {
        return _handler;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
         _logger.debug("ChannelClose method received");
        ChannelCloseBody method = (ChannelCloseBody) evt.getMethod();

        int errorCode = method.replyCode;
        String reason = method.replyText;
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Channel close reply code: " + errorCode + ", reason: " + reason);
        }

        AMQFrame frame = ChannelCloseOkBody.createAMQFrame(evt.getChannelId());
        evt.getProtocolSession().writeFrame(frame);
        if (errorCode != 200)
        {
            _logger.debug("Channel close received with errorCode " + errorCode + ", throwing exception");
            throw new AMQException(errorCode, "Error: " + reason);
        }
    }
}
