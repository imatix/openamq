package org.openamq.rawclient;

/**
 * Very simple state management.
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface AMQStates
{
    static final int CONNECTED = 1;

    static final int CONNECTION_CLOSED = 2;

    static final int CHANNEL_CLOSED = 3;
}
