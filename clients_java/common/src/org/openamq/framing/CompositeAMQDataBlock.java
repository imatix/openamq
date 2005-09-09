package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CompositeAMQDataBlock extends AMQDataBlock implements EncodableAMQDataBlock
{
    private AMQDataBlock[] _blocks;

    public CompositeAMQDataBlock(AMQDataBlock[] blocks)
    {
        _blocks = blocks;
    }

    public AMQDataBlock[] getBlocks()
    {
        return _blocks;
    }

    public long getSize()
    {
        long frameSize = 0;
        for (int i = 0; i < _blocks.length; i++)
        {
            frameSize += _blocks[i].getSize();
        }
        return frameSize;
    }

    public void writePayload(ByteBuffer buffer)
    {
        for (int i = 0; i < _blocks.length; i++)
        {
            _blocks[i].writePayload(buffer);
        }
    }
}
