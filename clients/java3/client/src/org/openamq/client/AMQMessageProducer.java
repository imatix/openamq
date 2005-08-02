package org.openamq.client;

import org.apache.log4j.Logger;
import org.openamq.client.message.AbstractMessage;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.state.listener.SpecificMethodFrameListener;
import org.openamq.framing.*;
import org.openamq.jms.MessageProducer;
import org.openamq.AMQException;

import javax.jms.DeliveryMode;
import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import java.io.UnsupportedEncodingException;

/**
 * An implementation of an AMQMessageProducer which extends the JMS AbstractMessage Producer.
 */
public class AMQMessageProducer extends Closeable implements MessageProducer
{
    private static final Logger _logger = Logger.getLogger(AMQMessageProducer.class);

    /**
     * If true, messages will not get a timestamp.
     */
    private boolean _disableTimestamps;

    /**
     * Priority of messages created by this producer.
     */
    private int _messagePriority;

    /**
     * Time to live of messages. Specified in milliseconds but AMQ has 1 second resolution.
     */
    private long _timeToLive;

    /**
     * Delivery mode used for this producer.
     */
    private int _deliveryMode = DeliveryMode.PERSISTENT;

    /**
     * The Destination used for this consumer, if specified upon creation.
     */
    private AMQDestination _destination;

    /**
     * Default encoding used for messages produced by this producer.
     */
    private String _encoding;

    /**
     * Default encoding used for message produced by this producer.
     */
    private String _mimeType;

    private AMQProtocolHandler _protocolHandler;

    /**
     * True if this producer was created from a transacted session
     */
    private boolean _transacted;

    private int _channelId;

    private IdFactory _idFactory;

    /**
     * The session used to create this producer
     */
    private AMQSession _session;

    AMQMessageProducer(AMQDestination destination, boolean transacted, int channelId,
                       IdFactory idFactory, AMQSession session, AMQProtocolHandler protocolHandler)
            throws AMQException
    {
        _destination = destination;
        _transacted = transacted;
        _protocolHandler = protocolHandler;
        _channelId = channelId;
        _idFactory = idFactory;
        _session = session;
        if (destination != null)
        {
            declareDestination(destination);
        }
    }

    private void declareDestination(AMQDestination destination) throws AMQException
    {
        AMQFrame exchangeDeclareFrame = ExchangeDeclareBody.createAMQFrame(_channelId, 0, destination.getExchangeName(),
                                                                           destination.getExchangeClass(), false, false,
                                                                           false, false);
        _protocolHandler.writeCommandFrameAndWaitForReply(exchangeDeclareFrame,
                                                          new SpecificMethodFrameListener(_channelId,
                                                                                          ExchangeDeclareOkBody.class));

        AMQFrame queueDeclareFrame = QueueDeclareBody.createAMQFrame(_channelId, 0, "",
                                                                     destination.getDestinationName(), null, false,
                                                                     false, false, false);
        _protocolHandler.writeCommandFrameAndWaitForReply(queueDeclareFrame,
                                                          new SpecificMethodFrameListener(_channelId,
                                                                                          QueueDeclareOkBody.class));
    }

    public void setDisableMessageID(boolean b) throws JMSException
    {
        checkNotClosed();
        // IGNORED
    }

    public boolean getDisableMessageID() throws JMSException
    {
        checkNotClosed();
        // Always false for OpenAMQ
        return false;
    }

    public void setDisableMessageTimestamp(boolean b) throws JMSException
    {
        checkNotClosed();
        _disableTimestamps = b;
    }

    public boolean getDisableMessageTimestamp() throws JMSException
    {
        checkNotClosed();
        return _disableTimestamps;
    }

    public void setDeliveryMode(int i) throws JMSException
    {
        checkNotClosed();
        if (i != DeliveryMode.NON_PERSISTENT && i != DeliveryMode.PERSISTENT)
        {
            throw new JMSException("DeliveryMode must be either NON_PERSISTENT or PERSISTENT. Value of " + i +
                                   " is illegal");
        }
        _deliveryMode = i;
    }

    public int getDeliveryMode() throws JMSException
    {
        checkNotClosed();
        return _deliveryMode;
    }

    public void setPriority(int i) throws JMSException
    {
        checkNotClosed();
        if (i < 0 || i > 9)
        {
            throw new IllegalArgumentException("Priority of " + i + " is illegal. Value must be in range 0 to 9");
        }
        _messagePriority = i;
    }

    public int getPriority() throws JMSException
    {
        checkNotClosed();
        return _messagePriority;
    }

    public void setTimeToLive(long l) throws JMSException
    {
        checkNotClosed();
        if (l < 0)
        {
            throw new IllegalArgumentException("Time to live must be non-negative - supplied value was " + l);
        }
        _timeToLive = l;
    }

    public long getTimeToLive() throws JMSException
    {
        checkNotClosed();
        return _timeToLive;
    }

    public Destination getDestination() throws JMSException
    {
        checkNotClosed();
        return _destination;
    }

    public void close() throws JMSException
    {
        synchronized (_closingLock)
        {
            _closed.set(true);
        }
        /*final Iterator it = _handleMap.values().iterator();
        while (it.hasNext())
        {
            Integer handleId = (Integer) it.next();
           Handle.Close frame = new Handle.Close();
            frame.handleId = handleId.intValue();
//            try
//            {
//                _protocolHandler.writeCommandFrameAndWaitForReply(frame,
//                                                                  new HandleCloseListener(handleId.intValue()));
//            }
//            catch (AMQException e)
//            {
//                throw new JMSException("Error closing producer: " + e);
//            }
        }  */
    }

    public void send(Message message) throws JMSException
    {
        send(message, _deliveryMode, _messagePriority, _timeToLive);
    }

    public void send(Message message, int deliveryMode, int priority,
                     long timeToLive) throws JMSException
    {
        checkNotClosed();
        sendImpl(_destination, (AbstractMessage)message, deliveryMode, priority, timeToLive);
    }

    public void send(Destination destination, Message message) throws JMSException
    {
        send(destination, message, _deliveryMode, _messagePriority, _timeToLive);
    }

    public void send(Destination destination, Message message, int deliveryMode,
                     int priority, long timeToLive)
            throws JMSException
    {
        if (_destination != null)
        {
            throw new UnsupportedOperationException("Cannot specify destination in send method call when destination has " +
                                                    "already been specified at message producer construction time");
        }
        checkNotClosed();
        /*try
        {
            openHandle((AMQDestination) destination);
        }
        catch (AMQException e)
        {
            throw new JMSException("Error opening destination " + destination + ": " + e);
        } */
        sendImpl((AMQDestination) destination, (AbstractMessage) message, deliveryMode, priority, timeToLive);
    }

    private void sendImpl(AMQDestination destination, AbstractMessage message, int deliveryMode, int priority,
                          long timeToLive) throws JMSException
    {
        AMQFrame publishFrame = JmsPublishBody.createAMQFrame(_channelId, 0, _destination.getExchangeName(), true,
                                                              true); // TODO: add destination (queue) here

        long currentTime = 0;
        if (!_disableTimestamps)
        {
            currentTime = System.currentTimeMillis();
            message.setJMSTimestamp(currentTime);
        }
        byte[] payload = message.getData();
        JmsContentHeaderBody contentHeader = message.getContentHeader();
        contentHeader.weight = 0; // indicates no child content headers, just bodies
        contentHeader.destination = _destination.getDestinationName();                                
        if (timeToLive > 0)
        {
            if (!_disableTimestamps)
            {
                contentHeader.expiration = currentTime + timeToLive;
            }
        }
        else
        {
            contentHeader.expiration = 0;
        }

        AMQFrame contentHeaderFrame = ContentHeaderBody.createAMQFrame(_channelId, contentHeader);
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Sending content header frame to " + destination);
        }

        ContentBody messageBody = new ContentBody();
        messageBody.payload = payload;
        AMQFrame contentBodyFrame = ContentBody.createAMQFrame(_channelId, messageBody);
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Sending content body frame to " + destination);
        }
        contentHeader.bodySize = messageBody.getSize();
        AMQFrame[] frames = new AMQFrame[]{publishFrame, contentHeaderFrame, contentBodyFrame};
        CompositeAMQDataBlock compositeFrame = new CompositeAMQDataBlock(frames);
        _protocolHandler.writeFrame(compositeFrame);
    }

    public void setMimeType(String mimeType)
    {
        checkNotClosed();
        _mimeType = mimeType;
    }

    public void setEncoding(String encoding) throws UnsupportedEncodingException
    {
        checkNotClosed();
        _encoding = encoding;
    }
}
