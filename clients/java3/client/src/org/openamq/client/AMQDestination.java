package org.openamq.client;

import javax.jms.Destination;

public abstract class AMQDestination implements Destination
{
    public final static String TOPIC_EXCHANGE_NAME = "topic";

    public final static String TOPIC_EXCHANGE_CLASS = "dest-wild";

    public final static String QUEUE_EXCHANGE_NAME = "queue";

    public final static String QUEUE_EXCHANGE_CLASS = "dest-name";

    private final String _exchangeName;

    private final String _exchangeClass;

    private final String _destinationName;

    public AMQDestination(String exchangeName, String exchangeClass, String destinationName)
    {
        if (destinationName == null)
        {
            throw new IllegalArgumentException("Destination name must not be null");
        }
        if (exchangeName == null)
        {
            throw new IllegalArgumentException("Exchange name must not be null");
        }
        if (exchangeClass == null)
        {
            throw new IllegalArgumentException("Exchange class must not be null");
        }
        _exchangeName = exchangeName;
        _exchangeClass = exchangeClass;
        _destinationName = destinationName;
    }

    public abstract String getEncodedName();

    public String getExchangeName()
    {
        return _exchangeName;
    }

    public String getExchangeClass()
    {
        return _exchangeClass;
    }

    public boolean isTopic()
    {
    	return TOPIC_EXCHANGE_NAME.equals(_exchangeName);
    }

    public boolean isQueue()
    {
        return QUEUE_EXCHANGE_NAME.equals(_exchangeName);
    }

    public String getDestinationName()
    {
        return _destinationName;
    }
    
    public String toString()
    {
        return "Destination: " + _destinationName + ", Exchange: " + _exchangeName +
               ", Exchange class: " + _exchangeClass;
    }

    public boolean equals(Object o)
    {
        if (this == o)
        {
            return true;
        }
        if (o == null || getClass() != o.getClass())
        {
            return false;
        }

        final AMQDestination that = (AMQDestination) o;

        if (!_destinationName.equals(that._destinationName))
        {
            return false;
        }
        if (!_exchangeClass.equals(that._exchangeClass))
        {
            return false;
        }
        if (!_exchangeName.equals(that._exchangeName))
        {
            return false;
        }

        return true;
    }

    public int hashCode()
    {
        int result;
        result = _exchangeName.hashCode();
        result = 29 * result + _exchangeClass.hashCode();
        result = 29 * result + _destinationName.hashCode();
        return result;
    }
}
