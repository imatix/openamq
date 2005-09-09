package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AMQDataBlock implements EncodableAMQDataBlock
{
    /**
     * Get the size of buffer needed to store the byte representation of this
     * frame.
     * @return unsigned integer
     */
    public abstract long getSize();

    public abstract void writePayload(ByteBuffer buffer);
}
