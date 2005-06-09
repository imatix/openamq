package org.openamq.client;

import javax.jms.JMSException;
import javax.jms.Queue;
import javax.jms.Topic;

import org.openamq.client.framing.Handle;


public class AMQDestination implements Queue, Topic
{

    private String _name;
    private boolean _topic;

    public AMQDestination(String name)
    {
    	this(name,false);
    }
    
    public AMQDestination(String name,boolean isTopic)
    {
        _name = name;
        _topic = isTopic;
    }

    public boolean isTopic()
    {
    	return(_topic);
    	
    }
    public String getQueueName() throws JMSException
    {
        return _name;
    }

    public String getTopicName() throws JMSException
    {
        return getQueueName();
    }
    
    public int getServiceType()
    {
    	return(_topic ? Handle.Open.SERVICE_TYPE_TOPIC : Handle.Open.SERVICE_TYPE_QUEUE);
    }

    public String toString()
    {
        return _name;
    }
}
