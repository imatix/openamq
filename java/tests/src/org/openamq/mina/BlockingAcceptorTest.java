package org.openamq.mina;

import junit.framework.JUnit4TestAdapter;
import org.apache.log4j.Logger;
import org.junit.Test;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class BlockingAcceptorTest
{
    private static final Logger _logger = Logger.getLogger(BlockingAcceptorTest.class);

    public static int PORT = 9999;

    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(AcceptorTest.class);
    }

    @Test
    public void startAcceptor() throws IOException
    {

        ServerSocket sock = new ServerSocket(PORT);

        sock.setReuseAddress(true);
        sock.setReceiveBufferSize(32768);
        _logger.info("Bound on port " + PORT);

        while (true)
        {
            final Socket s = sock.accept();
            _logger.info("Received connection from " + s.getRemoteSocketAddress());
            s.setReceiveBufferSize(32768);
            s.setSendBufferSize(32768);
            s.setTcpNoDelay(true);
            new Thread(new Runnable()
            {
                public void run()
                {
                    byte[] chunk = new byte[32768];
                    try
                    {
                        InputStream is = s.getInputStream();
                        OutputStream os = s.getOutputStream();

                        while (true)
                        {
                            int count = is.read(chunk, 0, chunk.length);
                            if (count > 0)
                            {
                                os.write(chunk, 0, count);
                            }
                        }
                    }
                    catch (IOException e)
                    {
                        _logger.error("Error - closing connection: " + e, e);
                    }
                }
            },  "SocketReaderWriter").start();
        }
    }

    public static void main(String[] args) throws IOException
    {
        BlockingAcceptorTest a = new BlockingAcceptorTest();
        a.startAcceptor();
    }
}
