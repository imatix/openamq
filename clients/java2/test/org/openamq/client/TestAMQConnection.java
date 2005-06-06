package org.openamq.client;

import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestAMQConnection
{
    public static void main(String[] args)
    {
        if (args.length != 5)
        {
            System.out.println("Usage: host port username password virtual-path");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            AMQConnection con = new AMQConnection(args[0], Integer.parseInt(args[1]), "guest", "guest",
                                                  address.getHostName(), "/test");
            System.out.println("Test complete.");
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }

    }
}
