package org.openamq.stac;

import org.openamq.management.jmx.AMQConsole;
import org.openamq.stac.jmx.MBeanServerConnectionContext;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionTest
{
    public static void main(String[] args) throws Exception
    {

        AMQConsole console = new AMQConsole("localhost", 5672, "guest", "guest", "/test");
        console.initialise();
        console.registerAllMBeans();
        MBeanServerConnectionContext connectionContext = new MBeanServerConnectionContext();
        connectionContext.connect();
        connectionContext.ls();
    }
}
