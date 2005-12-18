package org.openamq.stac.commands;

import org.openamq.stac.jmx.MBeanServerConnectionContext;
import org.openamq.stac.jmx.CurrentMBean;
import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CdCommand
{
    public static void execute(MBeanServerConnectionContext context, String destination)
            throws AMQException
    {
        // check if it is an absolute path and if so change to the root first
        if (destination.startsWith("/"))
        {
            context.changeBean("/");
            destination = destination.substring(1);
        }
        if (destination.length() == 0)
        {
            return;
        }
        String[] destinations = destination.split("/");
        for (String item : destinations)
        {
            if ("..".equals(item))
            {
                item = CurrentMBean.PARENT_ATTRIBUTE;
            }
            context.changeBean(item);
        }
    }

}
