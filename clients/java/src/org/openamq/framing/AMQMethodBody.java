package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;
import org.openamq.AMQChannelException;

public abstract class AMQMethodBody extends AMQBody
{
    public static final byte TYPE = 1;

    public long synchtag;

    /** unsigned short */
    protected abstract int getBodySize();

    /**
     * @return unsigned short
     */
    protected abstract int getClazz();

    /**
     * @return unsigned short
     */
    protected abstract int getMethod();

    public int getId() {
        return 1000 * getClazz() + getMethod();
    }

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
        EncodingUtils.writeUnsignedShort(buffer, getClazz());
        EncodingUtils.writeUnsignedShort(buffer, getMethod());
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
        buf.append("\n\tClass: ").append(getClazz());
        buf.append("\n\tMethod: ").append(getMethod());
        return buf.toString();
    }

    /**
     * Creates an AMQChannelException for the corresponding body type (a channel exception
     * should include the class and method ids of the body it resulted from).
     */
    public AMQChannelException getChannelException(int code, String message)
    {
        return new AMQChannelException(code, message, getClazz(), getMethod());
    }

    public AMQChannelException getChannelException(int code, String message, Throwable cause)
    {
        return new AMQChannelException(code, message, getClazz(), getMethod(), cause);
    }
}
