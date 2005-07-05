package org.openamq.client.state;

import org.openamq.client.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface StateListener
{
    void stateChanged(AMQState oldState, AMQState newState) throws AMQException;

    void error(Throwable t);
}
