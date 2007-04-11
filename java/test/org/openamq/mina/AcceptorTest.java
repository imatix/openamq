package org.openamq.mina;

import junit.framework.JUnit4TestAdapter;
import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoAcceptor;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.transport.socket.nio.SocketAcceptor;
import org.apache.mina.transport.socket.nio.SocketAcceptorConfig;
import org.apache.mina.transport.socket.nio.SocketSessionConfig;
import org.junit.Test;
import org.openamq.pool.ReadWriteThreadModel;

import java.io.IOException;
import java.net.InetSocketAddress;

/**
 * Tests MINA socket performance. This acceptor simply reads data from the network and writes it back again.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AcceptorTest
{
    private static final Logger _logger = Logger.getLogger(AcceptorTest.class);

    public static int PORT = 9999;

    private static class TestHandler extends IoHandlerAdapter
    {
        private int _sentCount;

        private int _bytesSent;

        public void messageReceived(IoSession session, Object message) throws Exception
        {
            ((ByteBuffer) message).acquire();
            session.write(message);
            _logger.debug("Sent response " + ++_sentCount);
            _bytesSent += ((ByteBuffer)message).remaining();
            _logger.debug("Bytes sent: " + _bytesSent);
        }

        public void messageSent(IoSession session, Object message) throws Exception
        {
            //((ByteBuffer) message).release();
        }

        public void exceptionCaught(IoSession session, Throwable cause) throws Exception
        {
            _logger.error("Error: " + cause, cause);
        }
    }

    @Test
    public void startAcceptor() throws IOException
    {
        boolean useMultithreadedIoProcessor = Boolean.getBoolean("blaze.io.multithreaded");
        IoAcceptor acceptor = null;
        if (useMultithreadedIoProcessor)
        {
            acceptor = new org.openamq.nio.SocketAcceptor();
        }
        else
        {
            acceptor = new SocketAcceptor();
        }
        SocketAcceptorConfig config = (SocketAcceptorConfig) acceptor.getDefaultConfig();
        SocketSessionConfig sc = (SocketSessionConfig) config.getSessionConfig();
        sc.setTcpNoDelay(true);
        sc.setSendBufferSize(32768);
        sc.setReceiveBufferSize(32768);

        config.setThreadModel(new ReadWriteThreadModel());

        acceptor.bind(new InetSocketAddress(PORT),
                      new TestHandler());
        _logger.info("Bound on port " + PORT);
    }

    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(AcceptorTest.class);
    }

    public static void main(String[] args) throws IOException
    {
        AcceptorTest a = new AcceptorTest();
        a.startAcceptor();
    }
}
