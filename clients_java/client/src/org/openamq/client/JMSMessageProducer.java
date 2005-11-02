package org.openamq.client;

import org.openamq.AMQException;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.JmsPublishBody;

/**
 * An implementation of an JMSMessageProducer which extends the JMS AbstractJMSMessage Producer.
 */
public class JMSMessageProducer extends AbstractMessageProducer
{

    JMSMessageProducer(AMQDestination destination, boolean transacted, int channelId,
                       AMQSession session, AMQProtocolHandler protocolHandler, long producerId)
            throws AMQException
    {
        super(destination, transacted, channelId, session, protocolHandler, producerId);
    }

    protected AMQFrame createPublishFrame(int channelId, int ticket, String exchangeName,
                                          String routingKey, boolean mandatory, boolean immediate)
    {
        return JmsPublishBody.createAMQFrame(channelId, 0, _destination.getExchangeName(),
                                             _destination.getDestinationName(), mandatory,
                                                              immediate);
    }

}
