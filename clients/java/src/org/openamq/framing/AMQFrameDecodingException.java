package org.openamq.framing;

import org.apache.log4j.Logger;
import org.openamq.AMQException;

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

    public AMQFrameDecodingException(Logger log, String message)
    {
        super(log, message);
    }

    public AMQFrameDecodingException(Logger log, String message, Throwable t)
    {
        super(log, message, t);
    }

}
