package org.openamq.client.transport;

import org.apache.mina.io.socket.SocketConnector;
import org.apache.mina.protocol.io.IoProtocolConnector;

import java.io.IOException;
import java.net.InetSocketAddress;

/**
 */
public class TransportConnection
{
    private SocketConnector _socketChannel;

    public TransportConnection(String host, int port,
                               String username, String password,
                               String clientName, String virtualPath)
            throws IOException
    {
        final SocketConnector ioConnector = new SocketConnector();
        final IoProtocolConnector protocolConnector = new IoProtocolConnector(ioConnector);

        final InetSocketAddress address = new InetSocketAddress(host, port);
        protocolConnector.connect(address, new AMQProtocolProvider(username, password,
                                                                   clientName, virtualPath));

        // TODO: how do we disconnect?
    }
}
