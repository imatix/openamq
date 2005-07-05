package org.openamq.client.state;

/**
 * An event that is fired when the protocol state has changed.
 * 
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQStateChangedEvent
{
    private final AMQState _oldState;

    private final AMQState _newState;

    public AMQStateChangedEvent(AMQState oldState, AMQState newState)
    {
        _oldState = oldState;
        _newState = newState;
    }

    public AMQState getOldState()
    {
        return _oldState;
    }

    public AMQState getNewState()
    {
        return _newState;
    }
}
