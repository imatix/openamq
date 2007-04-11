/*****************************************************************************
 * Filename    : AMQMessage.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.client.message;

import org.openamq.framing.ContentHeaderProperties;
import org.openamq.client.AMQSession;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQMessage
{
    protected ContentHeaderProperties _contentHeaderProperties;

    /**
     * If the acknowledge mode is CLIENT_ACKNOWLEDGE the session is required
     */
    protected AMQSession _session;

    protected final long _deliveryTag;

    public AMQMessage(ContentHeaderProperties properties, long deliveryTag)
    {
        _contentHeaderProperties = properties;
        _deliveryTag = deliveryTag;
    }

    public AMQMessage(ContentHeaderProperties properties)
    {
        this(properties, -1);
    }

    /**
     * The session is set when CLIENT_ACKNOWLEDGE mode is used so that the CHANNEL ACK can be sent when the user
     * calls acknowledge()
     * @param s the AMQ session that delivered this message
     */
    public void setAMQSession(AMQSession s)
    {
        _session = s;
    }

    public AMQSession getAMQSession()
    {
        return _session;
    }

    /**
     * Get the AMQ message number assigned to this message
     * @return the message number
     */
    public long getDeliveryTag()
    {
        return _deliveryTag;
    }       
}
