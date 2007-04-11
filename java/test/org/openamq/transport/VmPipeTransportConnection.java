package org.openamq.transport;

import org.apache.log4j.Logger;
import org.apache.mina.common.ConnectFuture;
import org.apache.mina.common.IoServiceConfig;
import org.apache.mina.transport.vmpipe.VmPipeAddress;
import org.apache.mina.transport.vmpipe.VmPipeConnector;
import org.openamq.client.AMQConnection;
import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.transport.ITransportConnection;
import org.openamq.pool.PoolingFilter;
import org.openamq.pool.ReferenceCountingExecutorService;

import java.io.IOException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class VmPipeTransportConnection implements ITransportConnection
{
    private static final Logger _logger = Logger.getLogger(VmPipeTransportConnection.class);

    public void connect(AMQProtocolHandler protocolHandler, AMQConnection.BrokerDetail brokerDetail) throws IOException
    {
        final VmPipeConnector ioConnector = new VmPipeConnector();
        final IoServiceConfig cfg = ioConnector.getDefaultConfig();
        ReferenceCountingExecutorService executorService = ReferenceCountingExecutorService.getInstance();
        PoolingFilter asyncRead = new PoolingFilter(executorService, PoolingFilter.READ_EVENTS,
                                                    "AsynchronousReadFilter");
        cfg.getFilterChain().addFirst("AsynchronousReadFilter", asyncRead);
        PoolingFilter asyncWrite = new PoolingFilter(executorService, PoolingFilter.WRITE_EVENTS,
                                                     "AsynchronousWriteFilter");
        cfg.getFilterChain().addLast("AsynchronousWriteFilter", asyncWrite);


        final VmPipeAddress address = new VmPipeAddress(1);
        _logger.info("Attempting connection to " + address);
        ConnectFuture future = ioConnector.connect(address, protocolHandler);
        // wait for connection to complete
        future.join();
        // we call getSession which throws an IOException if there has been an error connecting
        future.getSession();
    }
}
