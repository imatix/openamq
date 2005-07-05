/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client;

import org.openamq.client.framing.Handle;

import javax.jms.Queue;
import javax.jms.JMSException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQQueue extends AMQDestination implements Queue
{
    public AMQQueue(String name)
    {
        super(name, Handle.Open.SERVICE_TYPE_QUEUE);
    }

    public String getQueueName() throws JMSException
    {
        return super.getName();
    }
}
