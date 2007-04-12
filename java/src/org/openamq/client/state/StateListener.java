package org.openamq.client.state;

import org.openamq.AMQException;

public interface StateListener
{
    void stateChanged(AMQState oldState, AMQState newState) throws AMQException;

    void error(Throwable t);
}
