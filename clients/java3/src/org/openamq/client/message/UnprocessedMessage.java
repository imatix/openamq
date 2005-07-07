package org.openamq.client.message;

import org.openamq.client.framing.AMQMessage;

/**
 * This class contains everything extracted from one (or more, if fragmented)
 * Handle.Notify to enable a message factory to construct the appropriate
 * concrete message.
 *
 * This class exists primarily to allow one data unit to be handed off to the
 * session thread from the MINA dispatcher thread, thereby minimising the amount
 * of time spent in the MINA dispatcher thread.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class UnprocessedMessage
{
    public int handleId;
    public AMQMessage message;
    public long messageNbr;
    public boolean redelivered;
}
