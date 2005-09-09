package org.openamq.client.state;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface AMQStateListener
{
    void stateChanged(AMQStateChangedEvent evt);
}
