package org.openamq.stac.commands;

import org.openamq.stac.jmx.MBeanServerConnectionContext;
import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class InvokeCommand
{
    public static void execute(MBeanServerConnectionContext context, String methodName, Object... args)
            throws AMQException
    {
        // Not used currently

    }
}
