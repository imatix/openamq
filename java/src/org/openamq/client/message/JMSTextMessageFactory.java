package org.openamq.client.message;

import org.apache.mina.common.ByteBuffer;
import org.openamq.AMQException;
import org.openamq.framing.BasicContentHeaderProperties;
import org.openamq.framing.ContentHeaderBody;

import javax.jms.JMSException;

public class JMSTextMessageFactory extends AbstractJMSMessageFactory
{

    public AbstractJMSMessage createMessage() throws JMSException
    {
        return new JMSTextMessage();
    }

    protected AbstractJMSMessage createMessage(long deliveryTag, ByteBuffer data, ContentHeaderBody contentHeader) throws AMQException
    {
        return new JMSTextMessage(deliveryTag, data, (BasicContentHeaderProperties)contentHeader.properties);
    }
}
