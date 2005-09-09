package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.framing.JmsDeliverBody;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.message.UnprocessedMessage;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JmsDeliverMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(JmsDeliverMethodHandler.class);

    private static final JmsDeliverMethodHandler _instance = new JmsDeliverMethodHandler();

    public static JmsDeliverMethodHandler getInstance()
    {
        return _instance;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        final UnprocessedMessage msg = new UnprocessedMessage();
        msg.deliverBody = (JmsDeliverBody) evt.getMethod();
        msg.channelId = evt.getChannelId();
        _logger.debug("New JmsDeliver method received");
        evt.getProtocolSession().unprocessedMessageReceived(msg);
    }
}
