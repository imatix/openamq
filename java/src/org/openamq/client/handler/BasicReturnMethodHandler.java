package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.BasicReturnBody;

public class BasicReturnMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(BasicReturnMethodHandler.class);

    private static final BasicReturnMethodHandler _instance = new BasicReturnMethodHandler();

    public static BasicReturnMethodHandler getInstance()
    {
        return _instance;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        _logger.debug("New JmsBounce method received");
        final UnprocessedMessage msg = new UnprocessedMessage();
        msg.deliverBody = null;
        msg.bounceBody = (BasicReturnBody) evt.getMethod();
        msg.channelId = evt.getChannelId();

        evt.getProtocolSession().unprocessedMessageReceived(msg);
    }
}
