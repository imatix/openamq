package org.openamq.jms;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface ConnectionListener
{
    /**
     * Called when bytes have been transmitted to the server
     * @param count the number of bytes sent in total since the connection was opened
     */
    void bytesSent(long count);

    /**
     * Called when some bytes have been received on a connection
     * @param count the number of bytes received in total since the connection was opened
     */
    void bytesReceived(long count);
}
