package org.openamq.client.state;

/**
 * States used in the AMQ protocol. Used by the finite state machine to determine
 * valid responses.
 */
public class AMQState
{
    private int _id;

    private String _name;

    private AMQState(int id, String name)
    {
        _id = id;
        _name = name;
    }

    public String toString()
    {
        return "AMQState: id = " + _id + " name: " + _name;
    }

    public static final AMQState CONNECTION_NOT_AUTHENTICATED = new AMQState(1, "CONNECTION_NOT_AUTHENTICATED");

    public static final AMQState CONNECTION_TUNED = new AMQState(2, "CONNECTION_TUNED");

    public static final AMQState CONNECTION_CLOSED = new AMQState(3, "CONNECTION_CLOSED");
}
