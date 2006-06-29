package org.openamq.transport;

import org.apache.log4j.Logger;
import org.apache.mina.transport.socket.nio.SocketConnector;
import org.apache.mina.common.ConnectFuture;
import org.openamq.protocol.AMQProtocolHandler;

import java.net.InetSocketAddress;
import java.io.IOException;

public class AMQSocketConnection {
    private static final Logger
        _logger = Logger.getLogger(AMQSocketConnection.class);

    public void connect(AMQProtocolHandler aph, InetSocketAddress address) throws IOException {
        // Start connecting
        _logger.info("Connecting to: " + address);
        SocketConnector
            connector = new SocketConnector();
        ConnectFuture
            future = connector.connect(address, aph);

        // Make connection synchronous
        future.join();
        // Get the protocol session
        future.getSession();
    }
}
