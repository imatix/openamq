package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.ContentHeaderBody;
import org.openamq.framing.ContentBody;
import org.openamq.framing.BasicContentHeaderProperties;
import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;

import javax.jms.JMSException;
import java.util.List;
import java.util.Iterator;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractJMSMessageFactory implements MessageFactory
{
    private static final Logger _logger = Logger.getLogger(AbstractJMSMessageFactory.class);


    protected abstract AbstractJMSMessage createMessage(long messageNbr, ByteBuffer data,
                                                                ContentHeaderBody contentHeader) throws AMQException;

    protected AbstractJMSMessage createMessageWithBody(long messageNbr,
                                                       ContentHeaderBody contentHeader,
                                                       List bodies) throws AMQException
    {
        ByteBuffer data;

        // we optimise the non-fragmented case to avoid copying
        if (bodies != null && bodies.size() == 1)
        {
            _logger.debug("Non-fragmented message body (bodySize=" + contentHeader.bodySize +")");
            data = ((ContentBody)bodies.get(0)).payload;
        }
        else
        {
            _logger.debug("Fragmented message body (" + bodies.size() + " frames, bodySize=" + contentHeader.bodySize + ")");
            data = ByteBuffer.allocate((int)contentHeader.bodySize);
            final Iterator it = bodies.iterator();
            while (it.hasNext())
            {
                ContentBody cb = (ContentBody) it.next();
                data.put(cb.payload);
                cb.payload.release();
            }
            data.flip();
        }
        _logger.debug("Creating message from buffer with position=" + data.position() + " and remaining=" + data.remaining());

        return createMessage(messageNbr, data, contentHeader);
    }

    public AbstractJMSMessage createMessage(long messageNbr, boolean redelivered,
                                            ContentHeaderBody contentHeader,
                                            List bodies) throws JMSException, AMQException
    {
        final AbstractJMSMessage msg = createMessageWithBody(messageNbr, contentHeader, bodies);
        msg.setJMSRedelivered(redelivered);
        return msg;
    }

}
