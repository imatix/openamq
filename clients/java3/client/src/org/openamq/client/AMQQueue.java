/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client;

import javax.jms.Queue;
import javax.jms.JMSException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQQueue extends AMQDestination implements Queue
{
    /**
     * Create a reference to a non temporary queue. Note this does not actually imply the queue exists.
     * @param name the name of the queue
     */
    public AMQQueue(String name)
    {
        this(name, false);
    }

    /**
     * Create a reference to a queue. Note this does not actually imply the queue exists.
     * @param name the queue name
     * @param temporary true if the queue is temporary, false otherwise
     */
    public AMQQueue(String name, boolean temporary)
    {
        super(AMQDestination.QUEUE_EXCHANGE_NAME, AMQDestination.QUEUE_EXCHANGE_CLASS, name, temporary);
    }

    public String getQueueName() throws JMSException
    {
        return super.getDestinationName();
    }

    public String getEncodedName()
    {
        return 'Q' + getDestinationName();
    }
}
