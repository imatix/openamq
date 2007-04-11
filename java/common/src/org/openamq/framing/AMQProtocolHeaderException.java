package org.openamq.framing;

import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQProtocolHeaderException extends AMQException
{
   public AMQProtocolHeaderException(String message)
   {
       super(message);
   }
}
