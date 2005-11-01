package org.openamq.client;

import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.BasicConsumeBody;
import org.openamq.framing.BasicConsumeOkBody;
import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicSessionContentTypeFactory implements SessionContentTypeFactory
{
    public AbstractMessageConsumer createConsumer(int channelId, AMQDestination amqd, String selector, boolean noLocal, MessageFactoryRegistry messageFactoryRegistry, AMQSession session, AMQProtocolHandler protocolHandler)
    {
        return new BasicMessageConsumer(channelId, amqd, noLocal, messageFactoryRegistry, session, protocolHandler);
    }

    public AMQFrame createConsumeFrame(int channelId, int ticket, String queueScope, String queueName, int prefetchSize, int prefetchCount, boolean noLocal, boolean autoAck, boolean exclusive)
    {
        return BasicConsumeBody.createAMQFrame(channelId, ticket, queueScope, queueName, noLocal, exclusive);
    }

    public Class getConsumeOkClass()
    {
        return BasicConsumeOkBody.class;
    }

    public int getConsumerTag(AMQMethodEvent consumeOkEvent)
    {
        return ((BasicConsumeOkBody) consumeOkEvent.getMethod()).consumerTag;
    }

    public AbstractMessageProducer createProducer(AMQDestination amqd, boolean transacted, int channelId,
                                                  AMQSession session, AMQProtocolHandler protocolHandler,
                                                  long producerId) throws AMQException
   {
       return new BasicMessageProducer(amqd, channelId, session, protocolHandler, producerId);
   }
}
