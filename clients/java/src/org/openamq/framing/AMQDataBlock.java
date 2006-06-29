package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * A data block represents something that has a size in bytes and the ability to write itself to a byte
 * buffer (similar to a byte array).
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

    /**
     * Writes the datablock to the specified buffer.
     * @param buffer
     */
    public abstract void writePayload(ByteBuffer buffer);
}
