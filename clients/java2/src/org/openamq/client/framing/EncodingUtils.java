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

    public static final int SIZEOF_UNSIGNED_SHORT = 2;
    public static final int SIZEOF_UNSIGNED_INT = 4;

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
        if (s != null)
        {
            //try
            //{
                //final byte[] encodedString = s.getBytes(STRING_ENCODING);            
                final byte[] encodedString = s.getBytes();            
                assert encodedString.length <= 0xFE;
                buffer.put((byte)encodedString.length);            
                //buffer.put(s.getBytes(STRING_ENCODING));
                buffer.put(encodedString);
            //}
            //catch (UnsupportedEncodingException e)
            //{
                // IGNORE
            //}
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
    	// TODO: Is this comparison safe? Do I need to cast RHS to long?
    	if (s < Short.MAX_VALUE)
    	{
    		buffer.putShort((short)s);
    	}
    	else
    	{
	    	short sv = (short) s;
	    	buffer.put((byte) (0xFF & (sv >> 8)));
	        buffer.put((byte)(0xFF & sv));
    	}
    }

    public static void writeUnsignedInteger(ByteBuffer buffer, long l)
    {
    	// TODO: Is this comparison safe? Do I need to cast RHS to long?
    	if (l < Integer.MAX_VALUE)
    	{
    		buffer.putInt((int)l);
    	}
    	else
    	{
	        int iv = (int) l;
	        
	        // FIXME: This *may* go faster if we build this into a local 4-byte array and then
	        // put the array in a single call.
	        buffer.put((byte) (0xFF & (iv >> 24)));
	        buffer.put((byte) (0xFF & (iv >> 16)));
	        buffer.put((byte) (0xFF & (iv >> 8 )));
	        buffer.put((byte) (0xFF & iv));
    	}
    }

    public static void writeFieldTableBytes(ByteBuffer buffer, FieldTable table)
    {
        if (table != null)
        {
            table.writeToBuffer(buffer);
        }
        else
        {
            EncodingUtils.writeUnsignedShort(buffer, 0);
        }
    }

    public static void writeBooleans(ByteBuffer buffer, boolean[] values)
    {
        byte packedValue = 0;
        for (int i = 0; i < values.length; i++)
        {
            if (values[i])
            {
                packedValue = (byte)(packedValue | (1 << i));
            }
        }

        buffer.put(packedValue);
    }

    public static boolean[] readBooleans(ByteBuffer buffer)
    {
        byte packedValue = buffer.get();
        boolean[] result = new boolean[8];

        for (int i = 0; i < 8; i++)
        {
            result[i] = ((packedValue & (1 << i)) != 0);
        }
        return result;
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
    
    
    
    public static final FieldTable createFieldTableFromMessageSelector(String selector)
    {
    	return(null);
    	
    }
    
	static byte[] hexToByteArray(String id)
	{
		// Should check param for null, long enough for this check, upper-case and trailing char
		String s = (id.charAt(1) == 'x') ? id.substring(2) : id;	// strip 0x

		int len = s.length();
		int byte_len = len / 2;
		byte[] b = new byte[byte_len];

		for(int i = 0; i < byte_len; i++)
		{
			// fixme: refine these repetitive subscript calcs.
			int ch = i * 2;

			byte b1 = Byte.parseByte(s.substring(ch,ch + 1),16);
			byte b2 = Byte.parseByte(s.substring(ch + 1,ch + 2),16);

			b[i] = (byte)(b1 * 16 + b2);
		}

		return(b);
	}

	public static char[] convertToHexCharArray(byte[] from)
	{
		int	length = from.length;
		char[]	result_buff = new char[length * 2 + 2];

		result_buff[0] = '0';
		result_buff[1] = 'x';

		int		bite;
		int		dest = 2;

		for(int i = 0; i < length; i++)
		{
			bite = from[i];

			if (bite < 0) bite += 256;

			result_buff[dest++] = hex_chars[bite >> 4];
			result_buff[dest++] = hex_chars[bite & 0x0f];
		}

		return(result_buff);
	}

	public static String convertToHexString(byte[] from)
	{
		return(new String(convertToHexCharArray(from)));
	}

	public static String convertToHexString(ByteBuffer bb)
	{
		int size = bb.limit();
		
		byte[] from = new byte[size];
		
		for(int i = 0; i < size; i++)
		{
			from[i] = bb.get(i);
		}
		
		return(new String(convertToHexCharArray(from)));
	}

	private static char hex_chars[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
}
