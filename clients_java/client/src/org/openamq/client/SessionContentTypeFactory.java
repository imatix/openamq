package org.openamq.client;

import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.framing.AMQFrame;
import org.openamq.AMQException;

/**
 * This factory generates frames and other details that are specific to a particular content type. Currently
 * the only supported contents types are Basic and JMS but this can be extended to support other content types.
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface SessionContentTypeFactory
{
    AbstractMessageConsumer createConsumer(int channelId, AMQDestination amqd, String selector, boolean noLocal,
                                           MessageFactoryRegistry messageFactoryRegistry, AMQSession session,
                                           AMQProtocolHandler protocolHandler);

    AMQFrame createConsumeFrame(int channelId, int ticket, String queueName, int prefetchSize,
                                int prefetchCount, boolean noLocal, boolean autoAck, boolean exclusive);

    Class getConsumeOkClass();

    int getConsumerTag(AMQMethodEvent consumeOkEvent);

    AbstractMessageProducer createProducer(AMQDestination amqd, boolean transacted, int channelId,
                                           AMQSession session, AMQProtocolHandler protocolHandler,
                                           long producerId) throws AMQException;
}
