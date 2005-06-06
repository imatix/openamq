package org.openamq.client.state;

import org.apache.mina.common.Session;
import org.openamq.client.AMQException;

/**
 */
public interface StateTransitionHandler
{
    /**
     * Act on a message received.
     * @param state the state we are currently in. Generally this will be ignored since most
     * handlers will be state specific but some handlers will be applicable in multiple states.
     * @param message the message(frame) that has been received
     * @param session the protocol session. This is used to store and retrieve session-specific
     * data, in order to carry out a "conversation".
     * @return the new state and a frame to be sent in response. May be null indicating that the cliet
     * should not send any response.
     * @throws AMQException if the transition is illegal or if the message is invalid for some
     * reason
     */
    StateAndFrame messageReceived(AMQState state, Object message, Session session) throws AMQException;
}
