package org.openamq.client.state;

import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class IllegalStateTransitionException extends AMQException
{
    private AMQState _originalState;

    private Class _frame;

    public IllegalStateTransitionException(AMQState originalState, Class frame)
    {
        super("No valid state transition defined for receiving frame " + frame +
              " from state " + originalState);
        _originalState = originalState;
        _frame = frame;
    }

    public AMQState getOriginalState()
    {
        return _originalState;
    }

    public Class getFrameClass()
    {
        return _frame;
    }
}
