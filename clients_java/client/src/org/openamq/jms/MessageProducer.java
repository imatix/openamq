package org.openamq.jms;

import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import java.io.UnsupportedEncodingException;

/**
 */
public interface MessageProducer extends javax.jms.MessageProducer
{
    /**
     * Set the default MIME type for messages produced by this producer. This reduces the overhead of each message.
     * @param mimeType
     */
    void setMimeType(String mimeType);

    /**
     * Set the default encoding for messages produced by this producer. This reduces the overhead of each message.
     * @param encoding the encoding as understood by XXXX how do I specify this?? RG
     * @throws UnsupportedEncodingException if the encoding is not understood
     */
    void setEncoding(String encoding) throws UnsupportedEncodingException;
    
    void send(Destination destination, Message message, int deliveryMode,
                     int priority, long timeToLive, boolean immediate)
            throws JMSException;

    void send(Destination destination, Message message, int deliveryMode,
                     int priority, long timeToLive, boolean mandatory, boolean immediate)
            throws JMSException;
}
