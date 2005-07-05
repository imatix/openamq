package org.openamq.jms;


public interface Connection extends javax.jms.Connection
{
    /**
     * @return the maximum number of sessions supported by this Connection
     */
    long getMaximumChannelCount();
}
