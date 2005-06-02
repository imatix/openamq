package org.openamq.rawclient;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionFactory
{
    public static Connection createConnection(String host, int port, String username,
                                       String password, int flags) throws AMQClientException
    {
        return new Connection(host, port, username, password);
    }
}
