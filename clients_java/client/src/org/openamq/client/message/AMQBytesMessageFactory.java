package org.openamq.client.message;

import org.openamq.framing.AMQMessage;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.framing.ContentBody;
import org.openamq.AMQException;

import javax.jms.JMSException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQBytesMessageFactory extends AbstractMessageFactory
{
     protected AbstractMessage createMessageWithBody(long messageNbr,
                                                    JmsContentHeaderBody contentHeader,
                                                    ContentBody[] bodies) throws AMQException
    {
        byte[] data;

        // we optimise the non-fragmented case to avoid copying
        if (bodies != null && bodies.length == 1 && bodies[0] != null)
        {
            data = bodies[0].payload;
        }
        else
        {
            data = new byte[(int)contentHeader.bodySize];
            int currentPosition = 0;
            for (int i = 0; i < bodies.length; i++)
            {
                System.arraycopy(bodies[i].payload, 0, data, currentPosition, bodies[i].payload.length);
                currentPosition += bodies[i].payload.length;
            }
        }

        return new AMQBytesMessage(messageNbr, data, contentHeader);
    }

    public AbstractMessage createMessage() throws JMSException
    {
        return new AMQBytesMessage();  
    }
}
