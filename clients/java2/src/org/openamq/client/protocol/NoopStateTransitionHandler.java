package org.openamq.client.protocol;

import org.openamq.client.state.StateTransitionHandler;
import org.openamq.client.state.StateAndFrame;
import org.openamq.client.state.AMQState;
import org.openamq.client.AMQException;
import org.apache.mina.common.Session;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class NoopStateTransitionHandler implements StateTransitionHandler
{
    public StateAndFrame messageReceived(AMQState state, Object message, Session session) throws AMQException
    {
        return new StateAndFrame(state, null);
    }
}
