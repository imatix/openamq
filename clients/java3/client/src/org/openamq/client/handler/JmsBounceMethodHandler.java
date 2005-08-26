package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.framing.ConnectionCloseBody;
import org.openamq.framing.ConnectionCloseOkBody;
import org.openamq.framing.JmsBounceBody;
import org.openamq.framing.JmsDeliverBody;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.message.UnprocessedMessage;
import javax.jms.JMSException;

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
        JmsBounceBody method = (JmsBounceBody) evt.getMethod();
        msg.deliverBody = null;
        msg.bounceBody = method;
        msg.channelId = evt.getChannelId();
        
        evt.getProtocolSession().unprocessedMessageReceived(msg);
        
        int errorCode = method.replyCode;
        String reason = method.replyText;

        _logger.debug("Message returned with errorCode " + errorCode + ", routing through ExceptionListener");
        evt.getProtocolSession().getAMQConnection().exceptionReceived(new AMQException(errorCode, "Error: " + reason));
    }
}