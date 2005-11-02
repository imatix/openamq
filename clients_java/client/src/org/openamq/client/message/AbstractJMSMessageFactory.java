package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.ContentHeaderBody;

import javax.jms.JMSException;
import java.util.List;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractJMSMessageFactory implements MessageFactory
{

    protected abstract AbstractJMSMessage createMessageWithBody(long messageNbr,
                                                                ContentHeaderBody contentHeader,
                                                                List bodies) throws AMQException;

    public AbstractJMSMessage createMessage(long messageNbr, boolean redelivered,
                                         ContentHeaderBody contentHeader,
                                         List bodies) throws JMSException, AMQException
    {
        final AbstractJMSMessage msg = createMessageWithBody(messageNbr, contentHeader, bodies);
        msg.setJMSRedelivered(redelivered);
        return msg;
    }

}
