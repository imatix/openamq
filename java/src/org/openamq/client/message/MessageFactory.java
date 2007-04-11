package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.ContentHeaderBody;

import javax.jms.JMSException;
import java.util.List;


/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface MessageFactory
{
    AbstractJMSMessage createMessage(long deliveryTag, boolean redelivered,
                                     ContentHeaderBody contentHeader,
                                     List bodies)
        throws JMSException, AMQException;

    AbstractJMSMessage createMessage() throws JMSException;
}
