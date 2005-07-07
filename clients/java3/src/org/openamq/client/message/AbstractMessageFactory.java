package org.openamq.client.message;

import org.openamq.client.framing.FieldTable;
import org.openamq.client.framing.AMQMessage;
import org.openamq.client.AMQException;

import javax.jms.JMSException;
import javax.jms.DeliveryMode;
import java.util.Map;
import java.util.Iterator;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AbstractMessageFactory implements MessageFactory
{

    protected abstract AbstractMessage createMessageWithBody(long messageNbr, AMQMessage frame) throws JMSException;

    public AbstractMessage createMessage(long messageNbr, boolean redelivered, AMQMessage frame) throws JMSException, AMQException
    {
        final AbstractMessage msg = createMessageWithBody(messageNbr, frame);
        msg.setJMSRedelivered(redelivered);
        msg.setJMSExpiration(frame.expiration);
        msg.setJMSPriority(frame.priority);
        msg.setJMSDeliveryMode((frame.persistent? DeliveryMode.PERSISTENT:DeliveryMode.NON_PERSISTENT));
        msg.setUnderlyingMessagePropertiesMap(createPropertiesMap(frame.headers));
        return msg;
    }

    private static Map createPropertiesMap(FieldTable headers) throws AMQException
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
                if (propertyName != null)                
                {
                    final char typeIdentifier = propertyName.charAt(0);
                    final String value = (String) entry.getValue();
                    switch (typeIdentifier)
                    {
                        case org.openamq.client.message.AbstractMessage.BOOLEAN_PROPERTY_PREFIX:
                            properties.put(propertyName, Boolean.valueOf(value));
                            break;
                        case org.openamq.client.message.AbstractMessage.BYTE_PROPERTY_PREFIX:
                            properties.put(propertyName, Byte.valueOf(value));
                            break;
                        case org.openamq.client.message.AbstractMessage.SHORT_PROPERTY_PREFIX:
                            properties.put(propertyName, Short.valueOf(value));
                            break;
                        case org.openamq.client.message.AbstractMessage.INT_PROPERTY_PREFIX:
                            properties.put(propertyName, Integer.valueOf(value));
                            break;
                        case org.openamq.client.message.AbstractMessage.LONG_PROPERTY_PREFIX:
                            properties.put(propertyName, Long.valueOf(value));
                            break;
                        case org.openamq.client.message.AbstractMessage.FLOAT_PROPERTY_PREFIX:
                            properties.put(propertyName, Float.valueOf(value));
                            break;
                        case org.openamq.client.message.AbstractMessage.DOUBLE_PROPERTY_PREFIX:
                            properties.put(propertyName, Double.valueOf(value));
                            break;
                        case org.openamq.client.message.AbstractMessage.STRING_PROPERTY_PREFIX:
                            properties.put(propertyName, value);
                            break;
                         default:
                            throw new AMQException("Internal error demarshalling property: unknown type prefix " +
                                                   typeIdentifier);
                    }

                }
            }
            return properties;
        }
    }
}
