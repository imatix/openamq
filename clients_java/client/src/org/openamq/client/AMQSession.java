package org.openamq.client;

import edu.emory.mathcs.backport.java.util.concurrent.BlockingQueue;
import edu.emory.mathcs.backport.java.util.concurrent.ConcurrentHashMap;
import edu.emory.mathcs.backport.java.util.concurrent.LinkedBlockingQueue;
import edu.emory.mathcs.backport.java.util.concurrent.atomic.AtomicBoolean;
import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.AMQUndeliveredException;
import org.openamq.AMQDisconnectedException;
import org.openamq.client.message.AbstractJMSMessage;
import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.state.listener.SpecificMethodFrameListener;
import org.openamq.framing.*;
import org.openamq.jms.Session;

import javax.jms.*;
import javax.jms.IllegalStateException;
import javax.jms.Queue;
import java.io.Serializable;
import java.util.*;

public class AMQSession extends Closeable implements Session, QueueSession, TopicSession
{
    private static final Logger _logger = Logger.getLogger(AMQSession.class);

    private static final int DEFAULT_PREFETCH = 1;

    private AMQConnection _connection;

    private boolean _transacted;

    private int _acknowledgeMode;

    private int _channelId;

    private int _defaultPrefetch = DEFAULT_PREFETCH;

    private BlockingQueue _queue = new LinkedBlockingQueue();

    private Dispatcher _dispatcher;

    private MessageFactoryRegistry _messageFactoryRegistry;

    /**
     * Set of all producers created by this session
     */
    private Map _producers = new ConcurrentHashMap();

    /**
     * Maps from consumer tag to JMSMessageConsumer instance
     */
    private Map _consumers = new ConcurrentHashMap();

    /**
     * Default value for immediate flag used by producers created by this session is false, i.e. a consumer does not
     * need to be attached to a queue
     */
    protected static final boolean DEFAULT_IMMEDIATE = false;

    /**
     * Default value for mandatory flag used by producers created by this sessio is true, i.e. server will not silently
     * drop messages where no queue is connected to the exchange for the message
     */
    protected static final boolean DEFAULT_MANDATORY = true;

    /**
     * The counter of the next producer id. This id is generated by the session and used only to allow the
     * producer to identify itself to the session when deregistering itself.
     *
     * Access to this id does not require to be synchronized since according to the JMS specification only one
     * thread of control is allowed to create producers for any given session instance.
     */
    private long _nextProducerId;

    /**
     * Responsible for decoding a message fragment and passing it to the appropriate message consumer.
     */
    private class Dispatcher extends Thread
    {
        private volatile AtomicBoolean _stopped = new AtomicBoolean(false);

        public void run()
        {
            UnprocessedMessage message;

            try
            {
                while (!_stopped.get() && (message = (UnprocessedMessage)_queue.take()) != null)
                {
                    //_queue.size()
                    dispatchMessage(message);
                }
            }
            catch(InterruptedException e)
            {
                ;
            }

            _logger.info("Dispatcher thread terminating for channel " + _channelId);
            _connection.exceptionReceived(new AMQDisconnectedException("Dispatcher thread terminating"));
        }

        private void dispatchMessage(UnprocessedMessage message)
        {
            if (message.deliverBody != null)
            {
                final BasicMessageConsumer consumer = (BasicMessageConsumer) _consumers.get(message.deliverBody.consumerTag);

                if (consumer == null)
                {
                    _logger.warn("Received a message from queue " + message.deliverBody.consumerTag + " without a handler - ignoring...");
                }
                else
                {
                    consumer.notifyMessage(message, _acknowledgeMode, _channelId);
                }
            }
            else
            {
                try
                {
                    // Bounced message is processed here, away from the mina thread
                    AbstractJMSMessage bouncedMessage = _messageFactoryRegistry.createMessage(0,
                                                                                           false,
                                                                                           message.contentHeader,
                                                                                           message.bodies);

                    int errorCode = message.bounceBody.replyCode;
                    String reason = message.bounceBody.replyText;
                    _logger.debug("Message returned with error code " + errorCode + " (" + reason + ")");

                    _connection.exceptionReceived(new AMQUndeliveredException(errorCode, "Error: " + reason, bouncedMessage));
                }
                catch (Exception e)
                {
                    _logger.error("Caught exception trying to raise undelivered message exception (dump follows) - ignoring...", e);
                }
            }
        }

        public void stopDispatcher()
        {
            _stopped.set(true);
            interrupt();
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
        _channelId = channelId;
        _messageFactoryRegistry = messageFactoryRegistry;
    }

    AMQSession(AMQConnection con, int channelId, boolean transacted, int acknowledgeMode)
    {
        this(con, channelId, transacted, acknowledgeMode, MessageFactoryRegistry.newDefaultRegistry());
    }

    AMQConnection getAMQConnection()
    {
        return _connection;
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

        /*Channel.Commit frame = new Channel.Commit();
        frame.channelId = _channelId;
        frame.confirmTag = 1;*/

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

        /*Channel.Rollback frame = new Channel.Rollback();
        frame.channelId = _channelId;
        frame.confirmTag = 1;*/

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

            // we pass null since this is not an error case
            closeProducersAndConsumers(null);

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
                _connection.deregisterSession(_channelId);
            }
        }
    }

    /**
     * Close all producers or consumers. This is called either in the error case or when closing the session normally.
     * @param amqe the exception, may be null to indicate no error has occurred
     */
    private void closeProducersAndConsumers(AMQException amqe)
    {
        try
        {
            closeProducers();
        }
        catch (JMSException e)
        {
            _logger.error("Error closing session: " + e, e);
        }
        try
        {
            closeConsumers(amqe);
        }
        catch (JMSException e)
        {
            _logger.error("Error closing session: " + e, e);
        }
    }

    /**
     * Called when the server initiates the closure of the session
     * unilaterally.
     * @param e the exception that caused this session to be closed
     */
    public void closed(Throwable e)
    {
        // An AMQException has an error code and message already and will be passed in when closure occurs as a
        // result of a channel close request
        AMQException amqe;
        if (e instanceof AMQException)
        {
            amqe = (AMQException) e;
        }
        else
        {
            amqe = new AMQException(_logger, "Closing session forcibly", e);
        }
        _connection.deregisterSession(_channelId);
        closeProducersAndConsumers(amqe);
    }

    /**
     * Called to close message producers cleanly. This may or may <b>not</b> be as a result of an error. There is
     * currently no way of propagating errors to message producers (this is a JMS limitation).
     */
    private void closeProducers() throws JMSException
    {
        // we need to clone the list of producers since the close() method updates the _producers collection
        // which would result in a concurrent modification exception
        final ArrayList clonedProducers = new ArrayList(_producers.values());

        final Iterator it = clonedProducers.iterator();
        while (it.hasNext())
        {
            final BasicMessageProducer prod = (BasicMessageProducer) it.next();
            prod.close();
        }
        // at this point the _producers map is empty
    }

    /**
     * Called to close message consumers cleanly. This may or may <b>not</b> be as a result of an error.
      * @param error not null if this is a result of an error occurring at the connection level
     */
    private void closeConsumers(Throwable error) throws JMSException
    {
        if (_dispatcher != null)
        {
            _dispatcher.stopDispatcher();
        }
        // we need to clone the list of consumers since the close() method updates the _consumers collection
        // which would result in a concurrent modification exception
        final ArrayList clonedConsumers = new ArrayList(_consumers.values());

        final Iterator it = clonedConsumers.iterator();
        while (it.hasNext())
        {
            final BasicMessageConsumer con = (BasicMessageConsumer) it.next();
            if (error != null)
            {
                con.notifyError(error);
            }
            else
            {
                con.close();
            }
        }
        // at this point the _consumers map will be empty
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

    public MessageProducer createProducer(Destination destination, boolean mandatory, boolean immediate)
            throws JMSException
    {
        return createProducerImpl(destination, mandatory, immediate);
    }

    public MessageProducer createProducer(Destination destination, boolean immediate)
            throws JMSException
    {
        return createProducerImpl(destination, DEFAULT_MANDATORY, immediate);
    }

    public MessageProducer createProducer(Destination destination) throws JMSException
    {
        return createProducerImpl(destination, DEFAULT_MANDATORY, DEFAULT_IMMEDIATE);
    }

    private org.openamq.jms.MessageProducer createProducerImpl(Destination destination, boolean mandatory,
                                                               boolean immediate)
            throws JMSException
    {
        synchronized (_closingLock)
        {
            checkNotClosed();

            AMQDestination amqd = (AMQDestination)destination;

            BasicMessageProducer producer = null;
            try
            {
                producer = new BasicMessageProducer(amqd, _transacted, _channelId,
                                                    this, _connection.getProtocolHandler(),
                                                    getNextProducerId(), immediate, mandatory);
            }
            catch (AMQException e)
            {
                _logger.error("Error creating message producer: " + e, e);
                final JMSException jmse = new JMSException("Error creating message producer");
                jmse.setLinkedException(e);
                throw jmse;

            }
            return producer;
        }
    }

    public MessageConsumer createConsumer(Destination destination) throws JMSException
    {
        return createConsumer(destination, _defaultPrefetch, false, false, null);
    }

    public MessageConsumer createConsumer(Destination destination, String messageSelector) throws JMSException
    {
        return createConsumer(destination, _defaultPrefetch, false, false, messageSelector);
    }

    public MessageConsumer createConsumer(Destination destination, String messageSelector, boolean noLocal)
            throws JMSException
    {
        return createConsumer(destination, _defaultPrefetch, noLocal, false, messageSelector);
    }

    public MessageConsumer createConsumer(Destination destination,
                                          int prefetch,
                                          boolean noLocal,
                                          boolean exclusive,
                                          String selector) throws JMSException
    {
        return createConsumer(destination, prefetch, noLocal, exclusive, selector, null);
    }

    public MessageConsumer createConsumer(Destination destination,
                                          int prefetch,
                                          boolean noLocal,
                                          boolean exclusive,
                                          String selector,
                                          FieldTable rawSelector) throws JMSException
    {
        return createConsumerImpl(destination, prefetch, noLocal, exclusive,
                                  selector, rawSelector);
    }

    protected MessageConsumer createConsumerImpl(Destination destination,
                                                 int prefetch,
                                                 boolean noLocal,
                                                 boolean exclusive,
                                                 String selector,
                                                 FieldTable rawSelector) throws JMSException
    {
        synchronized (_closingLock)
        {
            checkNotClosed();

            AMQDestination amqd = (AMQDestination)destination;

            final AMQProtocolHandler protocolHandler = _connection.getProtocolHandler();
            BasicMessageConsumer consumer = new BasicMessageConsumer(_channelId, amqd, selector, noLocal,
                                                                     _messageFactoryRegistry, this, protocolHandler);

            String consumerTag = null;
            try
            {
                // Declare exchange
                AMQFrame exchangeDeclare = ExchangeDeclareBody.createAMQFrame(_channelId, 0, amqd.getExchangeName(),
                                                                              amqd.getExchangeClass(), false, false,
                                                                              false, false, null);

                protocolHandler.writeCommandFrameAndWaitForReply(exchangeDeclare,
                                            new SpecificMethodFrameListener(_channelId, ExchangeDeclareOkBody.class));

                AMQFrame queueDeclare = QueueDeclareBody.createAMQFrame(_channelId, 0,
                                                                        amqd.getQueueName(),
                                                                        false, false, amqd.isExclusive(),
                                                                        amqd.isAutoDelete(), null);

                AMQMethodEvent evt = protocolHandler.writeCommandFrameAndWaitForReply(queueDeclare,
                                             new SpecificMethodFrameListener(_channelId, QueueDeclareOkBody.class));
                QueueDeclareOkBody qdb = (QueueDeclareOkBody) evt.getMethod();
                amqd.setQueueName(qdb.queue);

                // Bind exchange to queue
                // TODO: construct the rawSelector from the selector string if rawSelector == null
                final FieldTable ft = new FieldTable();
                //if (rawSelector != null)
                //    ft.put("headers", rawSelector.getDataAsBytes());
                if (rawSelector != null)
                    ft.putAll(rawSelector);
                AMQFrame queueBind = QueueBindBody.createAMQFrame(_channelId, 0,
                                                                  amqd.getQueueName(), amqd.getExchangeName(),
                                                                  amqd.getRoutingKey(), ft);

                protocolHandler.writeCommandFrameAndWaitForReply(queueBind,
                                              new SpecificMethodFrameListener(_channelId, QueueBindOkBody.class));

                // Consume from queue
                AMQFrame jmsConsume = BasicConsumeBody.createAMQFrame(_channelId, 0,
                                                                      qdb.queue, null, 0,
                                                                      prefetch, noLocal, true, exclusive);

                AMQMethodEvent consumeOkEvent = protocolHandler.writeCommandFrameAndWaitForReply(jmsConsume,
                                                 new SpecificMethodFrameListener(_channelId,
                                                                                 BasicConsumeOkBody.class));

                consumerTag = ((BasicConsumeOkBody) consumeOkEvent.getMethod()).consumerTag;
                consumer.setConsumerTag(consumerTag);
                registerConsumer(consumerTag, consumer);
              }
              catch (AMQException e)
              {
                  if (consumerTag != null)
                  {
                    deregisterConsumer(consumerTag);
                  }
                  throw new JMSException("Error creating consumer: " + e);
              }

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
     * Send an acknowledgement for all messages up to a specified number on this session.
     * @param messageNbr the message number up to an including which all messages will be acknowledged.
     */
    public void sendAcknowledgement(long messageNbr)
    {
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Channel Ack being sent for channel id " + _channelId + " and message number " + messageNbr);
        }
        /*Channel.Ack frame = new Channel.Ack();
        frame.channelId = _channelId;
        frame.messageNbr = messageNbr;
        _connection.getProtocolHandler().writeFrame(frame);*/
    }

    void start()
    {
        _dispatcher = new Dispatcher();
        _dispatcher.setDaemon(true);
        _dispatcher.start();
    }

    void registerConsumer(String consumerTag, MessageConsumer consumer)
    {
        _consumers.put(consumerTag, consumer);
    }

    /**
     * Called by the MessageConsumer when closing, to deregister the consumer from the
     * map from consumerTag to consumer instance.
     * @param consumerTag the consumer tag, that was broker-generated
     */
    void deregisterConsumer(String consumerTag)
    {
        _consumers.remove(consumerTag);
    }

    void registerProducer(long producerId, MessageProducer producer)
    {
        _producers.put(new Long(producerId), producer);
    }

    void deregisterProducer(long producerId)
    {
        _producers.remove(new Long(producerId));
    }

    private long getNextProducerId()
    {
        return ++_nextProducerId;
    }
}
