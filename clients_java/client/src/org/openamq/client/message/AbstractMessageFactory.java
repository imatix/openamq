package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.AMQMessage;
import org.openamq.framing.FieldTable;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.framing.ContentBody;

import javax.jms.DeliveryMode;
import javax.jms.JMSException;
import java.util.Iterator;
import java.util.Map;
import java.util.List;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractMessageFactory implements MessageFactory
{

    protected abstract AbstractMessage createMessageWithBody(long messageNbr,
                                                             JmsContentHeaderBody contentHeader,
                                                             List bodies) throws AMQException;

    public AbstractMessage createMessage(long messageNbr, boolean redelivered,
                                         JmsContentHeaderBody contentHeader,
                                         List bodies) throws JMSException, AMQException
    {
        final AbstractMessage msg = createMessageWithBody(messageNbr, contentHeader, bodies);
        msg.setJMSRedelivered(redelivered);
        return msg;
    }

}
