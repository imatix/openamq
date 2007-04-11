package org.openamq.client.protocol;

/**
 * This exception is thrown when failover is taking place and we need to let other
 * parts of the client know about this.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class FailoverException extends RuntimeException
{
    public FailoverException(String message)
    {
        super(message);
    }
}
