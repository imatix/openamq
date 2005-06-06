package org.openamq.client.state;

import org.openamq.client.framing.AMQFrame;

/**
 * Contains a frame and an AMQState. Used typically for the result of a state transition.
 *
 * @see org.openamq.client.state.StateAwareProtocolHandler
 * @see org.openamq.client.state.StateTransitionHandler
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class StateAndFrame
{
    public AMQState state;

    public AMQFrame frame;

    public StateAndFrame(AMQState state, AMQFrame frame)
    {
        this.state = state;
        this.frame = frame;
    }
}
