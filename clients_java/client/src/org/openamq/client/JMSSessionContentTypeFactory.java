package org.openamq.client;

import org.openamq.AMQException;
import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.JmsConsumeBody;
import org.openamq.framing.JmsConsumeOkBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JMSSessionContentTypeFactory implements SessionContentTypeFactory
{
    public AbstractMessageConsumer createConsumer(int channelId, AMQDestination amqd, String selector,
                                                  boolean noLocal, MessageFactoryRegistry messageFactoryRegistry,
                                                  AMQSession session, AMQProtocolHandler protocolHandler)
    {
        return new JMSMessageConsumer(channelId, amqd, selector, noLocal, messageFactoryRegistry, session, protocolHandler);
    }

    public AMQFrame createConsumeFrame(int channelId, int ticket, String queueScope, String queueName, int prefetchSize,
                                       int prefetchCount, boolean noLocal, boolean autoAck, boolean exclusive)
    {
        return JmsConsumeBody.createAMQFrame(channelId, ticket, queueScope, queueName, prefetchSize, prefetchCount,
                                             noLocal, autoAck, exclusive);
    }

    public Class getConsumeOkClass()
    {
        return JmsConsumeOkBody.class;
    }

    public int getConsumerTag(AMQMethodEvent consumeOkEvent)
    {
        return ((JmsConsumeOkBody) consumeOkEvent.getMethod()).consumerTag;
    }

    public AbstractMessageProducer createProducer(AMQDestination amqd, boolean transacted, int channelId,
                                                  AMQSession session, AMQProtocolHandler protocolHandler,
                                                  long producerId) throws AMQException
    {
        return new JMSMessageProducer(amqd, transacted, channelId, session, protocolHandler, producerId);
    }
}
