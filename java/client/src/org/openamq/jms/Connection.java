package org.openamq.jms;

import javax.jms.*;


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

    /**
     * Create a session specifying the prefetch limit of messages.
     * @param transacted
     * @param acknowledgeMode
     * @param prefetch the maximum number of messages to buffer in the client. This
     * applies as a total across all consumers
     * @return
     * @throws JMSException
     */
    org.openamq.jms.Session createSession(boolean transacted, int acknowledgeMode,
                                          int prefetch) throws JMSException;
}
