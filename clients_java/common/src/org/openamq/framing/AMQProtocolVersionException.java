package org.openamq.framing;

import org.openamq.AMQException;

/**
 * Exception that is thrown when the client and server differ on expected protocol version (header) information.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQProtocolVersionException extends AMQException
{
   public AMQProtocolVersionException(String message)
   {
       super(message);
   }
}
