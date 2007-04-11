package org.openamq.client.transport;

import org.apache.log4j.Logger;
import org.apache.mina.common.ConnectFuture;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.SimpleByteBufferAllocator;
import org.apache.mina.transport.socket.nio.SocketConnector;
import org.apache.mina.transport.socket.nio.SocketConnectorConfig;
import org.apache.mina.transport.socket.nio.SocketSessionConfig;
import org.openamq.client.AMQConnection;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.pool.ReadWriteThreadModel;

import java.io.IOException;
import java.net.InetSocketAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class SocketTransportConnection implements ITransportConnection
{
    private static final Logger _logger = Logger.getLogger(SocketTransportConnection.class);

    public void connect(AMQProtocolHandler protocolHandler, AMQConnection.BrokerDetail brokerDetail)
            throws IOException
    {
        ByteBuffer.setUseDirectBuffers(Boolean.getBoolean("amqj.enableDirectBuffers"));

        // the MINA default is currently to use the pooled allocator although this may change in future
        // once more testing of the performance of the simple allocator has been done
        if (!Boolean.getBoolean("amqj.enablePooledAllocator"))
        {
            ByteBuffer.setAllocator(new SimpleByteBufferAllocator());
        }


        final SocketConnector ioConnector = new SocketConnector();
        SocketConnectorConfig cfg = (SocketConnectorConfig) ioConnector.getDefaultConfig();

        // if we do not use our own thread model we get the MINA default which is to use
        // its own leader-follower model
        boolean readWriteThreading = Boolean.getBoolean("amqj.shared_read_write_pool");
        if (readWriteThreading)
        {
            cfg.setThreadModel(new ReadWriteThreadModel());
        }

        SocketSessionConfig scfg = (SocketSessionConfig) cfg.getSessionConfig();
        scfg.setTcpNoDelay("true".equalsIgnoreCase(System.getProperty("amqj.tcpNoDelay", "true")));
        scfg.setSendBufferSize(Integer.getInteger("amqj.sendBufferSize", 32768));
        scfg.setReceiveBufferSize(Integer.getInteger("amqj.receiveBufferSize", 32768));
        final InetSocketAddress address = new InetSocketAddress(brokerDetail.host, brokerDetail.port);
        protocolHandler.setUseSSL(brokerDetail.useSSL);
        _logger.info("Attempting connection to " + address);
        ConnectFuture future = ioConnector.connect(address, protocolHandler);
        // wait for connection to complete
        future.join();
        // we call getSession which throws an IOException if there has been an error connecting
        future.getSession();
    }
}
