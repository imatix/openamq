package org.openamq.client;

import org.openamq.jms.MessageProducer;

import javax.jms.DeliveryMode;
import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import java.io.UnsupportedEncodingException;
import java.util.LinkedHashMap;

/**
 * An implementation of an AMQMessageProducer which extends the JMS Message Producer.
 */
public class AMQMessageProducer extends Closeable implements MessageProducer
{
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
    private Destination _destination;

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

    AMQMessageProducer(Destination destination, int handleId)
    {
        _destination = destination;
        _handleMap.put(destination, new Integer(handleId));
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
    }

    public void send(Message message) throws JMSException
    {
        checkNotClosed();

        // construct a HANDLE SEND frame for the default destination, priority etc.

    }

    public void send(Message message, int deliveryMode, int priority,
                     long timeToLive) throws JMSException
    {
        checkNotClosed();
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void send(Destination destination, Message message) throws JMSException
    {
        checkNotClosed();
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void send(Destination destination, Message message, int deliveryMode,
                     int priority,long timeToLive)
            throws JMSException
    {
        checkNotClosed();
        //To change body of implemented methods use File | Settings | File Templates.
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
