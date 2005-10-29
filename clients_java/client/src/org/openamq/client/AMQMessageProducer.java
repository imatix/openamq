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

    /**
     * Default value for immediate flag is false, i.e. a consumer does not need to be attached to a queue
     */
    private static final boolean DEFAULT_IMMEDIATE = false;

    /**
     * Default value for mandatory flag is true, i.e. server will not silently drop messages where no queue is
     * connected to the exchange for the message
     */
    private static final boolean DEFAULT_MANDATORY = true;

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
        // Declare the exchange
        // Note that the durable and internal arguments are ignored since passive is set to false
        AMQFrame exchangeDeclareFrame = ExchangeDeclareBody.createAMQFrame(_channelId, 0, destination.getExchangeName(),
                                                                           destination.getExchangeClass(), false,
                                                                           false, false, false);
        _protocolHandler.writeCommandFrameAndWaitForReply(exchangeDeclareFrame,
                                                          new SpecificMethodFrameListener(_channelId,
                                                                                          ExchangeDeclareOkBody.class));        
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
        _closed.set(true);        
    }

    public void send(Message message) throws JMSException
    {
        sendImpl(_destination, (AbstractMessage) message, _deliveryMode, _messagePriority, _timeToLive,
                 DEFAULT_MANDATORY, DEFAULT_IMMEDIATE);
    }

    public void send(Message message, int deliveryMode, int priority,
                     long timeToLive) throws JMSException
    {
        sendImpl(_destination, (AbstractMessage)message, deliveryMode, priority, timeToLive, DEFAULT_MANDATORY,
                 DEFAULT_IMMEDIATE);
    }

    public void send(Destination destination, Message message) throws JMSException
    {
        checkNotClosed();
        validateDestination(destination);
        sendImpl((AMQDestination) destination, (AbstractMessage) message, _deliveryMode, _messagePriority, _timeToLive,
                 DEFAULT_MANDATORY, DEFAULT_IMMEDIATE);
    }

    public void send(Destination destination, Message message, int deliveryMode,
                     int priority, long timeToLive)
            throws JMSException
    {
        checkNotClosed();
        validateDestination(destination);
        sendImpl((AMQDestination) destination, (AbstractMessage) message, deliveryMode, priority, timeToLive,
                 DEFAULT_MANDATORY, DEFAULT_IMMEDIATE);
    }

    private void validateDestination(Destination destination) throws JMSException
    {
        if (!(destination instanceof AMQDestination))
        {
            throw new JMSException("Unsupported destination class: " +
                                   (destination != null?destination.getClass():null));
        }
        try
        {
            declareDestination((AMQDestination)destination);
        }
        catch (AMQException e)
        {
            throw new JMSException("Unable to declare destination " + destination + ": " + e);
        }
    }

    private void sendImpl(AMQDestination destination, AbstractMessage message, int deliveryMode, int priority,
                          long timeToLive, boolean mandatory, boolean immediate) throws JMSException
    {
        AMQFrame publishFrame = JmsPublishBody.createAMQFrame(_channelId, 0, _destination.getExchangeName(),
                                                              _destination.getDestinationName(), mandatory,
                                                              immediate);

        long currentTime = 0;
        if (!_disableTimestamps)
        {
            currentTime = System.currentTimeMillis();
            message.setJMSTimestamp(currentTime);
        }
        byte[] payload = message.getData();
        JmsContentHeaderBody contentHeader = message.getContentHeader();
        contentHeader.weight = 0; // indicates no child content headers, just bodies
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
        contentHeader.deliveryMode = (byte) deliveryMode;
        contentHeader.priority = (byte) priority;

        AMQFrame contentHeaderFrame = ContentHeaderBody.createAMQFrame(_channelId, contentHeader);
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Sending content header frame to " + destination);
        }

        ContentBody[] contentBodies = createContentBodies(payload);
        AMQFrame[] frames = new AMQFrame[2 + contentBodies.length];
        for (int i = 0; i < contentBodies.length; i++)
        {
            frames[2 + i] = ContentBody.createAMQFrame(_channelId, contentBodies[i]);
        }
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Sending content body frames to " + destination);
        }
        contentHeader.bodySize = payload.length;

        frames[0] = publishFrame;
        frames[1] = contentHeaderFrame;
        CompositeAMQDataBlock compositeFrame = new CompositeAMQDataBlock(frames);
        _protocolHandler.writeFrame(compositeFrame);
    }

    /**
     * Create content bodies. This will split a large message into numerous bodies depending on the negotiated
     * maximum frame size.
     * @param payload
     * @return the array of content bodies
     */
    private ContentBody[] createContentBodies(byte[] payload)
    {
        if (payload == null)
        {
            return null;
        }
        // we substract one from the total frame maximum size to account for the end of frame marker in a body frame
        // (0xCE byte).
        final long framePayloadMax = _session.getAMQConnection().getMaximumFrameSize() - 1;
        int frameCount = (int) (payload.length/framePayloadMax) + 1;
        final ContentBody[] bodies = new ContentBody[frameCount];
        if (frameCount == 1)
        {
            bodies[0] = new ContentBody();
            bodies[0].payload = payload;
        }
        else
        {
            long remaining = payload.length;
            for (int i = 0; i < bodies.length; i++)
            {
                bodies[i] = new ContentBody();
                byte[] framePayload = new byte[(remaining >= framePayloadMax)?(int)framePayloadMax:(int)remaining];
                System.arraycopy(payload, (int)framePayloadMax * i, framePayload, 0, framePayload.length);
                bodies[i].payload = framePayload;
                remaining -= framePayload.length;
            }
        }
        return bodies;
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
