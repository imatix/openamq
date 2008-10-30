package org.openamq.framing;

/**
 * Exception that is thrown when the client and server differ on expected protocol version (header) information.
 *
 */
public class AMQProtocolVersionException extends AMQProtocolHeaderException
{
   public AMQProtocolVersionException(String message)
   {
       super(message);
   }
}
