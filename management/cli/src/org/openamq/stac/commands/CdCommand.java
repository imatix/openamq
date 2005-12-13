package org.openamq.stac.commands;

import org.openamq.stac.jmx.MBeanServerConnectionContext;
import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CdCommand
{
    public static void execute(MBeanServerConnectionContext context, String destination)
            throws AMQException
    {
        context.changeBean(destination);
    }

}
