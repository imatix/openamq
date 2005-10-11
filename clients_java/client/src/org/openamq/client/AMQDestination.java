package org.openamq.client;

import javax.jms.Destination;

public abstract class AMQDestination implements Destination
{
    public final static String TOPIC_EXCHANGE_NAME = "topic";

    public final static String TOPIC_EXCHANGE_CLASS = "topic";

    public final static String QUEUE_EXCHANGE_NAME = "queue";

    public final static String QUEUE_EXCHANGE_CLASS = "direct";

    public final static String HEADERS_EXCHANGE_NAME = "match";

    public final static String HEADERS_EXCHANGE_CLASS = "headers";
    
    protected final static String DEFAULT_SCOPE = "defaultScope";

    protected final String _exchangeName;

    protected final String _exchangeClass;

    protected final String _destinationName;

    protected final boolean _isTemporary;

    protected final boolean _newQueue;

    protected final String _scope;

    protected AMQDestination(String exchangeName, String exchangeClass, String destinationName, boolean isTemporary,
            String scope)
    {
        this(exchangeName, exchangeClass, destinationName, isTemporary, scope, false);
    }

    protected AMQDestination(String exchangeName, String exchangeClass, String destinationName, boolean isTemporary,
                             String scope, boolean newQueue)
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
        _scope = scope;
        _newQueue = newQueue;
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
    
    public boolean newQueue() {
        return _newQueue;
    }

    public String getScope()
    {
        return _scope;
    }

    public String toString()
    {
        return "Scope: " + _scope + ", Destination: " + _destinationName + ", Exchange: " + _exchangeName +
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
        if (!_scope.equals(that._scope))
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
        result = 29 * result + _scope.hashCode();
        result = result * (_isTemporary?13:7);
        return result;
    }
}
