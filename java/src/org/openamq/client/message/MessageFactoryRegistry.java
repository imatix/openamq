package org.openamq.client.message;

import org.openamq.AMQException;
import org.openamq.framing.BasicContentHeaderProperties;
import org.openamq.framing.ContentHeaderBody;

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
     * @param deliveryTag the AMQ message id
     * @param redelivered true if redelivered
     * @param contentHeader the content header that was received
     * @param bodies a list of ContentBody instances
     * @return the message.
     * @throws AMQException
     * @throws JMSException
     */
    public AbstractJMSMessage createMessage(long deliveryTag, boolean redelivered,
                                            ContentHeaderBody contentHeader,
                                            List bodies) throws AMQException, JMSException
    {
        BasicContentHeaderProperties properties =  (BasicContentHeaderProperties) contentHeader.properties;
        MessageFactory mf = (MessageFactory) _mimeToFactoryMap.get(properties.getContentType());
        if (mf == null)
        {
            throw new AMQException("Unsupport MIME type of " + properties.getContentType());
        }
        else
        {
            return mf.createMessage(deliveryTag, redelivered, contentHeader, bodies);
        }
    }

    public AbstractJMSMessage createMessage(String mimeType) throws AMQException, JMSException
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
        mf.registerFactory("text/plain", new JMSTextMessageFactory());
        mf.registerFactory("text/xml", new JMSTextMessageFactory());
        mf.registerFactory("application/octet-stream", new JMSBytesMessageFactory());
        mf.registerFactory(JMSObjectMessage.MIME_TYPE, new JMSObjectMessageFactory());
        mf.registerFactory(null, new JMSBytesMessageFactory());
        return mf;
    }
}
