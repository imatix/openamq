package org.openamq.client;

import edu.emory.mathcs.backport.java.util.concurrent.SynchronousQueue;
import edu.emory.mathcs.backport.java.util.concurrent.TimeUnit;
import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.message.AbstractMessage;
import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.state.listener.SpecificMethodFrameListener;
import org.openamq.framing.AMQFrame;
import org.openamq.jms.MessageConsumer;
import org.openamq.jms.Session;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractMessageConsumer extends Closeable implements MessageConsumer
{
    private static final Logger _logger = Logger.getLogger(JMSMessageConsumer.class);

    private String _messageSelector;

    private boolean _noLocal;

    private AMQDestination _destination;

    private MessageListener _messageListener;

    /**
     * The consumer tag allows us to close the consumer by sending a jmsCancel method to the
     * broker
     */
    private int _consumerTag;

    /**
     * We need to know the channel id when constructing frames
     */
    private int _channelId;

    /**
     * Protects the setting of a messageListener
     */
    private final Object _syncLock = new Object();

    /**
     * When true indicates that either a message listener is set or that
     * a blocking receive call is in progress
     */
    private boolean _receiving;

    /**
     * Used in the blocking receive methods to receive a message from
     * the Session thread. Argument true indicates we want strict FIFO semantics
     */
    private final SynchronousQueue _synchronousQueue = new SynchronousQueue(true);

    private MessageFactoryRegistry _messageFactory;

    private AMQSession _session;

    private AMQProtocolHandler _protocolHandler;

    AbstractMessageConsumer(int channelId, AMQDestination destination, String messageSelector, boolean noLocal,
                            MessageFactoryRegistry messageFactory, AMQSession session, AMQProtocolHandler protocolHandler)
    {
        _channelId = channelId;
        _messageSelector = messageSelector;
        _noLocal = noLocal;
        _destination = destination;
        _messageFactory = messageFactory;
        _session = session;
        _protocolHandler = protocolHandler;
    }

    public String getMessageSelector() throws JMSException
    {
        return _messageSelector;
    }

    public MessageListener getMessageListener() throws JMSException
    {
        return _messageListener;
    }

    public void setMessageListener(MessageListener messageListener) throws JMSException
    {
        checkNotClosed();

        synchronized (_syncLock)
        {
            // If someone is already receiving
            if (_messageListener != null && _receiving)
            {
                throw new IllegalStateException("Another thread is already receiving...");
            }

            _messageListener = messageListener;

            _receiving = (_messageListener != null);

            if (_receiving)
            {
                _logger.debug("Message listener set for destination " + _destination);
            }
        }
    }

    public Message receive() throws JMSException
    {
        return receive(0);
    }

    public Message receive(long l) throws JMSException
    {
        checkNotClosed();

        synchronized (_syncLock)
        {
            // If someone is already receiving
            if (_receiving)
            {
                throw new IllegalStateException("Another thread is already receiving (possibly asynchronously)...");
            }

            _receiving = true;
        }

        try
        {
            Object o = null;
            if (l > 0)
            {
                o = _synchronousQueue.poll(l, TimeUnit.MILLISECONDS);
            }
            else
            {
                o = _synchronousQueue.take();
            }
            return returnMessageOrThrow(o);
        }
        catch (InterruptedException e)
        {
            return null;
        }
        finally
        {
            synchronized(_syncLock)
            {
                _receiving = false;
            }
        }
    }

    public Message receiveNoWait() throws JMSException
    {
        checkNotClosed();

        synchronized (_syncLock)
        {
            // If someone is already receiving
            if (_receiving)
            {
                throw new IllegalStateException("Another thread is already receiving (possibly asynchronously)...");
            }

            _receiving = true;
        }

        try
        {
            Object o = _synchronousQueue.poll();
            return returnMessageOrThrow(o);
        }
        finally
        {
            synchronized(_syncLock)
            {
                _receiving = false;
            }
        }
    }

    /**
     * We can get back either a Message or an exception from the queue. This method examines the argument and deals
     * with it by throwing it (if an exception) or returning it (in any other case).
     * @param o
     * @return a message only if o is a Message
     * @throws JMSException if the argument is a throwable. If it is a JMSException it is rethrown as is, but if not
     * a JMSException is created with the linked exception set appropriately
     */
    private Message returnMessageOrThrow(Object o)
            throws JMSException
    {
        // errors are passed via the queue too since there is no way of interrupting the poll() via the API.
        if (o instanceof Throwable)
        {
            JMSException e = new JMSException("Message consumer forcibly closed due to error: " + o);
            if (o instanceof Exception)
            {
                e.setLinkedException((Exception) o);
            }
            throw e;
        }
        else
        {
            return (Message) o;
        }
    }

    protected abstract AMQFrame createCancelFrame(int channelId, int consumerTag);

    protected abstract Class getCancelOkClass();

    public void close() throws JMSException
    {
        synchronized (_closingLock)
        {
            _closed.set(true);

            final AMQFrame cancelFrame = createCancelFrame(_channelId, _consumerTag);

            try
            {
                _protocolHandler.writeCommandFrameAndWaitForReply(cancelFrame,
                                                                  new SpecificMethodFrameListener(_channelId,
                                                                                                  getCancelOkClass()));
            }
            catch (AMQException e)
            {
                _logger.error("Error closing consumer: " + e, e);
                throw new JMSException("Error closing consumer: " + e);
            }

            deregisterConsumer();
        }
    }

    /**
     * Called from the AMQSession when a message has arrived for this consumer. This methods handles both the case
     * of a message listener or a synchronous receive() caller.
     * @param messageFrame the raw unprocessed mesage
     * @param acknowledgeMode the acknowledge mode requested for this message
     * @param channelId channel on which this message was sent
     */
    void notifyMessage(UnprocessedMessage messageFrame, int acknowledgeMode, int channelId)
    {
        if (_logger.isDebugEnabled())
        {
            _logger.debug("notifyMessage called with message number " + messageFrame.deliverBody.deliveryTag);
        }
        try
        {
            AbstractMessage jmsMessage = _messageFactory.createMessage(messageFrame.deliverBody.deliveryTag,
                                                                       messageFrame.deliverBody.redelivered,
                                                                       messageFrame.contentHeader,
                                                                       messageFrame.bodies);

            if (acknowledgeMode == Session.PRE_ACKNOWLEDGE)
            {
                _session.sendAcknowledgement(messageFrame.deliverBody.deliveryTag);
            }
            else if (acknowledgeMode == Session.CLIENT_ACKNOWLEDGE)
            {
                // we set the session so that when the user calls acknowledge() it can call the method on session
                // to send out the appropriate frame
                jmsMessage.setAMQSession(_session);
            }

            synchronized (_syncLock)
            {
                if (_messageListener != null)
                {
                    _messageListener.onMessage(jmsMessage);
                }
                else
                {
                    _synchronousQueue.put(jmsMessage);
                }
            }
            if (acknowledgeMode == Session.AUTO_ACKNOWLEDGE)
            {
                _session.sendAcknowledgement(messageFrame.deliverBody.deliveryTag);
            }
        }
        catch (Exception e)
        {
            _logger.error("Caught exception (dump follows) - ignoring...", e);
        }
    }

    void notifyError(Throwable cause)
    {
        synchronized (_syncLock)
        {
            _closed.set(true);

            // we have no way of propagating the exception to a message listener - a JMS limitation - so we
            // deal with the case where we have a synchronous receive() waiting for a message to arrive
            if (_messageListener == null)
            {
                // offer only succeeds if there is a thread waiting for an item from the queue
                if (_synchronousQueue.offer(cause))
                {
                    _logger.debug("Passed exception to synchronous queue for propagation to receive()");
                }
            }
            deregisterConsumer();
        }
    }

    /**
     * Perform cleanup to deregister this consumer. This occurs when closing the consumer in both the clean
     * case and in the case of an error occurring.
     */
    private void deregisterConsumer()
    {
        _session.deregisterConsumer(_consumerTag);
    }

    public int getConsumerTag()
    {
        return _consumerTag;
    }

    public void setConsumerTag(int consumerTag)
    {
        _consumerTag = consumerTag;
    }
}
