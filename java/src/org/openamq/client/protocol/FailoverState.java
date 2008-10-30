package org.openamq.client.protocol;

/**
 * Enumeration of failover states. Used to handle failover from within AMQProtocolHandler where MINA events need to be
 * dealt with and can happen during failover.
 *
 */
final class FailoverState
{
    private final String _state;

    /** Failover has not yet been attempted on this connection */
    static final FailoverState NOT_STARTED = new FailoverState("NOT STARTED");

    /** Failover has been requested on this connection but has not completed */
    static final FailoverState IN_PROGRESS = new FailoverState("IN PROGRESS");

    /** Failover has been attempted and failed */
    static final FailoverState FAILED = new FailoverState("FAILED");

    private FailoverState(String state)
    {
        _state = state;
    }

    public String toString()
    {
        return "FailoverState: " + _state;
    }
}
