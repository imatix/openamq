package org.openamq.client.state;

import org.openamq.client.protocol.FrameEvent;
import org.openamq.client.AMQException;

/**
 * A frame listener that is informed of the protocl state when invoked and has
 * the opportunity to update state.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface StateAwareFrameListener
{
    void frameReceived(AMQStateManager stateManager, FrameEvent evt) throws AMQException;
}
