package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.AMQMessage;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.framing.ContentBody;

import javax.jms.JMSException;
import java.util.List;


/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface MessageFactory
{
    AbstractMessage createMessage(long messageNbr, boolean redelivered,
                                  JmsContentHeaderBody contentHeader,
                                  List bodies)
        throws JMSException, AMQException;

    AbstractMessage createMessage() throws JMSException;
}
