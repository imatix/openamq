package org.openamq.rawclient;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface MessageReceivedCallback
{
    /**
     * Callback for messages received.
     * @param m
     * @throws AMQClientException
     */
    void onMessage(Message m) throws AMQClientException;
}
