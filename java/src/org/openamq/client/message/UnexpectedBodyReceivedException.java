package org.openamq.client.message;

import org.openamq.AMQException;
import org.apache.log4j.Logger;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class UnexpectedBodyReceivedException extends AMQException
{

    public UnexpectedBodyReceivedException(Logger logger, String msg, Throwable t)
    {
        super(logger, msg, t);
    }

    public UnexpectedBodyReceivedException(Logger logger, String msg)
    {
        super(logger, msg);
    }

    public UnexpectedBodyReceivedException(Logger logger, int errorCode, String msg)
    {
        super(logger, errorCode, msg);
    }
}
