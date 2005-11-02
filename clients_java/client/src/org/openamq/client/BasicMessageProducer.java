package org.openamq.client;

import org.openamq.AMQException;
import org.openamq.client.message.AbstractJMSMessage;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.BasicPublishBody;

import javax.jms.JMSException;
import javax.jms.Message;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicMessageProducer extends AbstractMessageProducer
{
    BasicMessageProducer(AMQDestination destination, int channelId,
                         AMQSession session, AMQProtocolHandler protocolHandler, long producerId)
            throws AMQException
    {
        super(destination, false, channelId, session, protocolHandler, producerId);
    }

    public void send(Message message)
            throws JMSException
    {
        checkNotClosed();
        sendImpl(_destination, (AbstractJMSMessage) message, 0, 0, 0, DEFAULT_MANDATORY, DEFAULT_IMMEDIATE);
    }

    protected AMQFrame createPublishFrame(int channelId, int ticket, String exchangeName,
                                          String routingKey, boolean mandatory, boolean immediate)
    {
        return BasicPublishBody.createAMQFrame(channelId, 0, _destination.getExchangeName(),
                                               _destination.getDestinationName(), mandatory,
                                               immediate);
    }
}
