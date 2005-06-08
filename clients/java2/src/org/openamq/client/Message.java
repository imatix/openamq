package org.openamq.client;

import javax.jms.TextMessage;
import javax.jms.JMSException;
import javax.jms.Destination;
import java.util.Enumeration;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.io.UnsupportedEncodingException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Message implements TextMessage
{
    private byte[] _data;

    private Map _messageProperties;

    public static final char BOOLEAN_PROPERTY_PREFIX = 'B';
    public static final char BYTE_PROPERTY_PREFIX = 'b';
    public static final char SHORT_PROPERTY_PREFIX = 's';
    public static final char INT_PROPERTY_PREFIX = 'i';
    public static final char LONG_PROPERTY_PREFIX = 'l';
    public static final char FLOAT_PROPERTY_PREFIX = 'f';
    public static final char DOUBLE_PROPERTY_PREFIX = 'd';
    public static final char STRING_PROPERTY_PREFIX = 'S';

    public byte[] getData()
    {
        return _data;
    }

    public Message(String text) throws JMSException
    {
        setText(text);
    }

    public Message() throws JMSException
    {
        setText(null);
    }

    public void setText(String string) throws JMSException
    {
        try
        {
            // TODO : remove this
            if (string == null) string = "String is null";
            _data = string.getBytes("utf8");
        }
        catch (UnsupportedEncodingException e)
        {
            // should never occur
            throw new JMSException("Unable to decode string data");
        }
    }

    public String getText() throws JMSException
    {
        if (_data == null)
        {
            return null;
        }
        else
        {
            return new String(_data);
        }
    }

    public String getJMSMessageID() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSMessageID(String string) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public long getJMSTimestamp() throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSTimestamp(long l) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
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
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSReplyTo(Destination destination) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public Destination getJMSDestination() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSDestination(Destination destination) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public int getJMSDeliveryMode() throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSDeliveryMode(int i) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean getJMSRedelivered() throws JMSException
    {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSRedelivered(boolean b) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getJMSType() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSType(String string) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public long getJMSExpiration() throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSExpiration(long l) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public int getJMSPriority() throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setJMSPriority(int i) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
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
            return _messageProperties.containsKey(propertyName);
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
                return Long.valueOf(null).longValue();
            }
            else
            {
                return l.shortValue();
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
        _messageProperties.put(BYTE_PROPERTY_PREFIX + propertyName, Byte.valueOf(b));
    }

    public void setShortProperty(String propertyName, short i) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(SHORT_PROPERTY_PREFIX + propertyName, Short.valueOf(i));
    }

    public void setIntProperty(String propertyName, int i) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(INT_PROPERTY_PREFIX + propertyName, Integer.valueOf(i));
    }

    public void setLongProperty(String propertyName, long l) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(LONG_PROPERTY_PREFIX + propertyName, Long.valueOf(l));
    }

    public void setFloatProperty(String propertyName, float f) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(FLOAT_PROPERTY_PREFIX + propertyName,  Float.valueOf(f));
    }

    public void setDoubleProperty(String propertyName, double v) throws JMSException
    {
        checkPropertyName(propertyName);
        _messageProperties.put(DOUBLE_PROPERTY_PREFIX + propertyName, Double.valueOf(v));
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
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void clearBody() throws JMSException
    {
        _data = null;
    }

    public String toString()
    {
        try
        {
            StringBuffer buf = new StringBuffer("Body:\n");
            buf.append(getText());
            buf.append("Properties:\n");
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
                        buf.append("Internal error: Property with NULL key defined");
                    }
                    else
                    {
                        char typeIdentifier = propertyName.charAt(0);
                        switch (typeIdentifier)
                        {
                            case org.openamq.client.Message.BOOLEAN_PROPERTY_PREFIX:
                                buf.append("<boolean> ");
                                break;
                            case org.openamq.client.Message.BYTE_PROPERTY_PREFIX:
                                buf.append("<byte> ");
                                break;
                            case org.openamq.client.Message.SHORT_PROPERTY_PREFIX:
                                buf.append("<short> ");
                                break;
                            case org.openamq.client.Message.INT_PROPERTY_PREFIX:
                                buf.append("<int> ");
                                break;
                            case org.openamq.client.Message.LONG_PROPERTY_PREFIX:
                                buf.append("<long> ");
                                break;
                            case org.openamq.client.Message.FLOAT_PROPERTY_PREFIX:
                                buf.append("<float> ");
                                break;
                            case org.openamq.client.Message.DOUBLE_PROPERTY_PREFIX:
                                buf.append("<double> ");
                                break;
                            case org.openamq.client.Message.STRING_PROPERTY_PREFIX:
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
    }

}
