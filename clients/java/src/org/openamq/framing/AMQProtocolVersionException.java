package org.openamq.framing;

/**
 * Exception that is thrown when the client and server differ on expected protocol version (header) information.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQProtocolVersionException extends AMQProtocolHeaderException
{
   public AMQProtocolVersionException(String message)
   {
       super(message);
   }
}
