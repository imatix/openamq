package org.openamq.client.message;

import org.apache.mina.common.ByteBuffer;
import org.openamq.AMQException;
import org.openamq.framing.ContentHeaderBody;

import javax.jms.JMSException;

public class JMSBytesMessageFactory extends AbstractJMSMessageFactory
{
    protected AbstractJMSMessage createMessage(long deliveryTag, ByteBuffer data, ContentHeaderBody contentHeader) throws AMQException
    {
        return new JMSBytesMessage(deliveryTag, data, contentHeader);
    }

    public AbstractJMSMessage createMessage() throws JMSException
    {
        return new JMSBytesMessage();
    }
}
