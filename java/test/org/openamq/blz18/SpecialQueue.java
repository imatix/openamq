package org.openamq.blz18;

import org.openamq.client.AMQQueue;

/**
 * Queue that allows several private queues to be registered and bound
 * to an exchange with the same routing key.
 *
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
class SpecialQueue extends AMQQueue
{
    private final String name;

    SpecialQueue(String name)
    {
        super(name, true);
        this.name = name;
    }

    public String getRoutingKey()
    {
        return name;
    }
}
