package org.openamq.client.transport;

import org.openamq.client.AMQConnection;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.apache.mina.transport.socket.nio.SocketConnector;
import org.apache.mina.common.ConnectFuture;
import org.apache.log4j.Logger;

import java.io.IOException;
import java.net.InetSocketAddress;

/**
 * The TransportConnection is a helper class responsible for connecting to an AMQ server. It sets up
 * the underlying connector, which currently always uses TCP/IP sockets. It creates the
 * "protocol handler" which deals with MINA protocol events.
 *
 * Could be extended in future to support different transport types by turning this into concrete class/interface
 * combo.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 * @see org.openamq.client.transport.AMQProtocolProvider
 */
public class TransportConnection
{
    private static final Logger _logger = Logger.getLogger(TransportConnection.class);

    public static void connect(AMQProtocolHandler protocolHandler, AMQConnection.BrokerDetail brokerDetail)
            throws IOException
    {
        final SocketConnector ioConnector = new SocketConnector();

        final InetSocketAddress address = new InetSocketAddress(brokerDetail.host, brokerDetail.port);
        _logger.info("Attempting connection to " + address);
        ConnectFuture future = ioConnector.connect(address, protocolHandler);
        // wait for connection to complete
        future.join();
        // we call getSession which throws an IOException if there has been an error connecting
        future.getSession();
    }
}
