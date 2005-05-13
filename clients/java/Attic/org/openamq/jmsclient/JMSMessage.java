/**
 *  JMSMessage.java - JMS Message implementation using AMQP/Fast.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2005 JPMorgan
 *  Copyright (c) 2005 iMatix Corporation
 *
 **/

package org.openamq.jmsclient;

import java.util.Enumeration;
import java.util.Properties;
import javax.jms.Message;
import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.DeliveryMode;

import java.net.*;
import java.io.*;
import org.openamq.*;
import org.openamq.frames.*;

public class JMSMessage implements Message {

    static final int DEFAULT_DELIVERY_MODE = DeliveryMode.PERSISTENT;

    static final int DEFAULT_PRIORITY = 4;

    static final long DEFAULT_TIME_TO_LIVE = 0;

    int
        deliveryMode;
    JMSSession
        session;                            /* The creator                       */
    AMQMessage.Head
        head;                               /* Message head                      */
    InputStream
        body;                               /* Message body                      */

    public JMSMessage(JMSSession session) {
        this.session = session;
        body = null;

        try {
            head = (AMQMessage.Head)session.amqFraming.constructMessageHead();
            head.bodySize = 0;
            head.persistent = true;
            head.priority = DEFAULT_PRIORITY;
            head.expiration = 0;
            head.mimeType = "";
            head.encoding = "";
            head.identifier = "";
            head.headers = null;
        } catch (AMQException e) {
            System.err.println("JMSMessage: " + "JMSMessage: " + e.getMessage());
        }
    }

    public String getJMSMessageID() throws JMSException {
        return null;
    }

    public void setJMSMessageID(String id) throws JMSException {
    }

    public long getJMSTimestamp() throws JMSException {
        return -1;
    }

    public void setJMSTimestamp(long timestamp) throws JMSException {
    }

    public byte [] getJMSCorrelationIDAsBytes() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public void setJMSCorrelationIDAsBytes(byte[] correlationID) throws JMSException {
        throwNotImplementedException();
    }

    public void setJMSCorrelationID(String correlationID) throws JMSException {
        throwNotImplementedException();
    }

    public String getJMSCorrelationID() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public Destination getJMSReplyTo() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public void setJMSReplyTo(Destination replyTo) throws JMSException {
        throwNotImplementedException();
    }

    public Destination getJMSDestination() throws JMSException {
        return null;
    }

    public void setJMSDestination(Destination destination) throws JMSException {
    }

    public int getJMSDeliveryMode() throws JMSException {
        return deliveryMode;
    }
 
    public void setJMSDeliveryMode(int deliveryMode) throws JMSException {
        this.deliveryMode = deliveryMode;
    }

    public boolean getJMSRedelivered() throws JMSException {
        return false;
    }
 
    public void setJMSRedelivered(boolean redelivered) throws JMSException {
        throwNotImplementedException();
    }

    public String getJMSType() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public void setJMSType(String type) throws JMSException {
        throwNotImplementedException();
    }

    public long getJMSExpiration() throws JMSException {
        return -1;
    }
 
    public void setJMSExpiration(long expiration) throws JMSException {
    }

    public int getJMSPriority() throws JMSException {
        return head.priority;
    }

    public void  setJMSPriority(int priority) throws JMSException {
        head.priority = (short)priority;
    }

    public void clearProperties() throws JMSException {
        throwNotImplementedException();
    }

    public boolean propertyExists(String name) throws JMSException {
        throwNotImplementedException();
        return false;
    }

    public boolean getBooleanProperty(String name) throws JMSException {
        throwNotImplementedException();
        return false;
    }

    public byte getByteProperty(String name) throws JMSException {
        throwNotImplementedException();
        return -1;
    }

    public short getShortProperty(String name) throws JMSException {
        throwNotImplementedException();
        return -1;
    }
 
    public int getIntProperty(String name) throws JMSException {
        throwNotImplementedException();
        return -1;
    }

    public long getLongProperty(String name) throws JMSException {
        throwNotImplementedException();
        return -1;
    }

    public float getFloatProperty(String name) throws JMSException {
        throwNotImplementedException();
        return -1;
    }

    public double getDoubleProperty(String name) throws JMSException {
        throwNotImplementedException();
        return -1;
    }
    
    public String getStringProperty(String name) throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public Object getObjectProperty(String name) throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public Enumeration getPropertyNames() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public void setBooleanProperty(String name, boolean value) throws JMSException {
        throwNotImplementedException();
    }

    public void setByteProperty(String name, byte value) throws JMSException {
        throwNotImplementedException();
    }

    public void setShortProperty(String name, short value) throws JMSException {
        throwNotImplementedException();
    }
    
    public void setIntProperty(String name, int value) throws JMSException {
        throwNotImplementedException();
    }
    
    public void setLongProperty(String name, long value) throws JMSException {
        throwNotImplementedException();
    }
    
    public void setFloatProperty(String name, float value) throws JMSException {
        throwNotImplementedException();
    }
    
    public void setDoubleProperty(String name, double value) throws JMSException {
        throwNotImplementedException();
    }

    public void setStringProperty(String name, String value) throws JMSException {
        throwNotImplementedException();
    }
    
    public void setObjectProperty(String name, Object value) throws JMSException {
        throwNotImplementedException();
    }
    
    public void acknowledge() throws JMSException {
    }

    public void clearBody() throws JMSException {
    }
    
    private void throwNotImplementedException() throws JMSException {
        throw new JMSException("OpenAMQ/JMS: method not yet implemented.");
    }
}
