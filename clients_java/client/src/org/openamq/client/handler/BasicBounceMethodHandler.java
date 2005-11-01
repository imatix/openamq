package org.openamq.client.handler;

import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.AMQException;
import org.openamq.framing.JmsBounceBody;
import org.openamq.framing.BasicBounceBody;
import org.apache.log4j.Logger;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicBounceMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(BasicBounceMethodHandler.class);

    private static final BasicBounceMethodHandler _instance = new BasicBounceMethodHandler();

    public static BasicBounceMethodHandler getInstance()
    {
        return _instance;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        _logger.debug("New BasicBounce method received");
        final UnprocessedMessage msg = new UnprocessedMessage();
        BasicBounceBody basicBody = (BasicBounceBody) evt.getMethod();
        JmsBounceBody jmsBody = new JmsBounceBody();
        jmsBody.exchange = basicBody.exchange;
        jmsBody.replyCode = basicBody.replyCode;
        jmsBody.replyText = basicBody.replyText;
        jmsBody.routingKey = basicBody.routingKey;
        jmsBody.synchtag = basicBody.synchtag;

        msg.deliverBody = null;
        msg.bounceBody = jmsBody;
        msg.channelId = evt.getChannelId();

        evt.getProtocolSession().unprocessedMessageReceived(msg);
    }
}
