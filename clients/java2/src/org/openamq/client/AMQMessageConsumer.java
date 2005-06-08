package org.openamq.client;

import org.openamq.jms.MessageConsumer;

import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;

import org.openamq.client.framing.*;

import org.apache.log4j.*;

import edu.emory.mathcs.backport.java.util.concurrent.*;

import java.util.Map;
import java.util.Iterator;


/**
 */
public class AMQMessageConsumer extends Closeable implements MessageConsumer
{
    private static final Logger _logger = Logger.getLogger(AMQMessageConsumer.class);

    private String _messageSelector;

    private boolean _noLocal;

    private Destination _destination;

    private MessageListener _messageListener;

    private BlockingQueue _queue = new LinkedBlockingQueue();
    private Despatcher _despatcher;

    /**
     * Used for concurrency control
     */
    private final Object _lock = new Object();

    private final Object _syncLock = new Object();

    private boolean _receiving;

    AMQMessageConsumer(int handleId,Destination destination, String messageSelector, boolean noLocal)
    {
        _messageSelector = messageSelector;

        _despatcher = new Despatcher();

        _despatcher.start();
    }

    public String getMessageSelector() throws JMSException
    {
        return _messageSelector;
    }

    public MessageListener getMessageListener() throws JMSException
    {
        return _messageListener;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setMessageListener(MessageListener messageListener) throws JMSException
    {
        // This check may not be necessary. It may be harmless to change the
        // state of this - irrespective of whether it is closed. I have therefore
        // left this just in case and as an application clean-up sanity check,
        // but it may change...
        checkNotClosed();

        synchronized(_syncLock)
        {
            // If someone is already receiving
            if (_messageListener != null && _receiving) throw new IllegalStateException("Another thread is already receiving...");

            _messageListener = messageListener;

            _receiving = _messageListener != null;

            if (_receiving)
            {

            }
        }
    }

    public Message receive() throws JMSException
    {
        return receive(0);
    }

    public Message receive(long l) throws JMSException
    {
        checkNotClosed();

        synchronized(_syncLock)
        {
            // If someone is already receiving
            if (_receiving) throw new IllegalStateException("Another thread is already receiving (possibly asynchronously)...");

            _receiving = true;
        }

        try
        {
            // do read here
        }
        finally
        {
            synchronized(_syncLock)
            {
                _receiving = false;
            }
        }

        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Message receiveNoWait() throws JMSException
    {
        checkNotClosed();

        if (1 == 1) throw new UnsupportedOperationException();

        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void close() throws JMSException
    {
        synchronized (_closingLock)
        {
            _closed.set(true);
        }
    }

    public void notifyMessage(AMQMessage messageFragment)
    {
        if (_messageListener == null)
        {
            _logger.warn("Received a message without a listener - ignoring...");
        }
        else
        {
            try
            {
                org.openamq.client.Message message = new org.openamq.client.Message();
                message.setText(new String(messageFragment.message));
                message.setUnderlyingMessagePropertiesMap(createPropertiesMap(messageFragment.headers));

                try
                {
                    _queue.put(message);
                }
                catch(InterruptedException e)
                {
                    ;
                }
            }
            catch(JMSException e)
            {
                _logger.warn("Caught exception (dump follows) - ignoring...",e);
            }
        }
    }

    private Map createPropertiesMap(FieldTable headers)
    {
        if (headers == null)
        {
            return null;
        }
        else
        {
            //
            // We need to convert every property into a String representation
            // Note that type information is preserved in the property name
            //
            final Map properties  = new FieldTable();
            final Iterator entries = headers.entrySet().iterator();
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
                    final char typeIdentifier = propertyName.charAt(0);
                    final String value = (String) entry.getValue();
                    switch (typeIdentifier)
                    {
                        case org.openamq.client.Message.BOOLEAN_PROPERTY_PREFIX:
                            properties.put(propertyName, Boolean.valueOf(value));
                            break;
                        case org.openamq.client.Message.BYTE_PROPERTY_PREFIX:
                            properties.put(propertyName, Byte.valueOf(value));
                            break;
                        case org.openamq.client.Message.SHORT_PROPERTY_PREFIX:
                            properties.put(propertyName, Short.valueOf(value));
                        case org.openamq.client.Message.INT_PROPERTY_PREFIX:
                        case org.openamq.client.Message.LONG_PROPERTY_PREFIX:
                        case org.openamq.client.Message.FLOAT_PROPERTY_PREFIX:
                        case org.openamq.client.Message.DOUBLE_PROPERTY_PREFIX:
                        case org.openamq.client.Message.STRING_PROPERTY_PREFIX:
                    }

                }
            }
            return table;
        }
    }

    private class Despatcher extends Thread
    {
        public void run()
        {
            org.openamq.client.Message message;

            try
            {
                while((message = (org.openamq.client.Message)_queue.take()) != null)
                {
                    _messageListener.onMessage(message);
                }
            }
            catch(InterruptedException e)
            {
                ;
            }

            System.err.println("Despatcher thread termoinating...");
        }
    }
}
