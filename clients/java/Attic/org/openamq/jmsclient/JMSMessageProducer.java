/**
 *  JMSMessageProducer.java - JMS MessageProducer implementation using AMQP/Fast.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2005 JPMorgan
 *  Copyright (c) 2005 iMatix Corporation
 *
 **/

package org.openamq.jmsclient;

import javax.jms.MessageProducer;
import javax.jms.Session;
import javax.jms.Destination;
import javax.jms.Message;
import javax.jms.JMSException;

import java.io.*;
import org.openamq.*;
import org.openamq.frames.*;

public class JMSMessageProducer implements MessageProducer {
    JMSSession
        session;                            /* The creator                       */
    AMQHandle.Open 
        handleOpen;                         /* Handle open command               */
    AMQHandle.Send                          
        handle;                             /* Handle send command               */
    AMQFramingFactory
        amqFraming;                         /* Framing utility                   */

    public JMSMessageProducer(JMSSession session, Destination destination, int hID, boolean temporary) throws JMSException {
        AMQHandle.Created
            handleCreated;                  /* Handle created reply              */

        try {
            this.session = session;
            amqFraming = session.amqFraming;
            handleOpen = (AMQHandle.Open)amqFraming.constructFrame(AMQHandle.OPEN);
            handle = (AMQHandle.Send)amqFraming.constructFrame(AMQHandle.SEND);

            if (destination instanceof JMSQueue) {
                handleOpen.destName = ((JMSQueue)destination).getQueueName();
                handleOpen.serviceType = 1;
            } else {
                throwNotImplementedException();
            }
    
            // Open hadle
            handleOpen.channelId = session.channelOpen.channelId;
            handleOpen.handleId = hID;
            handleOpen.confirmTag = 0;
            handleOpen.producer = true;
            handleOpen.consumer = false;
            handleOpen.browser = false;
            handleOpen.temporary = temporary;
            handleOpen.mimeType = "";
            handleOpen.encoding = "";
            handleOpen.options = null;
            amqFraming.sendFrame(handleOpen);
            // Get handle created
            if (handleOpen.temporary)
                handleCreated = (AMQHandle.Created)amqFraming.receiveFrame();

            // Setup the handle
            handle.handleId = handleOpen.handleId;
            handle.confirmTag = 0;
            handle.fragmentSize = 0;
            handle.partial = false;
            handle.outOfBand = false;
            handle.recovery = false;
            handle.streaming = false;
            handle.destName = "";
        } catch (ClassCastException e) {
            System.err.println("JMSMessageProducer: " + "JMSMessageProducer: " + "unexpected frame from server");
        } catch (IOException e) {
            System.err.println("JMSMessageProducer: " + "JMSMessageProducer: " + e.getMessage());
        } catch (AMQException e) {
            System.err.println("JMSMessageProducer: " + "JMSMessageProducer: " + e.getMessage());
        }
    }

    public void setDisableMessageID(boolean value) throws JMSException {
        throwNotImplementedException();
    }

    public boolean getDisableMessageID() throws JMSException {
        throwNotImplementedException();
        return false;
    }

    public void setDisableMessageTimestamp(boolean value) throws JMSException {
        throwNotImplementedException();
    }

    public boolean getDisableMessageTimestamp() throws JMSException {
        throwNotImplementedException();
        return false;
    }

    public void setDeliveryMode(int deliveryMode) throws JMSException {
    }

    public int getDeliveryMode() throws JMSException {
        return -1;
    }

    public void setPriority(int defaultPriority) throws JMSException {
    }

    public int getPriority() throws JMSException {
        return -1;
    }

    public void setTimeToLive(long timeToLive) throws JMSException {
    }
   
    public long getTimeToLive() throws JMSException {
        return -1;
    }

    public Destination getDestination() throws JMSException {
        return null;
    }

    public void close() throws JMSException {
    }
    
    public void send(Message message) throws JMSException {
        JMSMessage
            jmsMessage = (JMSMessage)message;

        synchronized (session) {
            long
                messageSize = jmsMessage.head.size() + jmsMessage.head.bodySize;

            handle.partial = messageSize > amqFraming.getFrameMax();
            handle.fragmentSize = Math.min(messageSize, session.connection.frameMax);
            session.handles.add(handle);
            session.heads.add(jmsMessage.head);
            session.streams.add(jmsMessage.body);
            session.notify();
            try {
                session.wait();
            } catch (InterruptedException e) {}
        }
    }
    
    public void send(Message message, int deliveryMode, int priority, long timeToLive) 
        throws JMSException {
    }    
    
    public void send(Destination destination, Message message) throws JMSException {
    }
 
    public void send(Destination destination, Message message, int deliveryMode, 
        int priority,long timeToLive) throws JMSException {
    }    
        
    private void throwNotImplementedException() throws JMSException {
        throw new JMSException("OpenAMQ/JMS: method not yet implemented.");
    }
}
