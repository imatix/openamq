package org.openamq.client;

import javax.jms.Destination;

public abstract class AMQDestination implements Destination
{
    public final static String TOPIC_EXCHANGE_NAME = "amq.topic";

    public final static String TOPIC_EXCHANGE_CLASS = "topic";

    public final static String QUEUE_EXCHANGE_NAME = "amq.direct";

    public final static String QUEUE_EXCHANGE_CLASS = "direct";

    public final static String HEADERS_EXCHANGE_NAME = "amq.match";

    public final static String HEADERS_EXCHANGE_CLASS = "headers";

    protected final static String DEFAULT_SCOPE = "defaultScope";

    protected final String _exchangeName;

    protected final String _exchangeClass;

    protected final String _destinationName;

    protected final boolean _isTemporary;

    protected final String _queueName;

    protected final String _queueScope;

    protected AMQDestination(String exchangeName, String exchangeClass, String destinationName, String queueName,
            String queueScope)
    {
        this(exchangeName, exchangeClass, destinationName, false, queueName, queueScope);
    }

    protected AMQDestination(String exchangeName, String exchangeClass, String destinationName, boolean isTemporary,
                             String queueName, String queueScope)
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
        _isTemporary = isTemporary;
        _queueScope = queueScope;
        _queueName = queueName;
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

    public boolean isTemporary()
    {
        return _isTemporary;
    }

    public String getQueueScope()
    {
        return _queueScope;
    }

    public String getQueueName()
    {
        return _queueName;
    }

    public String toString()
    {
        return "Destination: " + _destinationName + ", Queue Scope: " + _queueScope + ", " +
               "Queue Name: " + _queueName + ", Exchange: " + _exchangeName +
               ", Exchange class: " + _exchangeClass + ", Temporary: " + _isTemporary;
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
        if (!_queueScope.equals(that._queueScope))
        {
            return false;
        }
        if ((_queueName == null && that._queueName != null) ||
            (_queueName != null && !_queueName.equals(that._queueName)))
        {
            return false;
        }
        if (_isTemporary != that._isTemporary)
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
        if (_queueScope != null)
        {
            result = 29 * result + _queueScope.hashCode();
        }
        if (_queueName != null)
        {
            result = 29 * result + _queueName.hashCode();
        }
        result = result * (_isTemporary?13:7);
        return result;
    }
}
