package org.openamq.client;

import org.openamq.jms.Session;
import org.openamq.client.framing.Handle;
import org.openamq.client.framing.Channel;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.client.message.MessageFactoryRegistry;

import javax.jms.*;
import javax.jms.IllegalStateException;
import javax.jms.Queue;
import java.io.Serializable;
import java.util.*;

import org.apache.log4j.*;
import edu.emory.mathcs.backport.java.util.concurrent.BlockingQueue;
import edu.emory.mathcs.backport.java.util.concurrent.LinkedBlockingQueue;
import edu.emory.mathcs.backport.java.util.concurrent.ConcurrentHashMap;

public class AMQSession extends Closeable implements Session, QueueSession, TopicSession
{
    private static final Logger _logger = Logger.getLogger(AMQSession.class);

    private static final int DEFAULT_PREFETCH = 1;

    private AMQConnection _connection;

    private boolean _transacted;

    private int _acknowledgeMode;

    private IdFactory _idFactory;

    private int _channelId;

    private int _defaultPrefetch = DEFAULT_PREFETCH;

    private BlockingQueue _queue = new LinkedBlockingQueue();

    private Dispatcher _dispatcher;

    private MessageFactoryRegistry _messageFactoryRegistry;

    /**
     * Set of all producers created by this session
     */
    private Set _producers = new HashSet();

    /**
     * Maps from handle id (Integer) to AMQMessageConsumer instance
     */
    private Map _consumers = new ConcurrentHashMap();

    /**
     * Responsible for decoding a message fragment and passing it to the appropriate message consumer
     */
    private class Dispatcher implements Runnable
    {
        public void run()
        {
            UnprocessedMessage message;

            try
            {
                while ((message = (UnprocessedMessage)_queue.take()) != null)
                {
                    dispatchMessage(message);
                }
            }
            catch(InterruptedException e)
            {
                ;
            }

            _logger.info("Dispatcher thread terminating...");
        }

        private void dispatchMessage(UnprocessedMessage message)
        {
            final AMQMessageConsumer consumer = (AMQMessageConsumer) _consumers.get(new Integer(message.handleId));

            if (consumer == null)
            {
                _logger.warn("Received a message for handleId " + message.handleId + " without a handler - ignoring...");
            }
            else
            {
                consumer.notifyMessage(message, _acknowledgeMode, _channelId);
            }
        }
    }

    AMQSession(AMQConnection con, int channelId, boolean transacted, int acknowledgeMode,
               MessageFactoryRegistry messageFactoryRegistry)
    {
        _connection = con;
        _transacted = transacted;
        if (transacted)
        {
            _acknowledgeMode = javax.jms.Session.SESSION_TRANSACTED;
        }
        else
        {
            _acknowledgeMode = acknowledgeMode;
        }
        _idFactory = con.getIdFactory();
        _channelId = channelId;
        _messageFactoryRegistry = messageFactoryRegistry;
    }

    AMQSession(AMQConnection con, int channelId, boolean transacted, int acknowledgeMode)
    {
        this(con, channelId, transacted, acknowledgeMode, MessageFactoryRegistry.newDefaultRegistry());
    }

    public BytesMessage createBytesMessage() throws JMSException
    {
        checkNotClosed();
        try
        {
            return (BytesMessage) _messageFactoryRegistry.createMessage("application/octet-stream");
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to create message: " + e);
        }
    }

    public MapMessage createMapMessage() throws JMSException
    {
        checkNotClosed();
        try
        {
            return (MapMessage) _messageFactoryRegistry.createMessage("jms/map-message");
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to create message: " + e);
        }
    }

    public javax.jms.Message createMessage() throws JMSException
    {
        checkNotClosed();
        try
        {
            return (BytesMessage) _messageFactoryRegistry.createMessage("application/octet-stream");
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to create message: " + e);
        }
    }

    public ObjectMessage createObjectMessage() throws JMSException
    {
        checkNotClosed();
        try
        {
            return (ObjectMessage) _messageFactoryRegistry.createMessage("application/java-object-stream");
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to create message: " + e);
        }
    }

    public ObjectMessage createObjectMessage(Serializable object) throws JMSException
    {
        checkNotClosed();
        try
        {
            ObjectMessage msg = (ObjectMessage) _messageFactoryRegistry.createMessage("application/java-object-stream");
            msg.setObject(object);
            return msg;
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to create message: " + e);
        }
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

        try
        {
            return (TextMessage) _messageFactoryRegistry.createMessage("text/plain");
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to create message: " + e);
        }
    }

    public TextMessage createTextMessage(String text) throws JMSException
    {
        checkNotClosed();
        try
        {
            TextMessage msg = (TextMessage) _messageFactoryRegistry.createMessage("text/plain");
            msg.setText(text);
            return msg;
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to create message: " + e);
        }
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

        Channel.Commit frame = new Channel.Commit();
        frame.channelId = _channelId;
        frame.confirmTag = 1;

//        try
//        {
//            _connection.getProtocolHandler().writeCommandFrameAndWaitForReply(frame, new ChannelReplyListener(_channelId));
//        }
//        catch (AMQException e)
//        {
//            throw new JMSException("Error creating session: " + e);
//        }
        _logger.info("Transaction commited on channel " + _channelId);
    }

    public void rollback() throws JMSException
    {
        checkNotClosed();
        checkTransacted(); // throws IllegalStateException if not a transacted session

        Channel.Rollback frame = new Channel.Rollback();
        frame.channelId = _channelId;
        frame.confirmTag = 1;

//        try
//        {
//            _connection.getProtocolHandler().writeCommandFrameAndWaitForReply(frame, new ChannelReplyListener(_channelId));
//        }
//        catch (AMQException e)
//        {
//            throw new JMSException("Error rolling back session: " + e);
//        }
        _logger.info("Transaction rolled back on channel " + _channelId);
    }

    public void close() throws JMSException
    {
        // We must close down all producers and consumers in an orderly fashion. This is the only method
        // that can be called from a different thread of control from the one controlling the session

        synchronized (_closingLock)
        {
            _closed.set(true);

            try
            {
                _connection.getProtocolHandler().closeSession(this);
            }
            catch (AMQException e)
            {
                throw new JMSException("Error closing session: " + e);
            }
            finally
            {
                _producers.clear();
                _consumers.clear();
            }
        }
    }

    /**
     * Called when the server initiates the closure of the session
     * unilaterally.
     * @param code the code (reason code) given by the server
     * @param text some text giving an explanation, suitable for putting into
     * an exception
     */
    public void closed(int code, String text)
    {
        _logger.error("Session closed by server: (" + code + ") Message; " + text);        
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

            AMQDestination amqd = (AMQDestination)destination;

            // TODO: move this handle sending into the message producer since we
            // do not need to have knowledge of the handle id here
            Handle.Open frame = createHandleOpenFrame(true, 1);

            AMQMessageProducer producer = new AMQMessageProducer(amqd, _transacted, frame.handleId,
                                                                 _connection.getProtocolHandler(), _channelId,
                                                                 _idFactory, this);
//            try
//            {
//                _connection.getProtocolHandler().writeCommandFrameAndWaitForReply(frame,
//                                                                                  new HandleReplyListener(frame.handleId));
//            }
//            catch (AMQException e)
//            {
//                throw new JMSException("Error creating producer: " + e);
//            }
            return producer;
        }
    }

    private Handle.Open createHandleOpenFrame(boolean producer, int confirmTag)
    {
        int handleId = _idFactory.getHandleId();
        Handle.Open frame = new Handle.Open();
        frame.channelId = _channelId;
        frame.handleId = handleId;
        frame.producer = producer;
        frame.consumer = !producer;
        frame.confirmTag = confirmTag;
        return frame;
    }

    public MessageConsumer createConsumer(Destination destination) throws JMSException
    {
        return createConsumer(destination, _defaultPrefetch, false, false, false, null);
    }

    public MessageConsumer createConsumer(Destination destination, String messageSelector) throws JMSException
    {
        return createConsumer(destination, _defaultPrefetch, false, false, false, messageSelector);
    }

    public MessageConsumer createConsumer(Destination destination, String messageSelector, boolean noLocal)
            throws JMSException
    {
        return createConsumer(destination, _defaultPrefetch, noLocal, false, false, messageSelector);
    }

    public MessageConsumer createConsumer(
            Destination destination,
            int prefetch,
            boolean noLocal,
            boolean dynamic,
            boolean exclusive,
            String selector) throws JMSException
    {
        synchronized (_closingLock)
        {
            checkNotClosed();

            AMQDestination amqd = (AMQDestination)destination;

            Handle.Open frame = createHandleOpenFrame(false, 1);
            AMQMessageConsumer consumer = new AMQMessageConsumer(frame.handleId, amqd, selector, noLocal,
                                                                 _messageFactoryRegistry, this,
                                                                 _connection.getProtocolHandler());
            registerConsumerHandleId(frame.handleId, consumer);

//            try
//            {
//                _connection.getProtocolHandler().writeCommandFrameAndWaitForReply(frame,
//                                                                                  new HandleReplyListener(frame.handleId));
//
//                Handle.Consume consumeFrame = new Handle.Consume();
//                consumeFrame.handleId = frame.handleId;
//                consumeFrame.confirmTag = 1;
//                consumeFrame.serviceType = amqd.getServiceType();
//                consumeFrame.prefetch = prefetch;
//                consumeFrame.noLocal = noLocal;
//                consumeFrame.noAck = (_acknowledgeMode == Session.NO_ACKNOWLEDGE);
//                consumeFrame.dynamic = dynamic;
//                consumeFrame.exclusive = exclusive;
//                consumeFrame.destName = amqd.getName();
//
//                if (selector != null)
//                {
//                    try
//                    {
//                        consumeFrame.selector = EncodingUtils.createFieldTableFromMessageSelector(selector);
//                    }
//                    catch(IllegalArgumentException e)
//                    {
//                        throw new JMSException("Failed to parse message selector: " + e);
//                    }
//                }
//                else
//                {
//                    consumeFrame.selector = null;
//                }
//
//                _connection.getProtocolHandler().writeCommandFrameAndWaitForReply(consumeFrame,
//                                                                                  new HandleReplyListener(frame.handleId));
//            }
//            catch (AMQException e)
//            {
//                deregisterConsumerHandleId(frame.handleId);
//                throw new JMSException("Error creating consumer: " + e);
//            }

            return consumer;
        }
    }

    public Queue createQueue(String queueName) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public QueueReceiver createReceiver(Queue queue) throws JMSException
    {
        return (QueueReceiver) createConsumer(queue);
    }

    public QueueReceiver createReceiver(Queue queue, String messageSelector) throws JMSException
    {
        return (QueueReceiver) createConsumer(queue, messageSelector);
    }

    public QueueSender createSender(Queue queue) throws JMSException
    {
        return (QueueSender) createProducer(queue);
    }

    public Topic createTopic(String topicName) throws JMSException
    {
        // TODO Auto-generated method stub
        return null;
    }

    public TopicSubscriber createSubscriber(Topic topic) throws JMSException
    {
        return (TopicSubscriber) createConsumer(topic);
    }

    public TopicSubscriber createSubscriber(Topic topic, String messageSelector, boolean noLocal) throws JMSException
    {
        return (TopicSubscriber) createConsumer(topic, messageSelector, noLocal);
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

    public TopicPublisher createPublisher(Topic topic) throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
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

    public void messageReceived(UnprocessedMessage message)
    {
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Message received in session with channel id " + _channelId);
        }
        try
        {
            _queue.put(message);
        }
        catch (InterruptedException e)
        {
            ;
        }
    }

    public int getDefaultPrefetch()
    {
        return _defaultPrefetch;
    }

    public void setDefaultPrefetch(int defaultPrefetch)
    {
        _defaultPrefetch = defaultPrefetch;
    }

    public int getChannelId()
    {
        return _channelId;
    }

    /**
     * @return the list of handle ids used by this session's consumers. This
     * is used by the protocol session when closing the session.
     */
    public List getHandleIds()
    {
        final List handles = new ArrayList(_consumers.size());
        handles.addAll(_consumers.keySet());
        return handles;
    }

    /**
     * Send an acknowledgement for all messages up to a specified number on this session.
     * @param messageNbr the message number up to an including which all messages will be acknowledged.
     */
    public void sendAcknowledgement(long messageNbr)
    {
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Channel Ack being sent for channel id " + _channelId + " and message number " + messageNbr);
        }
        Channel.Ack frame = new Channel.Ack();
        frame.channelId = _channelId;
        frame.messageNbr = messageNbr;
        _connection.getProtocolHandler().writeFrame(frame);
    }

    void start()
    {
        _dispatcher = new Dispatcher();
        Thread t = new Thread(_dispatcher);
        t.setDaemon(true);
        t.start();
    }

    void registerConsumerHandleId(int handleId, MessageConsumer consumer)
    {
        _consumers.put(new Integer(handleId), consumer);
        _connection.getProtocolHandler().addSessionByHandle(handleId, this);
    }

    void deregisterConsumerHandleId(int handleId)
    {
        _connection.getProtocolHandler().removeSessionByHandle(handleId);
        _consumers.remove(new Integer(handleId));
    }
}
