package org.openamq.client.message;

import org.openamq.framing.BasicContentHeaderProperties;
import org.openamq.framing.ContentHeaderBody;
import org.openamq.AMQException;

import javax.jms.JMSException;
import javax.jms.MessageNotReadableException;
import javax.jms.MessageNotWriteableException;
import java.io.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JMSBytesMessage extends AbstractJMSMessage implements javax.jms.BytesMessage
{
    private static final String MIME_TYPE = "application/octet-stream";

    /**
     * Used for the read* methods.
     */
    private DataInputStream _dis;

    private int _bodyLength;

    private ByteArrayOutputStream _baos;

    /**
     * Used for the write* methods
     */
    private DataOutputStream _dos;

    JMSBytesMessage()
    {
        this(null);
    }

    /**
     * Construct a bytes message with existing data.
     * @param data if data is not null, the message is immediately in read only mode. if data is null, it is in
     * write-only mode
     */
    JMSBytesMessage(byte[] data)
    {
        super(); // this instanties a content header
        getJmsContentHeaderProperties().contentType = MIME_TYPE;
        if (data == null)
        {
            _baos = new ByteArrayOutputStream();
            _dos = new DataOutputStream(_baos);
        }
        else
        {
            _dis = new DataInputStream(new ByteArrayInputStream(data));
            _bodyLength = data.length;
        }
    }

    JMSBytesMessage(long messageNbr, byte[] data, ContentHeaderBody contentHeader)
            throws AMQException
    {
        // TODO: this casting is ugly. Need to review whole ContentHeaderBody idea
        super(messageNbr, (BasicContentHeaderProperties) contentHeader.properties);
        getJmsContentHeaderProperties().contentType = MIME_TYPE;
        _dis = new DataInputStream(new ByteArrayInputStream(data));
        _bodyLength = data.length;
    }

    public void clearBody() throws JMSException
    {
        _dis = null;
        _baos = new ByteArrayOutputStream();
        _dos = new DataOutputStream(_baos);
    }

    public String toBodyString() throws JMSException
    {
        checkReadable();
        try
        {
            return getText();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    /**
     * We reset the stream before and after reading the data. This means that toString() will always output
     * the entire message and also that the caller can then immediately start reading as if toString() had
     * never been called.
     * @return
     * @throws IOException
     */
    private String getText() throws IOException
    {
        // this will use the default platform encoding
        _dis.reset();
        if (_dis.available() == 0)
        {
            return null;
        }
        else
        {
            String data = _dis.readUTF();
            _dis.reset();
            return data;
        }
    }

    public byte[] getData() throws JMSException
    {
        try
        {
            if (_dis == null)
            {
                if (_baos == null)
                {
                    return null;
                }
                else
                {
                    return _baos.toByteArray();
                }
            }
            else
            {
                byte[] data = new byte[_dis.available()];
                _dis.readFully(data);
                return data;
            }
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public String getMimeType()
    {
        return MIME_TYPE;
    }

    public long getBodyLength() throws JMSException
    {
        checkReadable();
        return _bodyLength;
    }

    private void checkReadable() throws MessageNotReadableException
    {
        if (_dis == null)
        {
            throw new MessageNotReadableException("You need to call reset() to make the message readable");
        }
    }

    private void checkWritable() throws MessageNotWriteableException
    {
        if (_dos == null)
        {
            throw new MessageNotWriteableException("You need to call clearBody() to make the message writable");
        }
    }

    public boolean readBoolean() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readBoolean();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public byte readByte() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readByte();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public int readUnsignedByte() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readUnsignedByte();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public short readShort() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readShort();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public int readUnsignedShort() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readUnsignedShort();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public char readChar() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readChar();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public int readInt() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readInt();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public long readLong() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readLong();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public float readFloat() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readFloat();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public double readDouble() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readDouble();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public String readUTF() throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.readUTF();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public int readBytes(byte[] bytes) throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.read(bytes);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public int readBytes(byte[] bytes, int i) throws JMSException
    {
        checkReadable();
        try
        {
            return _dis.read(bytes, 0, i);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeBoolean(boolean b) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeBoolean(b);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeByte(byte b) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeByte(b);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeShort(short i) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeShort(i);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeChar(char c) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeChar(c);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeInt(int i) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeInt(i);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeLong(long l) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeLong(l);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeFloat(float v) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeFloat(v);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeDouble(double v) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeDouble(v);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeUTF(String string) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.writeUTF(string);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeBytes(byte[] bytes) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.write(bytes);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeBytes(byte[] bytes, int offset, int length) throws JMSException
    {
        checkWritable();
        try
        {
            _dos.write(bytes, offset, length);
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }

    public void writeObject(Object object) throws JMSException
    {
        checkWritable();
        throw new JMSException("Not implemented");
    }

    public void reset() throws JMSException
    {
        checkWritable();
        try
        {
            _dos.close();
            _dos = null;
            _dis = new DataInputStream(new ByteArrayInputStream(_baos.toByteArray()));
            _bodyLength = _dis.available();
        }
        catch (IOException e)
        {
            throw new JMSException(e.toString());
        }
    }
}
