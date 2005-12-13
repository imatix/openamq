package org.openamq.stac.jmx;

import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class NotConnectedException extends AMQException
{
    public NotConnectedException()
    {
        super("Not connected to JMX server");
    }
}
