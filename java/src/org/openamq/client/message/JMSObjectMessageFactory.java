package org.openamq.client.message;

import org.apache.mina.common.ByteBuffer;
import org.openamq.AMQException;
import org.openamq.framing.ContentHeaderBody;

import javax.jms.JMSException;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class JMSObjectMessageFactory extends AbstractJMSMessageFactory
{
    protected AbstractJMSMessage createMessage(long deliveryTag, ByteBuffer data, ContentHeaderBody contentHeader) throws AMQException
    {
        return new JMSObjectMessage(deliveryTag, data, contentHeader);
    }

    public AbstractJMSMessage createMessage() throws JMSException
    {
        return new JMSObjectMessage();
    }
}
