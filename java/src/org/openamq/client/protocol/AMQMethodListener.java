package org.openamq.client.protocol;

import org.openamq.AMQException;

public interface AMQMethodListener
{
    /**
     * Invoked when a method frame has been received
     * @param evt the event
     * @return true if the handler has processed the method frame, false otherwise. Note
     * that this does not prohibit the method event being delivered to subsequent listeners
     * but can be used to determine if nobody has dealt with an incoming method frame.
     * @throws AMQException if an error has occurred. This exception will be delivered
     * to all registered listeners using the error() method (see below) allowing them to
     * perform cleanup if necessary.
     */
    boolean methodReceived(AMQMethodEvent evt) throws AMQException;

    /**
     * Callback when an error has occurred. Allows listeners to clean up.
     * @param e
     */
    void error(Exception e);
}
