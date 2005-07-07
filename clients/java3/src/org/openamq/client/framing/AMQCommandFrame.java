package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class AMQCommandFrame extends AMQDataBlock
{

    public final long getSize()
    {
        // two size octets plus one end octet as a minimum
        long size = getCommandSize();

        if (size > 0xFFFE)
        {
            // 1 long marker, 2 for the size and 1 end byte
            return 5 + size;
        }
        else
        {
            // 2 for size indicator and 1 for type and another 1 end byte
            return 4 + size;
        }
    }

    protected abstract long getCommandSize();

    public abstract short getType();

    /**
     * Write the data to the supplied buffer.
     * @param buffer the buffer to which to write the data
     */
    protected abstract void writeCommandPayload(ByteBuffer buffer);

    /**
     * Populate an instance from a supplied buffer
     * @param buffer the buffer containing the data to read
     */
    public abstract void populateFromBuffer(ByteBuffer buffer)
            throws AMQFrameDecodingException;

    public void writePayload(ByteBuffer buffer)
    {
        // size is payload + 1 for type + 1 byte for the end of frame marker
        final long size = getCommandSize() + 2;
        if (size > 0xFFFE)
        {
            EncodingUtils.writeUnsignedShort(buffer, 0xFFFF);
            EncodingUtils.writeUnsignedInteger(buffer, size);
        }
        else
        {
            EncodingUtils.writeUnsignedShort(buffer, (int) size);
        }

        EncodingUtils.writeUnsignedByte(buffer, getType());
        writeCommandPayload(buffer);
        // write the end of command frame marker
        buffer.put((byte)0xCE);
    }
}
