package org.openamq.client;

import org.openamq.jms.Session;

import javax.jms.*;
import java.io.Serializable;
import java.lang.IllegalStateException;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;

public class AMQSession extends Closeable implements Session
{
    private AMQConnection _connection;
    
    private boolean _transacted;
    
    private int _acknowledgeMode;
    
    private IdFactory _idFactory;
    
    private int _channelId;

    /**
     * Maps from handle id (Integer) to AMQMessageProducer instance
     */
    private LinkedHashMap _producers;

    /**
     * Maps from handle id (Integer) to AMQMessageConsumer instance
     */
    private LinkedHashMap _consumers;

    AMQSession(AMQConnection con, int channelId, boolean transacted, int acknowledgeMode)
    {
        _connection = con;
        _transacted = transacted;
        if (transacted)
        {
            _acknowledgeMode = Session.SESSION_TRANSACTED;
        }
        else
        {
            _acknowledgeMode = acknowledgeMode;
        }
        _idFactory = con.getIdFactory();
        _channelId = channelId;
    }
    
    public BytesMessage createBytesMessage() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public MapMessage createMapMessage() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public Message createMessage() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public ObjectMessage createObjectMessage() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public ObjectMessage createObjectMessage(Serializable object) throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public StreamMessage createStreamMessage() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public TextMessage createTextMessage() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public TextMessage createTextMessage(String text) throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public boolean getTransacted() throws JMSException
    {
        checkNotClosed();
        return _transacted;
    }

    public int getAcknowledgeMode() throws JMSException
    {
        checkNotClosed();
        return _acknowledgeMode;
    }

    public void commit() throws JMSException
    {
        checkNotClosed();
        checkTransacted(); // throws IllegalStateException if not a transacted session

        // TODO: add logic
    }

    public void rollback() throws JMSException
    {
        checkNotClosed();
        checkTransacted(); // throws IllegalStateException if not a transacted session

        // TODO: add logic
    }

    public void close() throws JMSException
    {
        // We must close down all producers and consumers in an orderly fashion. This is the only method
        // that can be called from a different thread of control from the one controlling the session

        synchronized (_closingLock)
        {
            _closed.set(true);

            final Iterator producers = _producers.entrySet().iterator();
            while (producers.hasNext())
            {
                final Map.Entry entry = (Map.Entry) producers.next();
                AMQMessageProducer producer = (AMQMessageProducer) entry.getValue();
                producer.close();
            }

            _producers.clear();

            final Iterator consumers = _consumers.entrySet().iterator();
            while (consumers.hasNext())
            {
                final Map.Entry entry = (Map.Entry) consumers.next();
                AMQMessageConsumer consumer = (AMQMessageConsumer) entry.getValue();
                consumer.close();
            }

            _consumers.clear();
        }
    }

    public void recover() throws JMSException
    {
        checkNotClosed();
        checkNotTransacted(); // throws IllegalStateException if not a transacted session

        // TODO: This cannot be implemented using 0.8 semantics
    }

    public MessageListener getMessageListener() throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub
        return null;
    }

    public void setMessageListener(MessageListener listener) throws JMSException
    {
        checkNotClosed();
        // TODO Auto-generated method stub

    }

    public void run()
    {
        // TODO Auto-generated method stub

    }

    public MessageProducer createProducer(Destination destination) throws JMSException
    {
        synchronized (_closingLock)
        {
            checkNotClosed();
            int handleId = _idFactory.getHandleId();
            AMQMessageProducer producer = new AMQMessageProducer(destination, handleId);
            _producers.put(new Integer(handleId), producer);
            return producer;
        }
    }

    public MessageConsumer createConsumer(Destination destination) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public MessageConsumer createConsumer(Destination destination, String messageSelector) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public MessageConsumer createConsumer(Destination destination, String messageSelector, boolean NoLocal)
                                    throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public Queue createQueue(String queueName) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public Topic createTopic(String topicName) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public TopicSubscriber createDurableSubscriber(Topic topic, String name) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public TopicSubscriber createDurableSubscriber(Topic topic, String name, String messageSelector, boolean noLocal)
                                    throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public QueueBrowser createBrowser(Queue queue) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public QueueBrowser createBrowser(Queue queue, String messageSelector) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public TemporaryQueue createTemporaryQueue() throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public TemporaryTopic createTemporaryTopic() throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public void unsubscribe(String name) throws JMSException
    {
        // TODO Auto-generated method stub

    }

    private void checkTransacted() throws JMSException
    {
        if (!getTransacted())
        {
            throw new IllegalStateException("Session is not transacted");
        }
    }

    private void checkNotTransacted() throws JMSException
    {
        if (getTransacted())
        {
            throw new IllegalStateException("Session is transacted");
        }
    }

}
