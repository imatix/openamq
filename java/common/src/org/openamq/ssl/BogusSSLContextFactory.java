package org.openamq.ssl;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import java.io.IOException;
import java.io.InputStream;
import java.security.GeneralSecurityException;
import java.security.KeyStore;

/**
 * Factory to create a bogus SSLContext. This means that it is easy to test SSL but this
 * cannot be used in a production environment.
 * <p/>
 * This is based on the sample that comes with MINA, written by Trustin Lee
 */
public class BogusSSLContextFactory
{

    /**
     * Protocol to use.
     */
    private static final String PROTOCOL = "TLS";

    /**
     * Bougus Server certificate keystore file name.
     */
    private static final String BOGUS_KEYSTORE = "blaze.cert";

    // NOTE: The keystore was generated using keytool:
    // keytool -genkey -alias blaze -keysize 512 -validity 3650
    // -keyalg RSA -dname "CN=blaze.jpmorgan.com, OU=IBTech,
    // O=JPMorgan Chase, L=Glasgow, S=Glasgow, C=GB" -keypass blazepw
    // -storepass blazepw -keystore blaze.cert

    /**
     * Bougus keystore password.
     */
    private static final char[] BOGUS_PW = {'b', 'l', 'a', 'z', 'e', 'p',
            'w'};

    private static SSLContext serverInstance = null;

    private static SSLContext clientInstance = null;

    /**
     * Get SSLContext singleton.
     *
     * @return SSLContext
     * @throws java.security.GeneralSecurityException
     *
     */
    public static SSLContext getInstance(boolean server)
            throws GeneralSecurityException
    {
        SSLContext retInstance = null;
        if (server)
        {
            if (serverInstance == null)
            {
                synchronized (BogusSSLContextFactory.class)
                {
                    if (serverInstance == null)
                    {
                        try
                        {
                            serverInstance = createBougusServerSSLContext();
                        }
                        catch (Exception ioe)
                        {
                            throw new GeneralSecurityException(
                                    "Can't create Server SSLContext:" + ioe);
                        }
                    }
                }
            }
            retInstance = serverInstance;
        }
        else
        {
            if (clientInstance == null)
            {
                synchronized (BogusSSLContextFactory.class)
                {
                    if (clientInstance == null)
                    {
                        clientInstance = createBougusClientSSLContext();
                    }
                }
            }
            retInstance = clientInstance;
        }
        return retInstance;
    }

    private static SSLContext createBougusServerSSLContext()
            throws GeneralSecurityException, IOException
    {
        // Create keystore
        KeyStore ks = KeyStore.getInstance("JKS");
        InputStream in = null;
        try
        {
            in = BogusSSLContextFactory.class
                    .getResourceAsStream(BOGUS_KEYSTORE);
            if (in == null)
            {
                throw new IOException("Unable to load keystore resource: " + BOGUS_KEYSTORE);
            }
            ks.load(in, BOGUS_PW);
        }
        finally
        {
            if (in != null)
            {
                try
                {
                    in.close();
                }
                catch (IOException ignored)
                {
                }
            }
        }

        // Set up key manager factory to use our key store
        KeyManagerFactory kmf = KeyManagerFactory.getInstance("SunX509");
        kmf.init(ks, BOGUS_PW);

        // Initialize the SSLContext to work with our key managers.
        SSLContext sslContext = SSLContext.getInstance(PROTOCOL);
        sslContext.init(kmf.getKeyManagers(),
                        BogusTrustManagerFactory.X509_MANAGERS, null);

        return sslContext;
    }

    private static SSLContext createBougusClientSSLContext()
            throws GeneralSecurityException
    {
        SSLContext context = SSLContext.getInstance(PROTOCOL);
        context.init(null, BogusTrustManagerFactory.X509_MANAGERS, null);
        return context;
    }

}
