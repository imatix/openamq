package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.AMQMessage;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.framing.ContentBody;

import javax.jms.JMSException;


/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface MessageFactory
{
    AbstractMessage createMessage(long messageNbr, boolean redelivered,
                                  JmsContentHeaderBody contentHeader,
                                  ContentBody[] bodies)
        throws JMSException, AMQException;

    AbstractMessage createMessage() throws JMSException;
}
