package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

public abstract class AMQMethodBody extends AMQBody
{
    public static final byte TYPE = 1;

    public long synchtag;

    /** unsigned short */
    protected abstract int getBodySize();

    /**
     * @return unsigned short
     */
    public abstract int getClassId();

    /**
     * @return unsigned short
     */
    public abstract int getMethodId();

    protected abstract void writeMethodPayload(ByteBuffer buffer);

    protected byte getType()
    {
        return TYPE;
    }

    protected int getSize()
    {
        return 2 + 2 + getBodySize();
    }

    protected void writePayload(ByteBuffer buffer)
    {
        EncodingUtils.writeUnsignedShort(buffer, getClassId());
        EncodingUtils.writeUnsignedShort(buffer, getMethodId());
        writeMethodPayload(buffer);
    }

    protected abstract void populateMethodBodyFromBuffer(ByteBuffer buffer) throws AMQFrameDecodingException;

    protected void populateFromBuffer(ByteBuffer buffer, long size) throws AMQFrameDecodingException
    {
        populateMethodBodyFromBuffer(buffer);
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer(getClass().toString());
        buf.append("\n\tClass: ").append(getClassId());
        buf.append("\n\tMethod: ").append(getMethodId());
        return buf.toString();
    }
}
