package org.openamq.rawclient;

import org.openamq.rawclient.Connection;
import org.openamq.rawclient.ConnectionFactory;
import org.openamq.rawclient.QueueWriter;
import org.openamq.rawclient.Message;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class SimpleTest1
{
    public static void main(String[] args)
    {
        String host = args[0];
        int port = Integer.parseInt(args[1]);

        try
        {
            Connection con = ConnectionFactory.createConnection(host, port, "mahnah", "mahanah", 0);
            QueueWriter qw = con.createQueueWriter("test", false, 0);
            Message msg = new Message("pies");
            qw.write(msg);
            System.out.println("Completed.");
            System.exit(1);
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }

    }
}
