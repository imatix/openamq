package org.openamq.client;

import org.openamq.jms.Session;
import org.openamq.client.framing.Handle;
import org.openamq.client.framing.AMQMessage;
import org.openamq.client.state.listener.HandleReplyListener;

import javax.jms.*;
import javax.jms.IllegalStateException;
import java.io.Serializable;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;

import org.apache.log4j.*;

public class AMQSession extends Closeable implements Session
{
    private static final Logger _logger = Logger.getLogger(AMQSession.class);

    private AMQConnection _connection;

    private boolean _transacted;

    private int _acknowledgeMode;

    private IdFactory _idFactory;

    private int _channelId;

    /**
     * Maps from handle id (Integer) to AMQMessageProducer instance
     */
    private Map _producers = new LinkedHashMap();

    /**
     * Maps from handle id (Integer) to AMQMessageConsumer instance
     */
    private Map _consumers = new LinkedHashMap();

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

    public javax.jms.Message createMessage() throws JMSException
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

        return new Message();
    }

    public TextMessage createTextMessage(String text) throws JMSException
    {
        checkNotClosed();
        return new Message(text);
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
                    Handle.Open frame = createHandleOpenFrame(destination,true, 1);
                    AMQMessageProducer producer = new AMQMessageProducer((AMQDestination) destination, frame.handleId, _connection.getProtocolHandler());
                    _producers.put(new Integer(frame.handleId), producer);
                    _connection.getProtocolHandler().writeFrameToSession(frame, new HandleReplyListener(frame.handleId));
                    return producer;
                }
    }

    private Handle.Open createHandleOpenFrame(Destination destination,boolean producer, int confirmTag)
    {
        int handleId = _idFactory.getHandleId();
        Handle.Open frame = new Handle.Open();
        frame.channelId = _channelId;
        frame.handleId = handleId;
        frame.producer = producer;
        frame.consumer = !producer;
        frame.confirmTag = confirmTag;
        frame.temporary = true;
        frame.destName = destination.toString();
        return frame;
    }

    public MessageConsumer createConsumer(Destination destination) throws JMSException
    {
        return (null);
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

    public MessageConsumer createQueueConsumer(
            Destination destination,
            int prefetch,
            boolean noLocal,
            boolean noAck,
            boolean dynamic,
            boolean exclusive,
            String selector) throws JMSException
    {
        synchronized (_closingLock)
        {
            checkNotClosed();

            Handle.Open frame = createHandleOpenFrame(destination,false, 1);
            AMQMessageConsumer consumer = new AMQMessageConsumer(frame.handleId, destination, (String) null, noLocal);
            _consumers.put(new Integer(frame.handleId), consumer);

            _connection.getProtocolHandler().writeFrameToSession(frame, new HandleReplyListener(frame.handleId));

            Handle.Consume consumeFrame = new Handle.Consume();
            consumeFrame.handleId = frame.handleId;
            consumeFrame.confirmTag = 1;
            consumeFrame.prefetch = prefetch;
            consumeFrame.noLocal = noLocal;
            consumeFrame.noAck = noAck;
            consumeFrame.dynamic = dynamic;
            consumeFrame.exclusive = exclusive;
            consumeFrame.destName = destination.toString();    // ?
            consumeFrame.selector = null;

            _connection.getProtocolHandler().addSessionByHandle(frame.handleId, this);

            _connection.getProtocolHandler().writeFrameToSession(consumeFrame, new HandleReplyListener(frame.handleId));
            return (consumer);
        }
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

    public void notifyHandle(int handleId, AMQMessage messageFragment)
    {
        AMQMessageConsumer consumer = (AMQMessageConsumer) _consumers.get(new Integer(handleId));

        if (consumer == null)
        {
            _logger.warn("Received a message for handleId " + handleId + " without a handler - ignoring...");
        }
        else
        {
            consumer.notifyMessage(messageFragment);
        }

        return;
    }
}
