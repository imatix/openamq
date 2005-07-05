package org.openamq.client.protocol;

import org.openamq.client.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface FrameListener
{
    void frameReceived(FrameEvent evt) throws AMQException;

    /**
     * Callback when an error has occurred. Allows listeners to clean up.
     * @param e
     */
    void error(AMQException e);
}
