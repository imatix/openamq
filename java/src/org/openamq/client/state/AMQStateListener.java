package org.openamq.client.state;

public interface AMQStateListener
{
    void stateChanged(AMQStateChangedEvent evt);
}
