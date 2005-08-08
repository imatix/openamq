package org.openamq.client.message;

import org.openamq.framing.AMQMessage;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.framing.ContentBody;
import org.openamq.AMQException;

import javax.jms.JMSException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQTextMessageFactory extends AbstractMessageFactory
{
    protected AbstractMessage createMessageWithBody(long messageNbr,
                                                    JmsContentHeaderBody contentHeader,
                                                    ContentBody[] bodies) throws AMQException
    {
        // TODO: support multiple bodies properly        
        return new AMQTextMessage(messageNbr, bodies[0].payload, contentHeader);
    }

    public AbstractMessage createMessage() throws JMSException
    {
        return new AMQTextMessage();  
    }
}
