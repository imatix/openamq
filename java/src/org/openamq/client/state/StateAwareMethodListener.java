package org.openamq.client.state;

import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.AMQException;

/**
 * A frame listener that is informed of the protocl state when invoked and has
 * the opportunity to update state.
 *
 */
public interface StateAwareMethodListener
{
    void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException;
}
