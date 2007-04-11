package org.openamq.ssl;

import javax.net.ServerSocketFactory;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.security.GeneralSecurityException;

/**
 * Simple Server Socket factory to create sockets with or without SSL enabled.
 * If SSL enabled a "bogus" SSL Context is used (suitable for test purposes)
 * <p/>
 * This is based on the example that comes with MINA, written by Trustin Lee.
 */
public class SSLServerSocketFactory extends javax.net.ServerSocketFactory
{
    private static boolean sslEnabled = false;

    private static javax.net.ServerSocketFactory sslFactory = null;

    private static ServerSocketFactory factory = null;

    public SSLServerSocketFactory()
    {
        super();
    }

    public ServerSocket createServerSocket(int port) throws IOException
    {
        return new ServerSocket(port);
    }

    public ServerSocket createServerSocket(int port, int backlog)
            throws IOException
    {
        return new ServerSocket(port, backlog);
    }

    public ServerSocket createServerSocket(int port, int backlog,
                                           InetAddress ifAddress)
            throws IOException
    {
        return new ServerSocket(port, backlog, ifAddress);
    }

    public static javax.net.ServerSocketFactory getServerSocketFactory()
            throws IOException
    {
        if (isSslEnabled())
        {
            if (sslFactory == null)
            {
                try
                {
                    sslFactory = BogusSSLContextFactory.getInstance(true)
                            .getServerSocketFactory();
                }
                catch (GeneralSecurityException e)
                {
                    IOException ioe = new IOException(
                            "could not create SSL socket");
                    ioe.initCause(e);
                    throw ioe;
                }
            }
            return sslFactory;
        }
        else
        {
            if (factory == null)
            {
                factory = new SSLServerSocketFactory();
            }
            return factory;
        }

    }

    public static boolean isSslEnabled()
    {
        return sslEnabled;
    }

    public static void setSslEnabled(boolean newSslEnabled)
    {
        sslEnabled = newSslEnabled;
    }
}
