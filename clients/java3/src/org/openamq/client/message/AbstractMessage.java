package org.openamq.client.message;

import org.openamq.client.framing.FieldTable;
import org.openamq.client.framing.Channel;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQQueue;
import org.openamq.client.AMQTopic;
import org.apache.commons.collections.map.ReferenceMap;

import javax.jms.Destination;
import javax.jms.JMSException;
import java.util.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractMessage implements javax.jms.Message
{
    private static final Map _destinationCache = Collections.synchronizedMap(new ReferenceMap());

    private Map _messageProperties;

    private static final String TIMESTAMP_PROPERTY_NAME = "JMS_timestamp";

    private static final String REPLYTO_PROPERTY_NAME = "JMS_replyto";

    public static final char BOOLEAN_PROPERTY_PREFIX = 'B';
    public static final char BYTE_PROPERTY_PREFIX = 'b';
    public static final char SHORT_PROPERTY_PREFIX = 's';
    public static final char INT_PROPERTY_PREFIX = 'i';
    public static final char LONG_PROPERTY_PREFIX = 'l';
    public static final char FLOAT_PROPERTY_PREFIX = 'f';
    public static final char DOUBLE_PROPERTY_PREFIX = 'd';
    public static final char STRING_PROPERTY_PREFIX = 'S';

    private long _messageNbr;

    /**
     * The unique message id, lazily initialised and can be disabled
     */
    private String _jmsMessageId;

    private long _expiration;

    private boolean _redelivered;

    private int _priority;

    private int _deliveryMode;

    /**
     * If the acknowledge mode is CLIENT_ACKNOWLEDGE the session is required
     */
    private AMQSession _session;

    protected AbstractMessage()
    {
    }

    protected AbstractMessage(long messageNbr)
    {
        _messageNbr = messageNbr;
    }

    public String getJMSMessageID() throws JMSException
    {
        if (_jmsMessageId == null)
        {
            _jmsMessageId = "ID:" + _messageNbr;
        }
        return _jmsMessageId;
    }

    public void setJMSMessageID(String messageId) throws JMSException
    {
        _jmsMessageId = messageId;
    }

    public long getJMSTimestamp() throws JMSException
    {
        return getLongProperty(TIMESTAMP_PROPERTY_NAME);
    }

    public void setJMSTimestamp(long timestamp) throws JMSException
    {
        setLongProperty(TIMESTAMP_PROPERTY_NAME, timestamp);
    }

    public byte[] getJMSCorrelationIDAsBytes() throws JMSException
    {
        return new byte[0];  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSCorrelationIDAsBytes(byte[] bytes) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSCorrelationID(String string) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getJMSCorrelationID() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Destination getJMSReplyTo() throws JMSException
    {
        String replyToEncoding = getStringProperty(REPLYTO_PROPERTY_NAME);
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
        if (destination instanceof AMQQueue)
        {
            final String encodedDestination = 'Q' + ((AMQQueue)destination).getName();
            _destinationCache.put(encodedDestination, destination);
            setStringProperty(REPLYTO_PROPERTY_NAME, encodedDestination);
        }
        else if (destination instanceof AMQTopic)
        {
            final String encodedDestination = 'T' + ((AMQTopic)destination).getName();
            _destinationCache.put(encodedDestination, destination);
            setStringProperty(REPLYTO_PROPERTY_NAME, encodedDestination);
        }
        else
        {
            throw new JMSException("Unknown destination type: " + destination.getClass());
        }
    }

    public Destination getJMSDestination() throws JMSException
    {
        return null;
    }

    public void setJMSDestination(Destination destination) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public int getJMSDeliveryMode() throws JMSException
    {
        return _deliveryMode;
    }

    public void setJMSDeliveryMode(int i) throws JMSException
    {
        _deliveryMode = i;
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
        return _expiration;
    }

    public void setJMSExpiration(long l) throws JMSException
    {
        _expiration = l;
    }

    public int getJMSPriority() throws JMSException
    {
        return _priority;
    }

    public void setJMSPriority(int i) throws JMSException
    {
        _priority = i;
    }

    public void clearProperties() throws JMSException
    {
        if (_messageProperties != null)
        {
            _messageProperties.clear();
        }
    }

    public boolean propertyExists(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_messageProperties == null)
        {
            return false;
        }
        else
        {
            // TODO: fix this
            return _messageProperties.containsKey(LONG_PROPERTY_PREFIX  + propertyName);
        }
    }

    public boolean getBooleanProperty(String propertyName) throws JMSException
    {
        checkPropertyName(propertyName);
        if (_messageProperties == null)
        {
            return Boolean.valueOf(null).booleanValue();
        }
        else
        {
            Boolean b = (Boolean) _messageProperties.get(BOOLEAN_PROPERTY_PREFIX + propertyName);

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
        if (_messageProperties == null)
        {
            return Byte.valueOf(null).byteValue();
        }
        else
        {
            Byte b = (Byte) _messageProperties.get(BYTE_PROPERTY_PREFIX + propertyName);
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
        if (_messageProperties == null)
        {
            return Short.valueOf(null).shortValue();
        }
        else
        {
            Short s = (Short) _messageProperties.get(SHORT_PROPERTY_PREFIX + propertyName);
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
        if (_messageProperties == null)
        {
            return Integer.valueOf(null).intValue();
        }
        else
        {
            Integer i = (Integer) _messageProperties.get(INT_PROPERTY_PREFIX + propertyName);
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
        if (_messageProperties == null)
        {
            return Long.valueOf(null).longValue();
        }
        else
        {
            Long l = (Long) _messageProperties.get(LONG_PROPERTY_PREFIX + propertyName);
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
        if (_messageProperties == null)
        {
            return Float.valueOf(null).floatValue();
        }
        else
        {
            final Float f = (Float) _messageProperties.get(FLOAT_PROPERTY_PREFIX + propertyName);
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
        if (_messageProperties == null)
        {
            return Double.valueOf(null).doubleValue();
        }
        else
        {
            final Double d = (Double) _messageProperties.get(DOUBLE_PROPERTY_PREFIX + propertyName);
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
        if (_messageProperties == null)
        {
            return null;
        }
        else
        {
            return (String) _messageProperties.get(STRING_PROPERTY_PREFIX + propertyName);
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
        _messageProperties.put(BOOLEAN_PROPERTY_PREFIX + propertyName, Boolean.valueOf(b));
    }

    public void setByteProperty(String propertyName, byte b) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(BYTE_PROPERTY_PREFIX + propertyName, new Byte(b));
    }

    public void setShortProperty(String propertyName, short i) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(SHORT_PROPERTY_PREFIX + propertyName, new Short(i));
    }

    public void setIntProperty(String propertyName, int i) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(INT_PROPERTY_PREFIX + propertyName, new Integer(i));
    }

    public void setLongProperty(String propertyName, long l) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(LONG_PROPERTY_PREFIX + propertyName, new Long(l));
    }

    public void setFloatProperty(String propertyName, float f) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(FLOAT_PROPERTY_PREFIX + propertyName,  new Float(f));
    }

    public void setDoubleProperty(String propertyName, double v) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(DOUBLE_PROPERTY_PREFIX + propertyName, new Double(v));
    }

    public void setStringProperty(String propertyName, String value) throws JMSException
    {
        checkPropertyName(propertyName);
        createPropertyMapIfRequired();
        propertyName = STRING_PROPERTY_PREFIX + propertyName;
        _messageProperties.put(propertyName, value);
    }

    private void createPropertyMapIfRequired()
    {
        if (_messageProperties == null)
        {
            _messageProperties = new HashMap();
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
            if (_messageProperties == null)
            {
                buf.append("<NONE>");
            }
            else
            {
                final Iterator it = _messageProperties.entrySet().iterator();
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
        return _messageProperties;
    }

    void setUnderlyingMessagePropertiesMap(Map messageProperties)
    {
        _messageProperties = messageProperties;
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

        if (_messageProperties == null)
        {
            _messageProperties = new HashMap();
        }
    }

    public FieldTable populateHeadersFromMessageProperties()
    {
        if (_messageProperties == null)
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
            final Iterator entries = _messageProperties.entrySet().iterator();
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
}
