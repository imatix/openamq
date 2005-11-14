/**
 * Date Created: 20-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.client;

import javax.jms.Queue;

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
     * Create a queue with a specified name.
     *
     * @param name the destination name (used in the routing key)
     * @param temporary if true the broker will generate a queue name, also if true then the queue is autodeleted
     * and exclusive
     */
    public AMQQueue(String name, boolean temporary)
    {
        // queue name is set to null indicating that the broker assigns a name in the case of temporary queues
        // temporary queues are typically used as response queues
        this(name, temporary?null:name, temporary, temporary);
    }

    /**
     * Create a reference to a queue. Note this does not actually imply the queue exists.
     * @param destinationName the queue name
     * @param queueName the queue name
     * @param exclusive true if the queue should only permit a single consumer
     * @param autoDelete true if the queue should be deleted automatically when the last consumers detaches
     */
    public AMQQueue(String destinationName, String queueName, boolean exclusive, boolean autoDelete)
    {
        super(AMQDestination.QUEUE_EXCHANGE_NAME, AMQDestination.QUEUE_EXCHANGE_CLASS, destinationName, exclusive,
              autoDelete, queueName);
    }

    public String getEncodedName()
    {
        return 'Q' + getQueueName();
    }

    public String getRoutingKey()
    {
        return getQueueName();
    }    
}
