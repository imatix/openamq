package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.JmsBounceBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JmsBounceMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(JmsBounceMethodHandler.class);

    private static final JmsBounceMethodHandler _instance = new JmsBounceMethodHandler();

    public static JmsBounceMethodHandler getInstance()
    {
        return _instance;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        _logger.debug("New JmsBounce method received");
        final UnprocessedMessage msg = new UnprocessedMessage();
        msg.deliverBody = null;
        msg.bounceBody = (JmsBounceBody) evt.getMethod();
        msg.channelId = evt.getChannelId();

        evt.getProtocolSession().unprocessedMessageReceived(msg);
    }
}