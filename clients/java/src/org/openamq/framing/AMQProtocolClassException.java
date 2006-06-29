package org.openamq.framing;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQProtocolClassException extends AMQProtocolHeaderException
{
   public AMQProtocolClassException(String message)
   {
       super(message);
   }
}