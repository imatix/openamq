package org.openamq.client.transport;

import org.apache.mina.io.socket.SocketConnector;
import org.apache.mina.protocol.io.IoProtocolConnector;
import org.openamq.client.AMQConnection;

import java.io.IOException;
import java.net.InetSocketAddress;

/**
 */
public class TransportConnection
{
    private SocketConnector _socketChannel;

    private AMQProtocolProvider _protocolProvider;

    public TransportConnection(String host, int port,
                               String username, String password,
                               String clientName, String virtualPath,
                               AMQConnection connection)
            throws IOException
    {
        final SocketConnector ioConnector = new SocketConnector();
        final IoProtocolConnector protocolConnector = new IoProtocolConnector(ioConnector);

        final InetSocketAddress address = new InetSocketAddress(host, port);
        _protocolProvider = new AMQProtocolProvider(username, password,
                                                    clientName, virtualPath, connection);
        protocolConnector.connect(address, _protocolProvider);

        // TODO: how do we disconnect?
    }

    public AMQProtocolProvider getProtocolProvider()
    {
        return _protocolProvider;
    }
}
