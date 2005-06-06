package org.openamq.client;

import javax.jms.TextMessage;
import javax.jms.JMSException;
import javax.jms.Destination;
import java.util.Enumeration;
import java.io.UnsupportedEncodingException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Message implements TextMessage
{
    private byte[] _data;

    public byte[] getData()
    {
        return _data;
    }

    public void setText(String string) throws JMSException
    {
        try
        {
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
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean propertyExists(String string) throws JMSException
    {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean getBooleanProperty(String string) throws JMSException
    {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public byte getByteProperty(String string) throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public short getShortProperty(String string) throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public int getIntProperty(String string) throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public long getLongProperty(String string) throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public float getFloatProperty(String string) throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public double getDoubleProperty(String string) throws JMSException
    {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getStringProperty(String string) throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Object getObjectProperty(String string) throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Enumeration getPropertyNames() throws JMSException
    {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setBooleanProperty(String string, boolean b) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setByteProperty(String string, byte b) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setShortProperty(String string, short i) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setIntProperty(String string, int i) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setLongProperty(String string, long l) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setFloatProperty(String string, float v) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setDoubleProperty(String string, double v) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setStringProperty(String string, String string1) throws JMSException
    {
        //To change body of implemented methods use File | Settings | File Templates.
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
        //To change body of implemented methods use File | Settings | File Templates.
    }
}
