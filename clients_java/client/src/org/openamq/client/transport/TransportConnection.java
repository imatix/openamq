package org.openamq.client.transport;

import org.apache.mina.io.socket.SocketConnector;
import org.apache.mina.protocol.io.IoProtocolConnector;
import org.openamq.client.AMQConnection;
import org.openamq.client.protocol.AMQProtocolHandler;

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
            throws IOException
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
        final IoProtocolConnector protocolConnector = new IoProtocolConnector(ioConnector);

        final InetSocketAddress address = new InetSocketAddress(_connection.getHost(), _connection.getPort());
        _protocolProvider = new AMQProtocolProvider(_connection);
        protocolConnector.connect(address, _protocolProvider);
        // TODO: how do we disconnect?
        return (AMQProtocolHandler) _protocolProvider.getHandler();
    }

    /**
     * @return the protocol provider created to handle sending and receiving of data
     */
    public AMQProtocolProvider getProtocolProvider()
    {
        return _protocolProvider;
    }
}
