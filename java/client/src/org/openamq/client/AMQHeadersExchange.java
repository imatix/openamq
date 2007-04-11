package org.openamq.client;

/**
 * A destination backed by a headers exchange
 */
public class AMQHeadersExchange extends AMQDestination
{
    public AMQHeadersExchange(String queueName)
    {
        super(queueName, HEADERS_EXCHANGE_CLASS, queueName, true, true, null);
    }

    public String getEncodedName()
    {
        return getDestinationName();
    }

    public String getRoutingKey()
    {
        return getDestinationName();
    }

    public boolean isNameRequired()
    {
        //Not sure what the best approach is here, probably to treat this like a topic
        //and allow server to generate names. As it is AMQ specific it doesn't need to
        //fit the JMS API expectations so this is not as yet critical.
        return false;
    }
}
