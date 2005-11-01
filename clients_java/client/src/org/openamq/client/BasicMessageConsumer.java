package org.openamq.client;

import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.BasicCancelBody;
import org.openamq.framing.BasicCancelOkBody;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicMessageConsumer extends AbstractMessageConsumer
{

    BasicMessageConsumer(int channelId, AMQDestination destination, boolean noLocal,
                         MessageFactoryRegistry messageFactory, AMQSession session, AMQProtocolHandler protocolHandler)
    {
        super(channelId, destination, null, noLocal, messageFactory, session, protocolHandler);
    }

    protected AMQFrame createCancelFrame(int channelId, int consumerTag)
    {
        return BasicCancelBody.createAMQFrame(channelId, consumerTag);
    }

    protected Class getCancelOkClass()
    {
        return BasicCancelOkBody.class;
    }
}
