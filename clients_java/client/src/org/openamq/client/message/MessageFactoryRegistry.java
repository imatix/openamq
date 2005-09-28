package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.AMQMessage;
import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.framing.ContentBody;
import org.openamq.framing.FieldTable;

import javax.jms.JMSException;
import java.util.HashMap;
import java.util.Map;
import java.util.List;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class MessageFactoryRegistry
{
    private final Map _mimeToFactoryMap = new HashMap();

    public void registerFactory(String mimeType, MessageFactory mf)
    {
//        if (!FieldTable.grm && mimeType == null)
//        {
//            throw new IllegalArgumentException("Mime time must not be null");
//        }
        if (mf == null)
        {
            throw new IllegalArgumentException("Message factory must not be null");
        }
        _mimeToFactoryMap.put(mimeType, mf);
    }

    public MessageFactory deregisterFactory(String mimeType)
    {
        return (MessageFactory) _mimeToFactoryMap.remove(mimeType);
    }

    /**
     * Create a message. This looks up the MIME type from the content header and instantiates the appropriate
     * concrete message type.
     * @param messageNbr the AMQ message id
     * @param redelivered true if redelivered
     * @param contentHeader the content header that was received
     * @param bodies a list of ContentBody instances
     * @return the message.
     * @throws AMQException
     * @throws JMSException
     */
    public AbstractMessage createMessage(long messageNbr, boolean redelivered,
                                         JmsContentHeaderBody contentHeader,
                                         List bodies) throws AMQException, JMSException
    {
        MessageFactory mf = (MessageFactory) _mimeToFactoryMap.get(contentHeader.contentType);
        if (mf == null)
        {
            throw new AMQException("Unsupport MIME type of " + contentHeader.contentType);
        }
        else
        {
            return mf.createMessage(messageNbr, redelivered, contentHeader, bodies);
        }
    }

    public AbstractMessage createMessage(String mimeType) throws AMQException, JMSException
    {
        if (mimeType == null)
        {
            throw new IllegalArgumentException("Mime type must not be null");
        }
        MessageFactory mf = (MessageFactory) _mimeToFactoryMap.get(mimeType);
        if (mf == null)
        {
            throw new AMQException("Unsupport MIME type of " + mimeType);
        }
        else
        {
            return mf.createMessage();
        }
    }

    /**
     * Construct a new registry with the default message factories registered
     * @return a message factory registry
     */
    public static MessageFactoryRegistry newDefaultRegistry()
    {
        MessageFactoryRegistry mf = new MessageFactoryRegistry();
        mf.registerFactory("text/plain", new AMQTextMessageFactory());
        mf.registerFactory("application/octet-stream", new AMQBytesMessageFactory());
        // TODO: use bytes message for default message factory
        //if (FieldTable.grm)
            mf.registerFactory(null, new AMQTextMessageFactory());
        return mf;
    }
}
