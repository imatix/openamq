package org.openamq.rawclient;

import org.apache.log4j.Logger;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQClientException extends Exception
{

    public AMQClientException(Logger logger, String message)
    {
        super(message);
        logger.error(this, this);
    }

    public AMQClientException(Logger logger, String message, Throwable t)
    {
        super(message, t);
        logger.error(this, this);
    }
}
