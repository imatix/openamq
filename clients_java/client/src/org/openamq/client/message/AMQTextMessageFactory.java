package org.openamq.client.message;

import org.openamq.framing.AMQMessage;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.framing.ContentBody;
import org.openamq.AMQException;

import javax.jms.JMSException;
import java.util.List;
import java.util.Iterator;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQTextMessageFactory extends AbstractMessageFactory
{
    protected AbstractMessage createMessageWithBody(long messageNbr,
                                                    JmsContentHeaderBody contentHeader,
                                                    List bodies) throws AMQException
    {
        byte[] data;

        // we optimise the non-fragmented case to avoid copying
        if (bodies != null && bodies.size() == 1)
        {
            data = ((ContentBody)bodies.get(0)).payload;
        }
        else
        {
            data = new byte[(int)contentHeader.bodySize];
            int currentPosition = 0;
            final Iterator it = bodies.iterator();
            while (it.hasNext())
            {
                ContentBody cb = (ContentBody) it.next();
                System.arraycopy(cb.payload, 0, data, currentPosition, cb.payload.length);
                currentPosition += cb.payload.length;
            }
        }

        return new AMQTextMessage(messageNbr, data, contentHeader);
    }

    public AbstractMessage createMessage() throws JMSException
    {
        return new AMQTextMessage();
    }
}
