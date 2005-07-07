package org.openamq.client;

import javax.jms.Destination;

import org.openamq.client.framing.Handle;


public abstract class AMQDestination implements Destination
{
    private final String _name;

    private final int _serviceType;

    public AMQDestination(String name, int serviceType)
    {
        if (name == null)
        {
            throw new IllegalArgumentException("Destination name must not be null");
        }
        _name = name;
        _serviceType = serviceType;
    }

    public boolean isTopic()
    {
    	return _serviceType == Handle.Open.SERVICE_TYPE_TOPIC;
    }

    public boolean isQueue()
    {
        return _serviceType == Handle.Open.SERVICE_TYPE_QUEUE;
    }

    public int getServiceType()
    {
    	return _serviceType;
    }

    public String getName()
    {
        return _name;
    }
    
    public String toString()
    {
        return "Destination: " + _name + ", Type: " + (isQueue()?"Queue":"Topic");
    }

    public boolean equals(Object o)
    {
        if (!(o instanceof AMQDestination))
        {
            return false;
        }
        AMQDestination d = (AMQDestination) o;
        return _serviceType == d._serviceType && _name.equals(d._name);
    }
}
