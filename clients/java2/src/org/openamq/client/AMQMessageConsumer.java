package org.openamq.client;

import org.openamq.jms.MessageConsumer;

import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;

/**
 */
public class AMQMessageConsumer implements MessageConsumer
{

    private String _messageSelector;

    private boolean _noLocal;

    private Destination _destination;

    /**
     * Used for concurrency control
     */
    private final Object _lock = new Object();

    AMQMessageConsumer(Destination destination, String messageSelector, boolean noLocal)
    {
        _messageSelector = messageSelector;
    }

    public String getMessageSelector() throws JMSException
    {
        return _messageSelector;
    }

    public MessageListener getMessageListener() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setMessageListener(MessageListener messageListener) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public Message receive() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Message receive(long l) throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Message receiveNoWait() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void close() throws JMSException
    {
        synchronized (_lock)
        {

        }
    }
}
