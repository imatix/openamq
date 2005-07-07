package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AMQFrame
{
    /**
     * Get the size of buffer needed to store the byte representation of this
     * frame.
     * @return
     */
    public abstract long getSize();

    public abstract void writePayload(ByteBuffer buffer);    
}
