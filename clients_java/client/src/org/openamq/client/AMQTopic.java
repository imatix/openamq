/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client;

import javax.jms.JMSException;
import javax.jms.Topic;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQTopic extends AMQDestination implements Topic
{
    public AMQTopic(String name)
    {
        this(name, false);
    }

    public AMQTopic(String name, boolean temporary)
    {
        this(name, DEFAULT_SCOPE, temporary);
    }

    public AMQTopic(String name, String scope, boolean temporary)
    {
        super(AMQDestination.TOPIC_EXCHANGE_NAME, AMQDestination.TOPIC_EXCHANGE_CLASS, name, temporary, null, scope);
    }

    public String getTopicName() throws JMSException
    {
        return super.getDestinationName();
    }

    public String getEncodedName()
    {
        return 'T' + getDestinationName();
    }

     public String getRoutingKey()
    {
        return getDestinationName();
    }
}
