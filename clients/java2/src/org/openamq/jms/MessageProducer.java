package org.openamq.jms;

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

}
