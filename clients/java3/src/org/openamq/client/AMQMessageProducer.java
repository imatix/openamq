package org.openamq.client;

import org.openamq.jms.MessageProducer;
import org.openamq.client.framing.Handle;
import org.openamq.client.framing.AMQMessage;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.state.listener.HandleReplyListener;
import org.openamq.client.state.listener.HandleCloseListener;
import org.openamq.client.message.AbstractMessage;
import org.apache.log4j.Logger;

import javax.jms.DeliveryMode;
import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import java.io.UnsupportedEncodingException;
import java.util.LinkedHashMap;
import java.util.Iterator;

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

    /**
     * Maps from Destination to handle id. For all sends that are invokved with a Destination, a handle is opened
     * for the duration of this message producer.
     */
    private LinkedHashMap _handleMap = new LinkedHashMap();

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

    AMQMessageProducer(AMQDestination destination, boolean transacted, int handleId, AMQProtocolHandler protocolHandler,
                       int channelId, IdFactory idFactory, AMQSession session)
    {
        _destination = destination;
        if (destination != null)
        {
            _handleMap.put(destination, new Integer(handleId));
        }
        _transacted = transacted;
        _protocolHandler = protocolHandler;
        _channelId = channelId;
        _idFactory = idFactory;
        _session = session;
    }

    AMQMessageProducer(boolean transacted, AMQProtocolHandler protocolHandler, int channelId, IdFactory idFactory,
                       AMQSession session)
    {
        this(null, transacted, -1, protocolHandler, channelId, idFactory, session);
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
        final Iterator it = _handleMap.values().iterator();
        while (it.hasNext())
        {
            Integer handleId = (Integer) it.next();
            Handle.Close frame = new Handle.Close();
            frame.handleId = handleId.intValue();
            try
            {
                _protocolHandler.writeCommandFrameAndWaitForReply(frame,
                                                                  new HandleCloseListener(handleId.intValue()));
            }
            catch (AMQException e)
            {
                throw new JMSException("Error closing producer: " + e);
            }
        }
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
        try
        {
            openHandle((AMQDestination) destination);
        }
        catch (AMQException e)
        {
            throw new JMSException("Error opening destination " + destination + ": " + e);
        }
        sendImpl((AMQDestination) destination, (AbstractMessage) message, deliveryMode, priority, timeToLive);
    }

    private void sendImpl(AMQDestination destination, AbstractMessage message, int deliveryMode, int priority,
                          long timeToLive) throws JMSException
    {
        long currentTime = 0;
        if (!_disableTimestamps)
        {
            currentTime = System.currentTimeMillis();
            message.setJMSTimestamp(currentTime);
        }
        byte[] payload = message.getData();
        AMQMessage msg = new AMQMessage();
        msg.bodySize = payload.length;
        msg.persistent = (deliveryMode == DeliveryMode.PERSISTENT);
        msg.message = payload;
        msg.headers = message.populateHeadersFromMessageProperties();
        msg.mimeType = message.getMimeType();
        msg.priority = (byte)priority;
        if (timeToLive > 0)
        {
            if (!_disableTimestamps)
            {
                msg.expiration = currentTime + timeToLive;
            }
        }
        else
        {
            msg.expiration = 0;
        }

        final Handle.Send frame = new Handle.Send();
        frame.handleId = ((Integer) _handleMap.get(destination)).intValue();
        frame.fragmentSize = msg.getSize();
        frame.confirmTag = 0;
        frame.serviceType = destination.getServiceType();
        frame.destName = destination.getName();

        frame.message = msg;

        if (_logger.isDebugEnabled())
        {
            _logger.debug("Sending frame to send message to " + frame.destName);
        }

        _protocolHandler.writeFrame(frame);

        if (_logger.isDebugEnabled())
        {
            _logger.debug("Sent frame and received acknowledgement");
        }
    }

    private void openHandle(AMQDestination amqd) throws AMQException
    {
        Handle.Open frame = createHandleOpenFrame(true, 1);
        _protocolHandler.writeCommandFrameAndWaitForReply(frame,
                                                          new HandleReplyListener(frame.handleId));
        _handleMap.put(amqd, new Integer(frame.handleId));
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
