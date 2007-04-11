package org.openamq.framing;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQProtocolInstanceException extends AMQProtocolHeaderException
{
   public AMQProtocolInstanceException(String message)
   {
       super(message);
   }
}
