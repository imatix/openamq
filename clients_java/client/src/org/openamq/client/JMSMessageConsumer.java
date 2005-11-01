package org.openamq.client;

import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.JmsCancelBody;
import org.openamq.framing.JmsCancelOkBody;


/**
 */
public class JMSMessageConsumer extends AbstractMessageConsumer
{

    JMSMessageConsumer(int channelId, AMQDestination destination, String messageSelector, boolean noLocal,
                       MessageFactoryRegistry messageFactory, AMQSession session, AMQProtocolHandler protocolHandler)
    {
        super(channelId, destination, messageSelector, noLocal, messageFactory, session, protocolHandler);
    }

    protected AMQFrame createCancelFrame(int channelId, int consumerTag)
    {
        return JmsCancelBody.createAMQFrame(channelId, consumerTag);
    }

    protected Class getCancelOkClass()
    {
        return JmsCancelOkBody.class;
    }
}
