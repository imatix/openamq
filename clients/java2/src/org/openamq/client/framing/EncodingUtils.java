package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.openamq.client.AMQException;

import java.io.UnsupportedEncodingException;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class EncodingUtils
{
    private static final String STRING_ENCODING = "iso8859-15";

    private static final Charset _charset = Charset.forName("iso8859-15");

    public static short encodedShortStringLength(String s)
    {
        if (s == null)
        {
            return 1;
        }
        else
        {
            return (short) (1 + s.length());
        }
    }

    public static int encodedLongStringLength(String s)
    {
        if (s == null)
        {
            return 2;
        }
        else
        {
            return 2 + s.length();
        }
    }

    public static int encodedFieldTableLength(FieldTable table)
    {
        if (table == null)
        {
            // size is encoded as 2 octets
            return 2;
        }
        else
        {
            // size of the table plus 2 octets for the size
            return table.getEncodedSize() + 2;
        }
    }

    public static void writeShortStringBytes(ByteBuffer buffer, String s)
    {
        assert s == null || s.length() <= 0xFE;

        if (s != null)
        {
            buffer.put((byte)s.length());
            try
            {
                buffer.put(s.getBytes(STRING_ENCODING));
            }
            catch (UnsupportedEncodingException e)
            {
                // IGNORE
            }
        }
        else
        {
            buffer.put((byte) 0);
        }
    }

    public static void writeLongStringBytes(ByteBuffer buffer, String s)
    {
        assert s == null || s.length() <= 0xFFFE;
        if (s != null)
        {
            //buffer.putShort((short)s.length());
            writeUnsignedShort(buffer, s.length());
            try
            {
                buffer.put(s.getBytes(STRING_ENCODING));
            }
            catch (UnsupportedEncodingException e)
            {
                // IGNORE
            }
        }
        else
        {
            buffer.putShort((short) 0);
        }
    }

    public static void writeUnsignedByte(ByteBuffer buffer, short b)
    {
        byte bv = (byte) b;
        buffer.put(bv);
    }

    public static void writeUnsignedShort(ByteBuffer buffer, int s)
    {
        short sv = (short) s;
        buffer.put((byte) (0xFF & (sv >> 8)));
        buffer.put((byte)(0xFF & sv));
    }

    public static void writeUnsignedInteger(ByteBuffer buffer, long l)
    {
        int iv = (int) l;
        buffer.put((byte) (0xFF & (iv >> 24)));
        buffer.put((byte) (0xFF & (iv >> 16)));
        buffer.put((byte) (0xFF & (iv >> 8 )));
        buffer.put((byte) (0xFF & iv));
    }

    public static void writeFieldTableBytes(ByteBuffer buffer, FieldTable table)
    {
        if (table != null)
        {
            table.writeToBuffer(buffer);
        }
        else
        {
            buffer.put((byte) 0);
        }
    }

    public static void writeBoolean(ByteBuffer buffer, boolean value)
    {
        if (value)
        {
            buffer.put((byte) 1);
        }
        else
        {
            buffer.put((byte)0);
        }
    }

    public static boolean readBoolean(ByteBuffer buffer)
    {
        return (buffer.get() != 0);
    }

    public static FieldTable readFieldTable(ByteBuffer buffer) throws AMQFrameDecodingException
    {
        int length = buffer.getUnsignedShort();
        if (length == 0)
        {
            return null;
        }
        else
        {
            return new FieldTable(buffer, length);
        }
    }

    public static String readShortString(ByteBuffer buffer) throws AMQFrameDecodingException
    {
        byte length = buffer.get();
        if (length == 0)
        {
            return null;
        }
        else
        {
            // TODO: improve this so that we do not construct a new decoder every time
            // note that CharsetDecoder instances are NOT thread safe
            try
            {
                return buffer.getString(length, _charset.newDecoder());
            }
            catch (CharacterCodingException e)
            {
                throw new AMQFrameDecodingException("Unable to decode short string: " + e, e);
            }
        }
    }

    public static String readLongString(ByteBuffer buffer) throws AMQFrameDecodingException
    {
        short length = buffer.getShort();
        if (length == 0)
        {
            return null;
        }
        else
        {
            // TODO: improve this so that we do not construct a new decoder every time
            // note that CharsetDecoder instances are NOT thread safe
            try
            {
                return buffer.getString(length, _charset.newDecoder());
            }
            catch (CharacterCodingException e)
            {
                throw new AMQFrameDecodingException("Unable to decode long string: " + e, e);
            }
        }
    }
}
