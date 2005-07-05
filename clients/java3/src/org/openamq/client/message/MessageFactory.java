package org.openamq.client.message;

import org.openamq.client.framing.AMQMessage;
import org.openamq.client.AMQException;

import javax.jms.JMSException;


/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface MessageFactory
{
    AbstractMessage createMessage(long messageNbr, boolean redelivered, AMQMessage frame)
        throws JMSException, AMQException;

    AbstractMessage createMessage() throws JMSException;
}
