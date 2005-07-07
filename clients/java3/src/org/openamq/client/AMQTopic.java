/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client;

import org.openamq.client.framing.Handle;

import javax.jms.JMSException;
import javax.jms.Topic;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQTopic extends AMQDestination implements Topic
{
     public AMQTopic(String name)
    {
        super(name, Handle.Open.SERVICE_TYPE_TOPIC);
    }

    public String getTopicName() throws JMSException
    {
        return super.getName();
    }
}
