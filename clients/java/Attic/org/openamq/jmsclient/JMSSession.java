/**
 *  JMSSession.java - JMS Session implementation using AMQP/Fast.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2005 JPMorgan
 *  Copyright (c) 2005 iMatix Corporation
 *
 **/

package org.openamq.jmsclient;

import java.io.Serializable;
import javax.jms.Session;
import javax.jms.BytesMessage;
import javax.jms.MapMessage;
import javax.jms.ObjectMessage;
import javax.jms.StreamMessage;
import javax.jms.TextMessage;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import javax.jms.MessageConsumer;
import javax.jms.Message;
import javax.jms.Queue;
import javax.jms.Topic;
import javax.jms.TopicSubscriber;
import javax.jms.QueueBrowser;
import javax.jms.TemporaryQueue;
import javax.jms.TemporaryTopic;
import javax.jms.Destination;
import javax.jms.JMSException;

import java.io.*;
import java.util.*;
import org.openamq.*;
import org.openamq.frames.*;

public class JMSSession implements Session, Runnable {

    static final int AUTO_ACKNOWLEDGE = 1;

    static final int CLIENT_ACKNOWLEDGE = 2;

    static final int DUPS_OK_ACKNOWLEDGE = 3;
    
    static final int SESSION_TRANSACTED = 0;

    static int
        chID = 1,                           /* Current channel ID                */
        proID = 1;                          /* Current producer ID               */

    int
        acknowledgeMode;                    /* JMS ack mode                      */
    byte[]
        buffer;                             /* Message chunks when reading       */
    JMSConnection
        connection;                         /* The creator                       */
    LinkedList
        handles,                            /* Message handles                   */
        heads,                              /* Message heads                     */
        streams,                            /* Message bodies                    */
        consumers;                          /* Consumers list                    */
    Thread
        session;                            /* The thread for this session       */
    AMQFramingFactory
        amqFraming;                         /* Framing utility                   */
    AMQChannel.Open
        channelOpen;                        /* Channel open command              */

    public JMSSession(JMSConnection connection, boolean transacted, int acknowledgeMode) throws JMSException {
        try {
            this.connection = connection;
            amqFraming = connection.amqFraming;
            this.acknowledgeMode = acknowledgeMode;
            handles = new LinkedList();
            heads = new LinkedList();
            streams = new LinkedList();
            buffer = new byte[connection.frameMax];
            channelOpen = (AMQChannel.Open)amqFraming.constructFrame(AMQChannel.OPEN);
    
            // Open channel
            channelOpen.channelId = chID++;
            channelOpen.confirmTag = 0;
            channelOpen.transacted = transacted;
            channelOpen.restartable = false;
            channelOpen.options = null;
            channelOpen.outOfBand = "";
            amqFraming.sendFrame(channelOpen);

            // Start a new thread
            session = new Thread(this);
            session.start();
        } catch (IOException e) {
            throw new JMSException("JMSSession: " + "JMSSession: " + e.getMessage());
        } catch (AMQException e) {
            throw new JMSException("JMSSession: " + "JMSSession: " + e.getMessage());
        }
    }
    
    public BytesMessage createBytesMessage() throws JMSException {
        return null;
    } 

    public MapMessage createMapMessage() throws JMSException {
        throwNotImplementedException();
        return null;
    } 

    public Message createMessage() throws JMSException {
        return new JMSMessage(this);
    }

    public ObjectMessage createObjectMessage() throws JMSException {
        throwNotImplementedException();
        return null;
    } 

    public ObjectMessage createObjectMessage(Serializable object) throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public StreamMessage createStreamMessage() throws JMSException {
        throwNotImplementedException();
        return null;
    }  

    public TextMessage createTextMessage() throws JMSException {
        return new JMSTextMessage(this);
    }

    public TextMessage createTextMessage(String text) throws JMSException {
        return new JMSTextMessage(this, text);
    }

    public boolean getTransacted() throws JMSException {
        throwNotImplementedException();
        return false;
    }

    public int getAcknowledgeMode() throws JMSException {
        return -1;
    }

    public void commit() throws JMSException {
    }

    public void rollback() throws JMSException {
    }

    public void close() throws JMSException {
    }

    public void recover() throws JMSException {
        throwNotImplementedException();
    }

    public MessageListener  getMessageListener() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public void  setMessageListener(MessageListener listener) throws JMSException {
        throwNotImplementedException();
    }

    public void run() {
        AMQHandle.Send
            handle;                         /* Handle send                       */
        AMQMessage.Head
            head;                           /* Message head                      */
        InputStream
            is;                             /* Message body                      */
        OutputStream
            os;                             /* Message output stream             */

        while(session != null) {
            synchronized (this) {
                handle = null;
                head = null;
                is = null;
                if (handles.size() > 0)
                    handle = (AMQHandle.Send)handles.removeFirst();
                if (heads.size() > 0)
                    head = (AMQMessage.Head)heads.removeFirst();
                if (streams.size() > 0)
                    is = (InputStream)streams.removeFirst();
            }
    
            if (handle != null) {
                try {
                    os = amqFraming.sendMessage(handle, head, null, false);
                    for (long len = head.bodySize, chunk; len > 0; len -= chunk) {
                        chunk = Math.min(len, buffer.length);
    
                        amqFraming.readBytes(is, buffer, 0, (int)chunk);
                        os.write(buffer, 0, (int)chunk);
                    }
                    os.close();
                } catch (IOException e) {
                    break;
                } finally {
                    synchronized (this) {
                        this.notify();
                    }
                }
            } else {
                synchronized (this) {
                    try {
                        if (session != null && handles.size() == 0)
                            this.wait();
                    } catch (InterruptedException e) {}
                }
            }
        }
    }
    
    public MessageProducer createProducer(Destination destination) throws JMSException {
        return new JMSMessageProducer(this, destination, proID++, false);
    }
    
    public MessageConsumer createConsumer(Destination destination) throws JMSException {
        return null;
    }

    public MessageConsumer createConsumer(Destination destination, java.lang.String messageSelector) throws JMSException {
        throwNotImplementedException();
        return null;
    }
    
    public MessageConsumer  createConsumer(Destination destination, java.lang.String messageSelector, boolean NoLocal)   throws JMSException {
        throwNotImplementedException();
        return null;
    }
    
    public Queue createQueue(String queueName) throws JMSException {
        return null;
    }
    
    public Topic createTopic(String topicName) throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public TopicSubscriber createDurableSubscriber(Topic topic, String name) throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public TopicSubscriber createDurableSubscriber(Topic topic, String name, String messageSelector, boolean noLocal) throws JMSException {
        throwNotImplementedException();
        return null;
    }
    
    public QueueBrowser createBrowser(Queue queue) throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public QueueBrowser  createBrowser(Queue queue, String messageSelector) throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public TemporaryQueue createTemporaryQueue() throws JMSException {
        return null;
    }
   
    public TemporaryTopic createTemporaryTopic() throws JMSException {
        return null;
    }

    public void unsubscribe(String name) throws JMSException {
        throwNotImplementedException();
    }
    
    private void throwNotImplementedException() throws JMSException {
        throw new JMSException("OpenAMQ/JMS: method not yet implemented.");
    }
}
