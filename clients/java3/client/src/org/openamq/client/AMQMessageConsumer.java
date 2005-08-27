package org.openamq.client;

import edu.emory.mathcs.backport.java.util.concurrent.SynchronousQueue;
import edu.emory.mathcs.backport.java.util.concurrent.TimeUnit;
import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.AMQUndeliveredException;
import org.openamq.client.message.AbstractMessage;
import org.openamq.client.message.MessageFactoryRegistry;
import org.openamq.client.message.UnprocessedMessage;
import org.openamq.client.protocol.AMQConstant;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.ChannelCloseOkBody;
import org.openamq.jms.MessageConsumer;
import org.openamq.jms.Session;

import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;


/**
 */
public class AMQMessageConsumer extends Closeable implements MessageConsumer
{
    private static final Logger _logger = Logger.getLogger(AMQMessageConsumer.class);

    private String _messageSelector;

    private boolean _noLocal;

    private AMQDestination _destination;

    private MessageListener _messageListener;

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
     * the Session thread.
     */
    private final SynchronousQueue _synchronousQueue = new SynchronousQueue();

    private MessageFactoryRegistry _messageFactory;

    private AMQSession _session;

    private AMQProtocolHandler _protocolHandler;

    AMQMessageConsumer(AMQDestination destination, String messageSelector, boolean noLocal,
                       MessageFactoryRegistry messageFactory, AMQSession session, AMQProtocolHandler protocolHandler)
    {
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
            if (l > 0)
            {
                return (Message) _synchronousQueue.poll(l, TimeUnit.MILLISECONDS);
            }
            else
            {
                return (Message) _synchronousQueue.take();
            }
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
            return (Message) _synchronousQueue.poll();
        }
        finally
        {
            synchronized(_syncLock)
            {
                _receiving = false;
            }
        }
    }

    public void close() throws JMSException
    {
        synchronized (_closingLock)
        {
            _closed.set(true);
        }

        _session.deregisterConsumerQueue(_destination.getDestinationName());
        /*Handle.Close frame = new Handle.Close();
        frame.handleId = _handleId;*/
//        try
//        {
//            _protocolHandler.writeCommandFrameAndWaitForReply(frame,
//                                                              new HandleCloseListener(_handleId));
//        }
//        catch (AMQException e)
//        {
//            throw new JMSException("Error closing consumer: " + e);
//        }
    }

    public void notifyMessage(UnprocessedMessage messageFrame, int acknowledgeMode, int channelId)
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
}
