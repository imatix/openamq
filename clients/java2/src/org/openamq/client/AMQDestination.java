package org.openamq.client;

import javax.jms.JMSException;
import javax.jms.Queue;
import javax.jms.Topic;


public class AMQDestination implements Queue, Topic
{

    private String _name;
    
    public String getQueueName() throws JMSException
    {
        return _name;
    }

    public String getTopicName() throws JMSException
    {
        return getQueueName();
    }
}
