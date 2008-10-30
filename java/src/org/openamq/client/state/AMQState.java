package org.openamq.client.state;

/**
 * States used in the AMQ protocol. Used by the finite state machine to determine
 * valid responses.
 */
public class AMQState
{
    private final int _id;

    private final String _name;

    private AMQState(int id, String name)
    {
        _id = id;
        _name = name;
    }

    public String toString()
    {
        return "AMQState: id = " + _id + " name: " + _name;
    }

    public static final AMQState CONNECTION_NOT_STARTED = new AMQState(1, "CONNECTION_NOT_STARTED");
    
    public static final AMQState CONNECTION_NOT_TUNED = new AMQState(2, "CONNECTION_NOT_TUNED");
    
    public static final AMQState CONNECTION_NOT_OPENED = new AMQState(3, "CONNECTION_NOT_OPENED");        

    public static final AMQState CONNECTION_OPEN = new AMQState(4, "CONNECTION_OPEN");

    public static final AMQState CONNECTION_CLOSING = new AMQState(5, "CONNECTION_CLOSING");
    
    public static final AMQState CONNECTION_CLOSED = new AMQState(6, "CONNECTION_CLOSED");
    
}
