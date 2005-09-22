package org.openamq.jms;


public interface Connection extends javax.jms.Connection
{
    /**
     * @return the maximum number of sessions supported by this Connection
     */
    long getMaximumChannelCount();

    void setConnectionListener(ConnectionListener listener);

    /**
     * Get the connection listener that has been registered with this connection, if any
     * @return the listener or null if none has been set
     */
    ConnectionListener getConnectionListener();
}
