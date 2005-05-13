/**
 *  JMSTextMessage.java - JMS TextMessage implementation using AMQP/Fast.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2005 JPMorgan
 *  Copyright (c) 2005 iMatix Corporation
 *
 **/

package org.openamq.jmsclient;

import java.io.*;
import javax.jms.TextMessage;
import javax.jms.JMSException;

public class JMSTextMessage extends JMSMessage implements TextMessage {
    String
        message;

    public JMSTextMessage(JMSSession session) throws JMSException {
        super(session);
    }

    public JMSTextMessage(JMSSession session, String string) throws JMSException {
        this(session);
        setText(string);
    }

    public void setText(String string) throws JMSException {
        message = string;
        body = new ByteArrayInputStream(session.amqFraming.string2Bytes(message));
    }

    public String getText() throws JMSException {
        return message;
    }
}
