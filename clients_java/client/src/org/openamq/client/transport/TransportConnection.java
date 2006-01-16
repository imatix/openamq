package org.openamq.client.transport;

import org.openamq.client.AMQConnection;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.apache.mina.transport.socket.nio.SocketConnector;
import org.apache.mina.common.ConnectFuture;

import java.io.IOException;
import java.net.InetSocketAddress;

/**
 * The TransportConnection is responsible for connecting to an AMQ server. It sets up
 * the underlying connector, which currently always uses TCP/IP sockets. It creates a
 * "protocol provider" which handles the sending and receiving of data on the connection.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 * @see org.openamq.client.transport.AMQProtocolProvider
 */
public class TransportConnection
{
    private SocketConnector _socketChannel;

    private AMQProtocolProvider _protocolProvider;

    private AMQConnection _connection;

    public TransportConnection(AMQConnection connection)
    {
        if (connection == null)
        {
            throw new IllegalArgumentException("AMQConnection must not be null");
        }
        _connection = connection;
    }

    public AMQProtocolHandler connect()
            throws IOException
    {
        final SocketConnector ioConnector = new SocketConnector();        

        final InetSocketAddress address = new InetSocketAddress(_connection.getHost(), _connection.getPort());
        final AMQProtocolHandler protocolHandler = new AMQProtocolHandler(_connection);
        ConnectFuture future = ioConnector.connect(address, protocolHandler);
        // wait for connection to complete
        future.join();
        return protocolHandler;
    }

    /**
     * @return the protocol provider created to handle sending and receiving of data
     */
    public AMQProtocolProvider getProtocolProvider()
    {
        return _protocolProvider;
    }
}
