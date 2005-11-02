package org.openamq.client.handler;

import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.AMQException;
import org.openamq.framing.JmsDeliverBody;
import org.openamq.framing.BasicDeliverBody;
import org.apache.log4j.Logger;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicDeliverMethodHandler implements StateAwareMethodListener
{
    private static final Logger _logger = Logger.getLogger(BasicDeliverMethodHandler.class);

    private static final BasicDeliverMethodHandler _instance = new BasicDeliverMethodHandler();

    public static BasicDeliverMethodHandler getInstance()
    {
        return _instance;
    }

    public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
    {
        final UnprocessedMessage msg = new UnprocessedMessage();
        // this nastiness is because ASL isn't object oriented so we cannot derive JmsDeliverBody from BasicDeliverBody
        BasicDeliverBody basicBody = (BasicDeliverBody) evt.getMethod();
        JmsDeliverBody body = new JmsDeliverBody();

        // copy the fields from basic body into the jms body
        //body.deliveryTag = basicBody.deliveryTag;
        body.exchange = basicBody.exchange;
        body.routingKey = basicBody.routingKey;

        msg.deliverBody = body;
        msg.channelId = evt.getChannelId();
        _logger.debug("New BasicDeliver method received");
        evt.getProtocolSession().unprocessedMessageReceived(msg);
    }
}
