package org.openamq.rawclient;

import org.openamq.rawclient.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class SimpleTest2
{
    public static void main(String[] args)
    {
        String host = args[0];
        int port = Integer.parseInt(args[1]);

        try
        {
            Connection con = ConnectionFactory.createConnection(host, port, "mahnah", "mahanah", 0);
            QueueReader qr = con.createQueueReader("test", false, true, new MessageReceivedCallback()
            {
                public void onMessage(Message m) throws AMQClientException
                {
                    System.out.println("Message received: " + String.valueOf(m));
                }
            });
            con.readMessage();
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
