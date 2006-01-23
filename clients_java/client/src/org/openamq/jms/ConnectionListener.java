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

    /**
     * Called after the infrastructure has detected that failover is required but before attempting failover.
     * @return true to continue failing over, false to veto failover and raise a connection exception
     */
    boolean preFailover();

    /**
     * Called after connection has been made to another broker after failover has been started but before
     * any resubscription has been done.
     * @return true to continue with resubscription, false to prevent automatic resubscription. This is useful in
     * cases where the application wants to handle resubscription. Note that in the latter case all sessions, producers
     * and consumers are invalidated.
     */
    boolean preResubscribe();

    /**
     * Called once failover has completed successfully. This is called irrespective of whether the client has
     * vetoed automatic resubscription.
     */
    void failoverComplete();
}
