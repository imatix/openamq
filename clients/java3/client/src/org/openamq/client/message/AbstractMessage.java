package org.openamq.client.message;

import org.apache.commons.collections.map.ReferenceMap;
import org.openamq.client.AMQQueue;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQTopic;
import org.openamq.client.AMQDestination;
import org.openamq.framing.FieldTable;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.AMQException;

import javax.jms.Destination;
import javax.jms.JMSException;
import java.util.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractMessage implements javax.jms.Message
{
    private static final Map _destinationCache = Collections.synchronizedMap(new ReferenceMap());

//    protected Map _messageProperties;

    public static final char BOOLEAN_PROPERTY_PREFIX = 'B';
    public static final char BYTE_PROPERTY_PREFIX = 'b';
    public static final char SHORT_PROPERTY_PREFIX = 's';
    public static final char INT_PROPERTY_PREFIX = 'i';
    public static final char LONG_PROPERTY_PREFIX = 'l';
    public static final char FLOAT_PROPERTY_PREFIX = 'f';
    public static final char DOUBLE_PROPERTY_PREFIX = 'd';
    public static final char STRING_PROPERTY_PREFIX = 'S';

    protected long _messageNbr;

    protected final JmsContentHeaderBody _contentHeader;

    private String exchangeClass;

    protected boolean _redelivered;

    /**
     * If the acknowledge mode is CLIENT_ACKNOWLEDGE the session is required
     */
    protected AMQSession _session;

    protected AbstractMessage()
    {
        _contentHeader = new JmsContentHeaderBody();
    }

    protected AbstractMessage(long messageNbr, JmsContentHeaderBody contentHeader) throws AMQException
    {
        this(contentHeader);
        _messageNbr = messageNbr;
    }

    protected AbstractMessage(JmsContentHeaderBody contentHeader) throws AMQException
    {
        _contentHeader = contentHeader;        
    }

    public String getJMSMessageID() throws JMSException
    {
        if (_contentHeader.messageId == null)
        {
            _contentHeader.messageId = "ID:" + _messageNbr;
        }
        return _contentHeader.messageId;
    }

    public void setJMSMessageID(String messageId) throws JMSException
    {
        _contentHeader.messageId = messageId;
    }

    public long getJMSTimestamp() throws JMSException
    {
        return new Long(_contentHeader.timestamp).longValue();
    }

    public void setJMSTimestamp(long timestamp) throws JMSException
    {
        _contentHeader.timestamp = timestamp;
    }

    public byte[] getJMSCorrelationIDAsBytes() throws JMSException
    {
        return _contentHeader.correlationId.getBytes();
    }

    public void setJMSCorrelationIDAsBytes(byte[] bytes) throws JMSException
    {
        _contentHeader.correlationId = new String(bytes);
    }

    public void setJMSCorrelationID(String correlationId) throws JMSException
    {
        _contentHeader.correlationId = correlationId;
    }

    public String getJMSCorrelationID() throws JMSException
    {
        return _contentHeader.correlationId;
    }

    public Destination getJMSReplyTo() throws JMSException
    {
        String replyToEncoding = _contentHeader.replyTo;
        if (replyToEncoding == null)
        {
            return null;
        }
        else
        {
            Destination dest = (Destination) _destinationCache.get(replyToEncoding);
            if (dest == null)
            {
                char destType = replyToEncoding.charAt(0);
                if (destType == 'Q')
                {
                    dest = new AMQQueue(replyToEncoding.substring(1));
                }
                else if (destType == 'T')
                {
                    dest = new AMQTopic(replyToEncoding.substring(1));
                }
                else
                {
                    throw new JMSException("Illegal value in JMS_ReplyTo property: " + replyToEncoding);
                }
                _destinationCache.put(replyToEncoding, dest);
            }
            return dest;
        }
    }

    public void setJMSReplyTo(Destination destination) throws JMSException
    {
        if (destination == null)
        {
            throw new IllegalArgumentException("Null destination not allowed");
        }
        if (!(destination instanceof AMQDestination))
        {
            throw new IllegalArgumentException("ReplyTo destination my be an AMQ destination - passed argument was type " +
                                               destination.getClass());
        }
        final AMQDestination amqd = (AMQDestination) destination;

        final String encodedDestination = amqd.getEncodedName();
        _destinationCache.put(encodedDestination, destination);
        _contentHeader.replyTo = encodedDestination;
    }

    public Destination getJMSDestination() throws JMSException
    {
        // TODO: implement this once we have sorted out how to figure out the exchange class
        return null;
    }

    public void setJMSDestination(Destination destination) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public int getJMSDeliveryMode() throws JMSException
    {
        return _contentHeader.deliveryMode;
    }

    public void setJMSDeliveryMode(int i) throws JMSException
    {
        _contentHeader.deliveryMode = (byte) i;
    }

    public boolean getJMSRedelivered() throws JMSException
    {
        return _redelivered;
    }

    public void setJMSRedelivered(boolean b) throws JMSException
    {
        _redelivered = b;
    }

    public String getJMSType() throws JMSException
    {
        return getMimeType();
    }

    public void setJMSType(String string) throws JMSException
    {
        throw new JMSException("Cannot set JMS Type - it is implicitly defined based on message type");
    }

    public long getJMSExpiration() throws JMSException
    {
        return new Long(_contentHeader.expiration).longValue();
    }

    public void setJMSExpiration(long l) throws JMSException
    {
        _contentHeader.expiration = l;
    }

    public int getJMSPriority() throws JMSException
    {
        return _contentHeader.priority;
    }

    public void setJMSPriority(int i) throws JMSException
    {
        _contentHeader.priority = (byte)i;
    }

    public void clearProperties() throws JMSException
    {
        if (_contentHeader.headers != null)
        {
            _contentHeader.headers.clear();
        }
    }

    public boolean propertyExists(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return false;
        }
        else
        {
            // TODO: fix this
            return _contentHeader.headers.containsKey(LONG_PROPERTY_PREFIX  + propertyName);
        }
    }

    public boolean getBooleanProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return Boolean.valueOf(null).booleanValue();
        }
        else
        {
            Boolean b = (Boolean) _contentHeader.headers.get(BOOLEAN_PROPERTY_PREFIX + propertyName);

            if (b == null)
            {
                return Boolean.valueOf(null).booleanValue();
            }
            else
            {
                return b.booleanValue();
            }
        }
    }

    public byte getByteProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return Byte.valueOf(null).byteValue();
        }
        else
        {
            Byte b = (Byte) _contentHeader.headers.get(BYTE_PROPERTY_PREFIX + propertyName);
            if (b == null)
            {
                return Byte.valueOf(null).byteValue();
            }
            else
            {
                return b.byteValue();
            }
        }
    }

    public short getShortProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return Short.valueOf(null).shortValue();
        }
        else
        {
            Short s = (Short) _contentHeader.headers.get(SHORT_PROPERTY_PREFIX + propertyName);
            if (s == null)
            {
                return Short.valueOf(null).shortValue();
            }
            else
            {
                return s.shortValue();
            }
        }
    }

    public int getIntProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return Integer.valueOf(null).intValue();
        }
        else
        {
            Integer i = (Integer) _contentHeader.headers.get(INT_PROPERTY_PREFIX + propertyName);
            if (i == null)
            {
                return Integer.valueOf(null).intValue();
            }
            else
            {
                return i.intValue();
            }
        }
    }

    public long getLongProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return Long.valueOf(null).longValue();
        }
        else
        {
            Long l = (Long) _contentHeader.headers.get(LONG_PROPERTY_PREFIX + propertyName);
            if (l == null)
            {
                // temp - the spec says do this but this throws a NumberFormatException
                //return Long.valueOf(null).longValue();
                return 0;
            }
            else
            {
                return l.longValue();
            }
        }
    }

    public float getFloatProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return Float.valueOf(null).floatValue();
        }
        else
        {
            final Float f = (Float) _contentHeader.headers.get(FLOAT_PROPERTY_PREFIX + propertyName);
            if (f == null)
            {
                return Float.valueOf(null).floatValue();
            }
            else
            {
                return f.floatValue();
            }
        }
    }

    public double getDoubleProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return Double.valueOf(null).doubleValue();
        }
        else
        {
            final Double d = (Double) _contentHeader.headers.get(DOUBLE_PROPERTY_PREFIX + propertyName);
            if (d == null)
            {
                return Double.valueOf(null).doubleValue();
            }
            else
            {
                return d.shortValue();
            }
        }
    }

    public String getStringProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_contentHeader.headers == null)
        {
            return null;
        }
        else
        {
            return (String) _contentHeader.headers.get(STRING_PROPERTY_PREFIX + propertyName);
        }
    }

    public Object getObjectProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        throw new JMSException("Not implemented yet");
    }

    public Enumeration getPropertyNames() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setBooleanProperty(String propertyName, boolean b) throws JMSException
    {
        checkPropertyName(propertyName);
        _contentHeader.headers.put(BOOLEAN_PROPERTY_PREFIX + propertyName, Boolean.valueOf(b));
    }

    public void setByteProperty(String propertyName, byte b) throws JMSException
    {
        checkPropertyName(propertyName);
        _contentHeader.headers.put(BYTE_PROPERTY_PREFIX + propertyName, new Byte(b));
    }

    public void setShortProperty(String propertyName, short i) throws JMSException
    {
        checkPropertyName(propertyName);
        _contentHeader.headers.put(SHORT_PROPERTY_PREFIX + propertyName, new Short(i));
    }

    public void setIntProperty(String propertyName, int i) throws JMSException
    {
        checkPropertyName(propertyName);
        _contentHeader.headers.put(INT_PROPERTY_PREFIX + propertyName, new Integer(i));
    }

    public void setLongProperty(String propertyName, long l) throws JMSException
    {
        checkPropertyName(propertyName);
        _contentHeader.headers.put(LONG_PROPERTY_PREFIX + propertyName, new Long(l));
    }

    public void setFloatProperty(String propertyName, float f) throws JMSException
    {
        checkPropertyName(propertyName);
        _contentHeader.headers.put(FLOAT_PROPERTY_PREFIX + propertyName,  new Float(f));
    }

    public void setDoubleProperty(String propertyName, double v) throws JMSException
    {
        checkPropertyName(propertyName);
        _contentHeader.headers.put(DOUBLE_PROPERTY_PREFIX + propertyName, new Double(v));
    }

    public void setStringProperty(String propertyName, String value) throws JMSException
    {
        checkPropertyName(propertyName);
        createPropertyMapIfRequired();
        propertyName = STRING_PROPERTY_PREFIX + propertyName;
        _contentHeader.headers.put(propertyName, value);
    }

    private void createPropertyMapIfRequired()
    {
        if (_contentHeader.headers == null)
        {
            _contentHeader.headers = new FieldTable();
        }
    }

    public void setObjectProperty(String string, Object object) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void acknowledge() throws JMSException
    {
        // the JMS 1.1 spec says in section 3.6 that calls to acknowledge are ignored when client acknowledge
        // is not specified. In our case, we only set the session field where client acknowledge mode is specified.
        if (_session != null)
        {
            _session.sendAcknowledgement(_messageNbr);
        }
    }

    public abstract void clearBody() throws JMSException;

    /**
     * Get a String representation of the body of the message. Used in the
     * toString() method which outputs this before message properties.
     */
    public abstract String toBodyString() throws JMSException;

    /**
     * Return the raw byte array that is used to populate the frame when sending
     * the message.
     * @return a byte array of message data
     * @throws JMSException if something goes wrong.
     */
    public abstract byte[] getData() throws JMSException;

    public abstract String getMimeType();

    public String toString()
    {
        try
        {
            StringBuffer buf = new StringBuffer("Body:\n");
            buf.append(toBodyString());
            buf.append("\nJMS timestamp: ").append(getJMSTimestamp());
            buf.append("\nJMS expiration: ").append(getJMSExpiration());
            buf.append("\nJMS priority: ").append(getJMSPriority());
            buf.append("\nJMS delivery mode: ").append(getJMSDeliveryMode());
            buf.append("\nJMS reply to: ").append(String.valueOf(getJMSReplyTo()));
            buf.append("\nAMQ message number: ").append(_messageNbr);
            buf.append("\nProperties:");
            if (_contentHeader.headers == null)
            {
                buf.append("<NONE>");
            }
            else
            {
                final Iterator it = _contentHeader.headers.entrySet().iterator();
                while (it.hasNext())
                {
                    final Map.Entry entry = (Map.Entry) it.next();
                    final String propertyName = (String) entry.getKey();
                    if (propertyName == null)
                    {
                        buf.append("\nInternal error: Property with NULL key defined");
                    }
                    else
                    {
                        buf.append('\n').append(propertyName.substring(1));

                        char typeIdentifier = propertyName.charAt(0);
                        switch (typeIdentifier)
                        {
                            case org.openamq.client.message.AbstractMessage.BOOLEAN_PROPERTY_PREFIX:
                                buf.append("<boolean> ");
                                break;
                            case org.openamq.client.message.AbstractMessage.BYTE_PROPERTY_PREFIX:
                                buf.append("<byte> ");
                                break;
                            case org.openamq.client.message.AbstractMessage.SHORT_PROPERTY_PREFIX:
                                buf.append("<short> ");
                                break;
                            case org.openamq.client.message.AbstractMessage.INT_PROPERTY_PREFIX:
                                buf.append("<int> ");
                                break;
                            case org.openamq.client.message.AbstractMessage.LONG_PROPERTY_PREFIX:
                                buf.append("<long> ");
                                break;
                            case org.openamq.client.message.AbstractMessage.FLOAT_PROPERTY_PREFIX:
                                buf.append("<float> ");
                                break;
                            case org.openamq.client.message.AbstractMessage.DOUBLE_PROPERTY_PREFIX:
                                buf.append("<double> ");
                                break;
                            case org.openamq.client.message.AbstractMessage.STRING_PROPERTY_PREFIX:
                                buf.append("<string> ");
                                break;
                             default:
                                buf.append("<unknown type (identifier " +
                                           typeIdentifier + ") ");
                        }
                        buf.append(String.valueOf(entry.getValue()));
                    }
                }
            }
            return buf.toString();
        }
        catch (JMSException e)
        {
            return e.toString();
        }
    }

    Map getUnderlyingMessagePropertiesMap()
    {
        return _contentHeader.headers;
    }

    void setUnderlyingMessagePropertiesMap(FieldTable messageProperties)
    {
        _contentHeader.headers = messageProperties;
    }

    private void checkPropertyName(String propertyName)
    {
        if (propertyName == null)
        {
            throw new IllegalArgumentException("Property name must not be null");
        }
        else if ("".equals(propertyName))
        {
            throw new IllegalArgumentException("Property name must not be the empty string");
        }

        if (_contentHeader.headers == null)
        {
            _contentHeader.headers = new FieldTable();
        }
    }

    public FieldTable populateHeadersFromMessageProperties()
    {
        if (_contentHeader.headers == null)
        {
            return null;
        }
        else
        {
            //
            // We need to convert every property into a String representation
            // Note that type information is preserved in the property name
            //
            final FieldTable table = new FieldTable();
            final Iterator entries = _contentHeader.headers.entrySet().iterator();
            while (entries.hasNext())
            {
                final Map.Entry entry = (Map.Entry) entries.next();
                final String propertyName = (String) entry.getKey();
                if (propertyName == null)
                {
                    continue;
                }
                else
                {
                    table.put(propertyName, entry.getValue().toString());
                }
            }
            return table;
        }
    }

    /**
     * The session is set when CLIENT_ACKNOWLEDGE mode is used so that the CHANNEL ACK can be sent when the user
     * calls acknowledge()
     * @param s the AMQ session that delivered this message
     */
    public void setAMQSession(AMQSession s)
    {
        _session = s;
    }

    /**
     * Get the AMQ message number assigned to this message
     * @return the message number
     */
    public long getMessageNbr()
    {
        return _messageNbr;
    }

    public void setMessageNbr(long messageNbr)
    {
        _messageNbr = messageNbr;
    }

    public JmsContentHeaderBody getContentHeader()
    {
        return _contentHeader;
    }
}
