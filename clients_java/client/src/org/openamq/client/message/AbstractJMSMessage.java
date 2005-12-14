package org.openamq.client.message;

import org.apache.commons.collections.map.ReferenceMap;
import org.openamq.AMQException;
import org.openamq.client.AMQDestination;
import org.openamq.client.AMQQueue;
import org.openamq.client.AMQTopic;
import org.openamq.framing.FieldTable;
import org.openamq.framing.BasicContentHeaderProperties;

import javax.jms.Destination;
import javax.jms.JMSException;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Map;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractJMSMessage extends AMQMessage implements javax.jms.Message
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

    protected boolean _redelivered;

    protected AbstractJMSMessage()
    {
        super(new BasicContentHeaderProperties());
    }

    protected AbstractJMSMessage(long messageNbr, BasicContentHeaderProperties contentHeader) throws AMQException
    {
        this(contentHeader);
        _messageNbr = messageNbr;
    }

    protected AbstractJMSMessage(BasicContentHeaderProperties contentHeader) throws AMQException
    {
        super(contentHeader);
    }

    public String getJMSMessageID() throws JMSException
    {
        if (getJmsContentHeaderProperties().messageId == null)
        {
            getJmsContentHeaderProperties().messageId = "ID:" + _messageNbr;
        }
        return getJmsContentHeaderProperties().messageId;
    }

    public void setJMSMessageID(String messageId) throws JMSException
    {
        getJmsContentHeaderProperties().messageId = messageId;
    }

    public long getJMSTimestamp() throws JMSException
    {
        return new Long(getJmsContentHeaderProperties().timestamp).longValue();
    }

    public void setJMSTimestamp(long timestamp) throws JMSException
    {
        getJmsContentHeaderProperties().timestamp = timestamp;
    }

    public byte[] getJMSCorrelationIDAsBytes() throws JMSException
    {
        return getJmsContentHeaderProperties().correlationId.getBytes();
    }

    public void setJMSCorrelationIDAsBytes(byte[] bytes) throws JMSException
    {
        getJmsContentHeaderProperties().correlationId = new String(bytes);
    }

    public void setJMSCorrelationID(String correlationId) throws JMSException
    {
        getJmsContentHeaderProperties().correlationId = correlationId;
    }

    public String getJMSCorrelationID() throws JMSException
    {
        return getJmsContentHeaderProperties().correlationId;
    }

    public Destination getJMSReplyTo() throws JMSException
    {
        String replyToEncoding = getJmsContentHeaderProperties().replyTo;
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
        getJmsContentHeaderProperties().replyTo = encodedDestination;
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
        return getJmsContentHeaderProperties().deliveryMode;
    }

    public void setJMSDeliveryMode(int i) throws JMSException
    {
        getJmsContentHeaderProperties().deliveryMode = (byte) i;
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
        return new Long(getJmsContentHeaderProperties().expiration).longValue();
    }

    public void setJMSExpiration(long l) throws JMSException
    {
        getJmsContentHeaderProperties().expiration = l;
    }

    public int getJMSPriority() throws JMSException
    {
        return getJmsContentHeaderProperties().priority;
    }

    public void setJMSPriority(int i) throws JMSException
    {
        getJmsContentHeaderProperties().priority = (byte)i;
    }

    public void clearProperties() throws JMSException
    {
        if (getJmsContentHeaderProperties().headers != null)
        {
            getJmsContentHeaderProperties().headers.clear();
        }
    }

    public boolean propertyExists(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (getJmsContentHeaderProperties().headers == null)
        {
            return false;
        }
        else
        {
            // TODO: fix this
            return getJmsContentHeaderProperties().headers.containsKey(STRING_PROPERTY_PREFIX  + propertyName);
        }
    }

    public boolean getBooleanProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (getJmsContentHeaderProperties().headers == null)
        {
            return Boolean.valueOf(null).booleanValue();
        }
        else
        {
            Boolean b = (Boolean) getJmsContentHeaderProperties().headers.get(BOOLEAN_PROPERTY_PREFIX + propertyName);

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
        if (getJmsContentHeaderProperties().headers == null)
        {
            return Byte.valueOf(null).byteValue();
        }
        else
        {
            Byte b = (Byte) getJmsContentHeaderProperties().headers.get(BYTE_PROPERTY_PREFIX + propertyName);
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
        if (getJmsContentHeaderProperties().headers == null)
        {
            return Short.valueOf(null).shortValue();
        }
        else
        {
            Short s = (Short) getJmsContentHeaderProperties().headers.get(SHORT_PROPERTY_PREFIX + propertyName);
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
        if (getJmsContentHeaderProperties().headers == null)
        {
            return Integer.valueOf(null).intValue();
        }
        else
        {
            Integer i = (Integer) getJmsContentHeaderProperties().headers.get(INT_PROPERTY_PREFIX + propertyName);
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
        if (getJmsContentHeaderProperties().headers == null)
        {
            return Long.valueOf(null).longValue();
        }
        else
        {
            Long l = (Long) getJmsContentHeaderProperties().headers.get(LONG_PROPERTY_PREFIX + propertyName);
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
        if (getJmsContentHeaderProperties().headers == null)
        {
            return Float.valueOf(null).floatValue();
        }
        else
        {
            final Float f = (Float) getJmsContentHeaderProperties().headers.get(FLOAT_PROPERTY_PREFIX + propertyName);
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
        if (getJmsContentHeaderProperties().headers == null)
        {
            return Double.valueOf(null).doubleValue();
        }
        else
        {
            final Double d = (Double) getJmsContentHeaderProperties().headers.get(DOUBLE_PROPERTY_PREFIX + propertyName);
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
        if (getJmsContentHeaderProperties().headers == null)
        {
            return null;
        }
        else
        {
            return (String) getJmsContentHeaderProperties().headers.get(STRING_PROPERTY_PREFIX + propertyName);
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
        getJmsContentHeaderProperties().headers.put(BOOLEAN_PROPERTY_PREFIX + propertyName, Boolean.valueOf(b));
    }

    public void setByteProperty(String propertyName, byte b) throws JMSException
    {
        checkPropertyName(propertyName);
        getJmsContentHeaderProperties().headers.put(BYTE_PROPERTY_PREFIX + propertyName, new Byte(b));
    }

    public void setShortProperty(String propertyName, short i) throws JMSException
    {
        checkPropertyName(propertyName);
        getJmsContentHeaderProperties().headers.put(SHORT_PROPERTY_PREFIX + propertyName, new Short(i));
    }

    public void setIntProperty(String propertyName, int i) throws JMSException
    {
        checkPropertyName(propertyName);
        getJmsContentHeaderProperties().headers.put(INT_PROPERTY_PREFIX + propertyName, new Integer(i));
    }

    public void setLongProperty(String propertyName, long l) throws JMSException
    {
        checkPropertyName(propertyName);
        getJmsContentHeaderProperties().headers.put(LONG_PROPERTY_PREFIX + propertyName, new Long(l));
    }

    public void setFloatProperty(String propertyName, float f) throws JMSException
    {
        checkPropertyName(propertyName);
        getJmsContentHeaderProperties().headers.put(FLOAT_PROPERTY_PREFIX + propertyName,  new Float(f));
    }

    public void setDoubleProperty(String propertyName, double v) throws JMSException
    {
        checkPropertyName(propertyName);
        getJmsContentHeaderProperties().headers.put(DOUBLE_PROPERTY_PREFIX + propertyName, new Double(v));
    }

    public void setStringProperty(String propertyName, String value) throws JMSException
    {
        checkPropertyName(propertyName);
        createPropertyMapIfRequired();
        propertyName = STRING_PROPERTY_PREFIX + propertyName;
        getJmsContentHeaderProperties().headers.put(propertyName, value);
    }

    private void createPropertyMapIfRequired()
    {
        if (getJmsContentHeaderProperties().headers == null)
        {
            getJmsContentHeaderProperties().headers = new FieldTable();
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
            if (getJmsContentHeaderProperties().headers == null)
            {
                buf.append("<NONE>");
            }
            else
            {
                final Iterator it = getJmsContentHeaderProperties().headers.entrySet().iterator();
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
                            case org.openamq.client.message.AbstractJMSMessage.BOOLEAN_PROPERTY_PREFIX:
                                buf.append("<boolean> ");
                                break;
                            case org.openamq.client.message.AbstractJMSMessage.BYTE_PROPERTY_PREFIX:
                                buf.append("<byte> ");
                                break;
                            case org.openamq.client.message.AbstractJMSMessage.SHORT_PROPERTY_PREFIX:
                                buf.append("<short> ");
                                break;
                            case org.openamq.client.message.AbstractJMSMessage.INT_PROPERTY_PREFIX:
                                buf.append("<int> ");
                                break;
                            case org.openamq.client.message.AbstractJMSMessage.LONG_PROPERTY_PREFIX:
                                buf.append("<long> ");
                                break;
                            case org.openamq.client.message.AbstractJMSMessage.FLOAT_PROPERTY_PREFIX:
                                buf.append("<float> ");
                                break;
                            case org.openamq.client.message.AbstractJMSMessage.DOUBLE_PROPERTY_PREFIX:
                                buf.append("<double> ");
                                break;
                            case org.openamq.client.message.AbstractJMSMessage.STRING_PROPERTY_PREFIX:
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

    public Map getUnderlyingMessagePropertiesMap()
    {
        return getJmsContentHeaderProperties().headers;
    }

    public void setUnderlyingMessagePropertiesMap(FieldTable messageProperties)
    {
        getJmsContentHeaderProperties().headers = messageProperties;
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

        if (getJmsContentHeaderProperties().headers == null)
        {
            getJmsContentHeaderProperties().headers = new FieldTable();
        }
    }

    public FieldTable populateHeadersFromMessageProperties()
    {
        if (getJmsContentHeaderProperties().headers == null)
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
            final Iterator entries = getJmsContentHeaderProperties().headers.entrySet().iterator();
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

    public BasicContentHeaderProperties getJmsContentHeaderProperties()
    {
        return (BasicContentHeaderProperties) _contentHeaderProperties;
    }
}
