package org.openamq.jms;

import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.MessageConsumer;


public interface Session extends javax.jms.Session
{
	public MessageConsumer createQueueConsumer(
    		Destination destination,
    		int prefetch,
    		boolean noLocal,
    		boolean noAck,
    		boolean dynamic,
    		boolean exclusive,
    		String selector) throws JMSException;
	
	public MessageConsumer createTopicConsumer(
    		Destination destination,	// Topic or null for default CBR topic (recommended for now).
    		int prefetch,
    		boolean noLocal,	
    		boolean noAck, 
    		boolean dynamic,
    		boolean exclusive,
    		
    		String selector) throws JMSException;
}
