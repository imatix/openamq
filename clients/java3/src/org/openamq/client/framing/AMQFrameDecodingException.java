package org.openamq.client.framing;

import org.openamq.client.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQFrameDecodingException extends AMQException
{
    public AMQFrameDecodingException(String message)
    {
        super(message);
    }

    public AMQFrameDecodingException(String message, Throwable t)
    {
        super(message, t);
    }
}
