package org.openamq.client.message;

import org.openamq.client.framing.AMQMessage;

import javax.jms.JMSException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQTextMessageFactory extends AbstractMessageFactory
{
    protected AbstractMessage createMessageWithBody(long messageNbr, AMQMessage frame) throws JMSException
    {
        return new AMQTextMessage(messageNbr, frame.message, frame.encoding);
    }

    public AbstractMessage createMessage() throws JMSException
    {
        return new AMQTextMessage();  
    }
}
